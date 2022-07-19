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

static const uint8_t K[256] = {
	0x42, 0x8a, 0x2f, 0x98, 0x71, 0x37, 0x44, 0x91, 0xb5, 0xc0, 0xfb, 0xcf, 0xe9, 0xb5, 0xdb, 0xa5,
	0x39, 0x56, 0xc2, 0x5b, 0x59, 0xf1, 0x11, 0xf1, 0x92, 0x3f, 0x82, 0xa4, 0xab, 0x1c, 0x5e, 0xd5,
	0xd8, 0x07, 0xaa, 0x98, 0x12, 0x83, 0x5b, 0x01, 0x24, 0x31, 0x85, 0xbe, 0x55, 0x0c, 0x7d, 0xc3,
	0x72, 0xbe, 0x5d, 0x74, 0x80, 0xde, 0xb1, 0xfe, 0x9b, 0xdc, 0x06, 0xa7, 0xc1, 0x9b, 0xf1, 0x74,
	0xe4, 0x9b, 0x69, 0xc1, 0xef, 0xbe, 0x47, 0x86, 0x0f, 0xc1, 0x9d, 0xc6, 0x24, 0x0c, 0xa1, 0xcc,
	0x2d, 0xe9, 0x2c, 0x6f, 0x4a, 0x74, 0x84, 0xaa, 0x5c, 0xb0, 0xa9, 0xdc, 0x76, 0xf9, 0x88, 0xda,
	0x98, 0x3e, 0x51, 0x52, 0xa8, 0x31, 0xc6, 0x6d, 0xb0, 0x03, 0x27, 0xc8, 0xbf, 0x59, 0x7f, 0xc7,
	0xc6, 0xe0, 0x0b, 0xf3, 0xd5, 0xa7, 0x91, 0x47, 0x06, 0xca, 0x63, 0x51, 0x14, 0x29, 0x29, 0x67,
	0x27, 0xb7, 0x0a, 0x85, 0x2e, 0x1b, 0x21, 0x38, 0x4d, 0x2c, 0x6d, 0xfc, 0x53, 0x38, 0x0d, 0x13,
	0x65, 0x0a, 0x73, 0x54, 0x76, 0x6a, 0x0a, 0xbb, 0x81, 0xc2, 0xc9, 0x2e, 0x92, 0x72, 0x2c, 0x85,
	0xa2, 0xbf, 0xe8, 0xa1, 0xa8, 0x1a, 0x66, 0x4b, 0xc2, 0x4b, 0x8b, 0x70, 0xc7, 0x6c, 0x51, 0xa3,
	0xd1, 0x92, 0xe8, 0x19, 0xd6, 0x99, 0x06, 0x24, 0xf4, 0x0e, 0x35, 0x85, 0x10, 0x6a, 0xa0, 0x70,
	0x19, 0xa4, 0xc1, 0x16, 0x1e, 0x37, 0x6c, 0x08, 0x27, 0x48, 0x77, 0x4c, 0x34, 0xb0, 0xbc, 0xb5,
	0x39, 0x1c, 0x0c, 0xb3, 0x4e, 0xd8, 0xaa, 0x4a, 0x5b, 0x9c, 0xca, 0x4f, 0x68, 0x2e, 0x6f, 0xf3,
	0x74, 0x8f, 0x82, 0xee, 0x78, 0xa5, 0x63, 0x6f, 0x84, 0xc8, 0x78, 0x14, 0x8c, 0xc7, 0x02, 0x08,
	0x90, 0xbe, 0xff, 0xfa, 0xa4, 0x50, 0x6c, 0xeb, 0xbe, 0xf9, 0xa3, 0xf7, 0xc6, 0x71, 0x78, 0xf2
};

struct Data *encryptData(struct Data* data, char* key)
{
	printf("encrypting\n");
	
	struct Data *ret;
	struct DataBlock *first, *block;
	uint8_t hashData[256];
	uint32_t row, col, rowTicks, colTicks, rowDir, colDir, keyLen, counter = 0;
	
	keyLen = strlen(key);
	
	// get 8 iterations of SHA_256 hash of key
	
	sha256(key, strlen(key), (uint8_t*)hashData);
	
	for(int i = 1; i < ((int)sizeof(hashData) / SHA256_SIZE_BYTES); i++)
	{
		sha256(hashData + (i-1)*SHA256_SIZE_BYTES, SHA256_SIZE_BYTES, hashData + i*SHA256_SIZE_BYTES);
	}
	
	doByteSubstitution(data, key);
	
	first = dataToBlocks(data, false);
	
	// main encryption process
	
	block = first;
	
	do {
		for(uint32_t i = 0; i < ENCRYPTION_ROUNDS; i++)
		{
			row      = hashData[(~(i) * K[counter & 0xff]) & 0xff];
			col      = hashData[(i * keyLen ^ K[i & 0xff]) & 0xff];
			rowTicks = hashData[((~row) ^ K[col] ^ counter) & 0xff];
			colTicks = hashData[(counter ^ hashData[counter & 0xff]) & 0xff];
			rowDir   = (hashData[((counter * counter) ^ (~hashData[i & 0xff])) & 0xff] & 0x1) + 2;
			colDir   = hashData[(K[counter & 0xff] ^ (K[(~i) & 0xff])) & 0xff] & 0x1;
			
			printf("row: %u, col: %u, rowTicks: %u, colTicks: %u, rowDir: %u, colDir: %u\n", row, col, rowTicks, colTicks, rowDir, colDir);
			
			shiftRow(block, row, rowTicks, rowDir);
			shiftCol(block, col, colTicks, colDir);
			
			counter++;
		}
		block = block->next;
	}
	while (block != NULL);
	printf("counter: %u", counter);
	
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
	uint8_t hashData[256];
	uint32_t row, col, rowTicks, colTicks, rowDir, colDir, keyLen, counter, numBlocks;
	
	keyLen = strlen(key);
	
	// get 8 iterations of SHA_256 hash of key
	
	sha256(key, strlen(key), hashData);
	
	for(int i = 1; i < (int)(sizeof(hashData) / SHA256_SIZE_BYTES); i++)
	{
		sha256(hashData + (i-1)*SHA256_SIZE_BYTES, SHA256_SIZE_BYTES, hashData + i*SHA256_SIZE_BYTES);
	}
	
	first = dataToBlocks(data, true);
	numBlocks = getNumBlocks(first);
	counter = ENCRYPTION_ROUNDS * numBlocks - 1;
	
	// main decryption process
	
	block = first;
	
	do {
		for(uint32_t i = ENCRYPTION_ROUNDS - 1 ; i <= 0; i--)
		{
			row      = hashData[(~(i) * K[counter & 0xff]) & 0xff];
			col      = hashData[(i * keyLen ^ K[i & 0xff]) & 0xff];
			rowTicks = hashData[((~row) ^ K[col] ^ counter) & 0xff];
			colTicks = hashData[(counter ^ hashData[counter & 0xff]) & 0xff];
			rowDir   = (hashData[((counter * counter) ^ (~hashData[i & 0xff])) & 0xff] & 0x1) + 2;
			colDir   = hashData[(K[counter & 0xff] ^ (K[(~i) & 0xff])) & 0xff] & 0x1;
			
			// flipping the least bit happens to change both directions
			rowDir ^= 0x1;
			colDir ^= 0x1;
			
			printf("row: %u, col: %u, rowTicks: %u, colTicks: %u, rowDir: %u, colDir: %u\n", row, col, rowTicks, colTicks, rowDir, colDir);
			
			shiftCol(block, col, colTicks, colDir);
			shiftRow(block, row, rowTicks, rowDir);
			
			counter--;
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

