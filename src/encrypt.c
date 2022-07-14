#include "../include/encrypt.h"

#include "../include/data.h"
#include "../include/data_blocks.h"
#include "../include/sha256.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void doByteSubstitution    (struct Data* data, char* key);
void undoByteSubstitution  (struct Data* data, char* key);

struct Data *encryptData(struct Data* data, char* key)
{
	struct Data *ret;
	struct DataBlock *first, *temp;
	uint32_t hashData[64];
	uint32_t counter = 0, a, b, c, d;
	
	// get 8 iterations of SHA_256 hash of key
	
	sha256(key, strlen(key), hashData);
	
	for(int i = 1; i < (sizeof(hashData) / SHA256_SIZE_BYTES); i++)
	{
		sha256(hashData + (i-1)*SHA256_SIZE_BYTES, SHA256_SIZE_BYTES, hashData + i*SHA256_SIZE_BYTES);
	}
	
	doByteSubstitution(data, key);
	
	first = dataToBlocks(data, false);
	
	// main encryption process
	
	temp = first;
	
	do {
		for(uint32_t i = 0; i < 64; i++)
		{
			a = hashData[counter % sizeof(hashData)];
			b = hashData[(i * a) % sizeof(hashData)];
			c = hashData[hashData[i] % sizeof(hashData)];
			d = (i << (a && 0xf)) ^ (b);
			
			shiftRow(temp, a, b, c % 2+2);
			shiftCol(temp, d, c, b % 2);
			
			counter++;
		}
	}
	while ((temp = temp->next) != NULL);
	
	ret = blocksToData(first, true);
	if(ret == NULL)
	{
		return NULL;
	}
	return ret;
}

struct Data *decryptData(struct Data* data, char* key)
{
	struct Data *ret;
	struct DataBlock *first, *temp, **blockArr;
	uint32_t hashData[64];
	uint32_t counter, a, b, c, d;
	int numBlocks = 64, curBlock;
	
	// get 8 iterations of SHA_256 hash of key
	
	sha256(key, strlen(key), hashData);
	
	for(int i = 1; i < (sizeof(hashData) / SHA256_SIZE_BYTES); i++)
	{
		sha256(hashData + (i-1)*SHA256_SIZE_BYTES, SHA256_SIZE_BYTES, hashData + i*SHA256_SIZE_BYTES);
	}
	
	first = dataToBlocks(data, true);
	if(first == NULL)
	{
		printf("data to blocks process failed\n");
		return NULL;
	}
	
	temp = first;
	
	// generate array of block pointers to optimize working backwards
	numBlocks = getNumBlocks(first);
	curBlock = numBlocks - 1;
	counter = numBlocks * 64;
	blockArr = malloc(sizeof(struct DataBlock*) * numBlocks);
	
	for(int i = 0; i < numBlocks; i++)
	{
		blockArr[i] = temp;
		temp = temp->next;
	}
	
	do {
		temp = blockArr[curBlock];
		
		for(uint32_t i = 63; i >= 0; i--)
		{
			a = hashData[counter % sizeof(hashData)];
			b = hashData[(i * a) % sizeof(hashData)];
			c = hashData[hashData[i] % sizeof(hashData)];
			d = (i << (a & 0xf)) ^ (b);
			
			shiftCol(temp, d, c, !(b % 2));
			shiftRow(temp, a, b, !(c % 2)+2);
			
			counter--;
		}
	}
	while (--curBlock > -1);
	
	ret = blocksToData(first, false);
	undoByteSubstitution(ret, key);
	
	free(blockArr);
	
	return ret;
}

void doByteSubstitution(struct Data* data, char* key)
{
	uint8_t table[256];
	uint8_t hashTable[256];
	uint8_t temp, randIndex;
	
	// get 8 iterations of SHA_256 hash of key
	
	sha256(key, strlen(key), hashTable);
	
	for(int i = 1; i < (sizeof(hashTable) / SHA256_SIZE_BYTES); i++)
	{
		sha256(hashTable + (i-1)*SHA256_SIZE_BYTES, SHA256_SIZE_BYTES, hashTable + i*SHA256_SIZE_BYTES);
	}
	
	// generate 16*16 matrice containing all 1-Byte values (0x00 to 0xff)
	
	for(int i = 0; i < sizeof(table); i++)
	{
		table[i] = i;
	}
	
	// use the Fisher–Yates shuffle (using the hash table) to randomize values in the matrice
	
	for(int i = 0; i < sizeof(table); i++)
	{
		randIndex = hashTable[i];
		temp = table[i];
		table[i] = table[randIndex];
		table[randIndex] = temp;
	}
	
	// replace every byte you're encrypting with a byte from the table using the original byte as the index
	
	for(int i = 0; i < data->size; i++)
	{
		data->ptr[i] = table[data->ptr[i]];
	}
}

void undoByteSubstitution(struct Data* data, char* key)
{
	uint8_t table[256];
	uint8_t inverseTable[256];
	uint8_t hashTable[256];
	uint8_t temp, randIndex;
	
	// get 8 iterations of SHA_256 hash of key
	
	sha256(key, strlen(key), hashTable);
	
	for(int i = 1; i < (sizeof(hashTable) / SHA256_SIZE_BYTES); i++)
	{
		sha256(hashTable + (i-1)*SHA256_SIZE_BYTES, SHA256_SIZE_BYTES, hashTable + i*SHA256_SIZE_BYTES);
	}
	
	// generate 16*16 matrice containing all 1-Byte values (0x00 to 0xff)
	
	for(int i = 0; i < sizeof(table); i++)
	{
		table[i] = i;
	}
	
	// use the Fisher–Yates shuffle (using the hash table) to randomize values in the matrice
	
	for(int i = 0; i < sizeof(table); i++)
	{
		randIndex = hashTable[i];
		temp = table[i];
		table[i] = table[randIndex];
		table[randIndex] = temp;
	}
	
	// calculate the inverse of the table
	
	for(int i = 0; i < sizeof(table); i++)
	{
		inverseTable[table[i]] = i;
	}
	
	// replace every byte you're decrypting with a byte from the table using the original byte as the index
	
	for(int i = 0; i < data->size; i++)
	{
		data->ptr[i] = inverseTable[data->ptr[i]];
	}
}

