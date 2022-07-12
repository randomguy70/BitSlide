#include "../include/encrypt.h"

#include "../include/data.h"
#include "../include/data_blocks.h"
#include "../include/sha256.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void doByteSubstitution(struct Data* data, char* key);

struct Data *encryptData(struct Data* data, char* key)
{
	struct Data *ret;
	struct DataBlock *block1;
	
	doByteSubstitution(data, key);
	
	block1 = dataToBlocks(data, false);
	// scrambleBlockData(block1, key);
	ret = blocksToData(block1, true);
	if(ret == NULL)
	{
		printf("Bad return. Encryption failed\n");
		return NULL;
	}
	return ret;
}

struct Data *decryptData(struct Data* data, char* key)
{
	struct Data *ret;
	struct DataBlock *block1;
	
	printf("data to blocks\n");
	block1 = dataToBlocks(data, true);
	if(block1 == NULL)
	{
		printf("data to blocks process failed\n");
		return NULL;
	}
	printf("\nunscramble data\n");
	// unscrambleBlockData(block1, key);
	ret = blocksToData(block1, false);
	
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
}

void doRound(struct DataBlock* first, int iteration, char* key)
{
	
}