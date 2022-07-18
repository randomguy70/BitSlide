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
	printf("encrypting\n");
	
	struct Data *ret;
	struct DataBlock *first, *block;
	uint32_t hashData[64];
	uint32_t row, col, rowTicks, colTicks, rowDir, colDir, keyLen;
	
	keyLen = strlen(key);
	
	// get 8 iterations of SHA_256 hash of key
	
	sha256(key, strlen(key), (uint8_t*)hashData);
	
	for(int i = 1; i < ((int)sizeof(hashData) / SHA256_SIZE_BYTES); i++)
	{
		sha256(hashData + (i-1)*SHA256_SIZE_BYTES, SHA256_SIZE_BYTES, (uint8_t*)hashData + i*SHA256_SIZE_BYTES);
	}
	
	doByteSubstitution(data, key);
	
	first = dataToBlocks(data, false);
	
	// main encryption process
	
	block = first;
	
	do {
		for(uint32_t i = 0; i < 32; i++)
		{
			row       = hashData[(i | (i ^ i)) & 0xff] & (BLOCK_HEIGHT - 1);
			col       = hashData[(i ^ row) & 0xff] & (BLOCK_WIDTH - 1);
			rowTicks  = hashData[hashData[i & 0xff] & 0xff] & (BLOCK_HEIGHT - 1);
			colTicks  = hashData[((i << (key[i & (keyLen - 1)] & 0xf)) ^ (row ^ col)) & 0xff] & (BLOCK_WIDTH - 1);
			rowDir    = (hashData[(row * row ^ col) & 0xff]) & 0x1;
			colDir    = (hashData[(colTicks * colTicks) & 0xff] & 0x1) + 2;
			
			printf("row: %u, col: %u, rowTicks: %u, colTicks: %u, rowDir: %u, colDir: %u\n", row, col, rowTicks, colTicks, rowDir, colDir);
			
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
	uint32_t hashData[64];
	uint32_t row, col, rowTicks, colTicks, rowDir, colDir, keyLen;
	
	keyLen = strlen(key);
	
	// get 8 iterations of SHA_256 hash of key
	
	sha256(key, strlen(key), (uint8_t *)hashData);
	
	for(int i = 1; i < (int)(sizeof(hashData) / SHA256_SIZE_BYTES); i++)
	{
		sha256(hashData + (i-1)*SHA256_SIZE_BYTES, SHA256_SIZE_BYTES, (uint8_t*) hashData + i*SHA256_SIZE_BYTES);
	}
	
	first = dataToBlocks(data, true);
	
	// main decryption process
	
	block = first;
	
	do {
		for(uint32_t i = 0; i < 32; i++)
		{
			row       = hashData[(i | (i ^ i)) & 0xff] & (BLOCK_HEIGHT - 1);
			col       = hashData[(i ^ row) & 0xff] & (BLOCK_WIDTH - 1);
			rowTicks  = hashData[hashData[i & 0xff] & 0xff] & (BLOCK_HEIGHT - 1);
			colTicks  = hashData[((i << (key[i & (keyLen - 1)] & 0xf)) ^ (row ^ col)) & 0xff] & (BLOCK_WIDTH - 1);
			rowDir    = (hashData[(row * row ^ col) & 0xff]) & 0x1;
			colDir    = (hashData[(colTicks * colTicks) & 0xff] & 0x1) + 2;
			
			if(rowDir == SHIFT_LEFT)
			{
				rowDir = SHIFT_RIGHT;
			}
			else if(rowDir == SHIFT_RIGHT)
			{
				rowDir = SHIFT_LEFT;
			}
			else {printf("rowDir is off\n");}
			
			if(colDir == SHIFT_UP)
			{
				colDir = SHIFT_DOWN;
			}
			else if(colDir == SHIFT_DOWN)
			{
				colDir = SHIFT_UP;
			}
			else {printf("colDir is off\n");}
			
			printf("row: %u, col: %u, rowTicks: %u, colTicks: %u, rowDir: %u, colDir: %u\n", row, col, rowTicks, colTicks, rowDir, colDir);
			
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
	
	for(unsigned int i = 0; i < data->size; i++)
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
	
	for(unsigned int i = 0; i < (int)sizeof(table); i++)
	{
		randIndex = hashTable[i];
		temp = table[i];
		table[i] = table[randIndex];
		table[randIndex] = temp;
	}
	
	// calculate the inverse of the table
	
	for(unsigned int i = 0; i < (int)sizeof(table); i++)
	{
		inverseTable[table[i]] = i;
	}
	
	// replace every byte you're decrypting with a byte from the table using the original byte as the index
	
	for(unsigned int i = 0; i < data->size; i++)
	{
		data->ptr[i] = inverseTable[data->ptr[i]];
	}
}

