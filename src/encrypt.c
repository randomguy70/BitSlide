#include "../include/encrypt.h"

#include "../include/data.h"
#include "../include/data_blocks.h"
#include "../include/sha256.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

void doByteSubstitution    (struct Data* data, char* key);
void undoByteSubstitution  (struct Data* data, char* key);

struct Data *encryptData(struct Data* data, char* key)
{
	printf("encrypting\n");
	
	struct Data *ret;
	struct DataBlock *first, *block;
	uint32_t hashData[(SHA256_SIZE_BYTES / sizeof(uint32_t)) * 8] = {0};
	const uint32_t hashDataLen = sizeof(hashData) / sizeof(uint32_t);
	uint32_t row, col, rowTicks, colTicks, rowDir, colDir;
	uint32_t i3, i5;
	
	// seed the table & fill it with random numbers
	
	strcpy((char*)hashData, key);
	sha256Table((uint8_t*)hashData, sizeof(hashData));
	
	doByteSubstitution(data, key);
	first = dataToBlocks(data, false);
	
	// main encryption process
	
	block = first;
	
	do {
		for(uint32_t i = 0; i < ENCRYPTION_ROUNDS; i++)
		{
			i3 = i  * i * i;
			i5 = i3 * i * i;
			
			row      = hashData[ i  % hashDataLen] & (BLOCK_WIDTH  - 1);
			col      = hashData[~i  % hashDataLen] & (BLOCK_HEIGHT - 1);
			rowTicks = hashData[ i3 % hashDataLen] & (BLOCK_WIDTH  - 1);
			colTicks = hashData[~i3 % hashDataLen] & (BLOCK_HEIGHT - 1);
			rowDir   = hashData[ i5 % hashDataLen] & 0x1;
			colDir   = hashData[~i5 % hashDataLen] & 0x1;
			
			shiftRow(block, row, rowTicks, rowDir);
			shiftCol(block, col, colTicks, colDir);
		}
		block = block->next;
	}
	while (block != NULL);
	
	ret = blocksToData(first, true);
	
	if(ret == NULL)
	{
		return NULL;
	}
	return ret;
}

struct Data *decryptData(struct Data* data, char* key)
{
	printf("decrypting\n");
	
	struct Data *ret;
	struct DataBlock *first, *block;
	uint32_t hashData[(SHA256_SIZE_BYTES / sizeof(uint32_t)) * 8] = {0};
	const uint32_t hashDataLen = sizeof(hashData) / sizeof(uint32_t);
	uint32_t row, col, rowTicks, colTicks, rowDir, colDir;
	uint32_t i3, i5;
	
	// seed the table & fill it with random numbers
	
	strcpy((char*)hashData, key);
	sha256Table((uint8_t*)hashData, sizeof(hashData));
	// printSHA256Table((uint8_t*)hashData, sizeof(hashData));
	
	first = dataToBlocks(data, true);
	
	// main decryption process
	
	printf("Unscrambling Bytes\n");
	
	block = first;
	
	do {
		for(uint32_t j = 0, i = ENCRYPTION_ROUNDS - 1; j < ENCRYPTION_ROUNDS; j++, i--)
		{
			i3 = i * i * i;
			i5 = i3 * i * i;
			
			row      = hashData[ i  % hashDataLen] & (BLOCK_WIDTH  - 1);
			col      = hashData[~i  % hashDataLen] & (BLOCK_HEIGHT - 1);
			rowTicks = hashData[ i3 % hashDataLen] & (BLOCK_WIDTH  - 1);
			colTicks = hashData[~i3 % hashDataLen] & (BLOCK_HEIGHT - 1);
			rowDir   = hashData[ i5 % hashDataLen] & 0x1;
			colDir   = hashData[~i5 % hashDataLen] & 0x1;
			
			// invert the scrambling
			
			rowDir ^= 0x1;
			colDir ^= 0x1;
			
			shiftCol(block, col, colTicks, colDir);
			shiftRow(block, row, rowTicks, rowDir);
		}
		block = block->next;
	}
	while (block != NULL);
	
	ret = blocksToData(first, false);
	undoByteSubstitution(ret, key);
	
	return ret;
}
			
void doByteSubstitution(struct Data* data, char* key)
{
	uint8_t table[256];
	uint8_t hashTable[256];
	uint8_t temp, randIndex;
	
	// get 8 iterations of SHA_256 hash of key
	
	sha256(key, strlen(key), hashTable);
	
	for(int i = 1; i < (int)(sizeof(hashTable) / SHA256_SIZE_BYTES); i++)
	{
		sha256(hashTable + (i-1)*SHA256_SIZE_BYTES, SHA256_SIZE_BYTES, hashTable + i*SHA256_SIZE_BYTES);
	}
	
	// generate 16*16 matrice containing all 1-Byte values (0x00 to 0xff)
	
	for(int i = 0; i < (int)sizeof(table); i++)
	{
		table[i] = i;
	}
	
	// use the Fisher–Yates shuffle (using the hash table) to randomize values in the matrice
	
	for(int i = 0; i < (int)sizeof(table); i++)
	{
		randIndex = hashTable[i];
		temp = table[i];
		table[i] = table[randIndex];
		table[randIndex] = temp;
	}
	
	// replace every byte you're encrypting with a byte from the table using the original byte as the index
	
	for(uint32_t i = 0; i < data->size; i++)
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
	
	for(int i = 1; i < (int)(sizeof(hashTable) / SHA256_SIZE_BYTES); i++)
	{
		sha256(hashTable + (i-1)*SHA256_SIZE_BYTES, SHA256_SIZE_BYTES, hashTable + i*SHA256_SIZE_BYTES);
	}
	
	// generate 16*16 matrice containing all 1-Byte values (0x00 to 0xff)
	
	for(int i = 0; i < (int)sizeof(table); i++)
	{
		table[i] = i;
	}
	
	// use the Fisher–Yates shuffle (using the hash table) to randomize values in the matrice
	
	for(uint32_t i = 0; i < (int)sizeof(table); i++)
	{
		randIndex = hashTable[i];
		temp = table[i];
		table[i] = table[randIndex];
		table[randIndex] = temp;
	}
	
	// calculate the inverse of the table
	
	for(uint32_t i = 0; i < (int)sizeof(table); i++)
	{
		inverseTable[table[i]] = i;
	}
	
	// replace every byte you're decrypting with a byte from the table using the original byte as the index
	
	for(uint32_t i = 0; i < data->size; i++)
	{
		data->ptr[i] = inverseTable[data->ptr[i]];
	}
}
