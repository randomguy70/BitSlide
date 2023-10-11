#include "../include/encrypt.h"

#include "../include/file.h"
#include "../include/data.h"
#include "../include/data_blocks.h"
#include "../include/sha256.h"
#include "../include/password.h"

#include <stdio.h>
#include <string.h>

uint32_t encryptData(struct file* file, struct password *password)
{
	uint32_t row, col, rowTicks, colTicks, rowDir, colDir, val;
	
	doByteSubstitution(&file->data, password->shaArray);
	
	// data scrambling
	
	for(uint32_t blockOffset = 0; blockOffset < file->data.size; blockOffset+=256)
	{
		for(uint32_t round = 0; round < ENCRYPTION_ROUNDS; round++)
		{
			val = password->shaArray[round & (SHA_ARRAY_WORD_LEN - 1)];
			
			row      = (val >> 0 ) & (BLOCK_WIDTH  - 1);
			col      = (val >> 4 ) & (BLOCK_HEIGHT - 1);
			rowTicks = (val >> 8 ) & (BLOCK_WIDTH  - 1);
			colTicks = (val >> 12) & (BLOCK_HEIGHT - 1);
			rowDir   = (val >> 16) & 0x1;
			colDir   = (val >> 17) & 0x1;
						
			shiftRow(file->data.ptr + blockOffset, row, rowTicks, rowDir);
			shiftCol(file->data.ptr + blockOffset, col, colTicks, colDir);
		}
	}
	
	return 1;
}

uint32_t decryptData(struct file* file, struct password *password)
{		
	uint32_t row, col, rowTicks, colTicks, rowDir, colDir, val;
		
	// main decryption process
			
	for(uint32_t block = 0; block < file->data.size; block += 256)
	{
		for(uint32_t round = ENCRYPTION_ROUNDS - 1, k = ENCRYPTION_ROUNDS; k > 0; round--, k--)
		{
			val = password->shaArray[round & (SHA_ARRAY_WORD_LEN - 1)];
			
			row      = (val >> 0 ) & (BLOCK_WIDTH  - 1);
			col      = (val >> 4 ) & (BLOCK_HEIGHT - 1);
			rowTicks = (val >> 8 ) & (BLOCK_WIDTH  - 1);
			colTicks = (val >> 12) & (BLOCK_HEIGHT - 1);
			rowDir   = (val >> 16) & 0x1;
			colDir   = (val >> 17) & 0x1;
			
			// invert the scrambling
			
			rowDir ^= 0x1;
			colDir ^= 0x1;
						
			shiftCol(file->data.ptr + block, col, colTicks, colDir);
			shiftRow(file->data.ptr + block, row, rowTicks, rowDir);
		}
	}
	
	undoByteSubstitution(&file->data, password->shaArray);
	
	file->data.size = *(uint32_t*)(file->data.ptr + file->data.size - (uint32_t)(sizeof(uint32_t)));
	
	return 1;
}

void doByteSubstitution(struct Data *data, uint8_t *shaArray)
{
	const tableSize = 256;
	uint8_t table[256];
	uint8_t temp, randIndex;
	
	// generate matrice containing all 1-Byte values (0x00 to 0xff)
	
	for(int i = 0; i < tableSize; i++)
	{
		table[i] = i;
	}
	
	// use the Fisher–Yates shuffle (using the hash table) to randomize values in the matrice
	
	for(int i = 0; i < (int)sizeof(table); i++)
	{
		randIndex = shaArray[i];
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

void undoByteSubstitution(struct Data *data, uint8_t *shaArray)
{
	const tableSize = 256;
	uint8_t table[256];
	uint8_t inverseTable[256];
	uint8_t temp, randIndex;
	
	// generate matrice containing all 1-Byte values (0x00 to 0xff)
	
	for(int i = 0; i < tableSize; i++)
	{
		table[i] = i;
	}
	
	// use the Fisher–Yates shuffle (using the hash table) to randomize values in the matrice
	
	for(uint32_t i = 0; i < tableSize; i++)
	{
		randIndex = shaArray[i];
		temp = table[i];
		table[i] = table[randIndex];
		table[randIndex] = temp;
	}
	
	// calculate the inverse of the table
	
	for(uint32_t i = 0; i < tableSize; i++)
	{
		inverseTable[table[i]] = i;
	}
	
	// replace every byte you're decrypting with a byte from the table using the original byte as the index
	
	for(uint32_t i = 0; i < data->size; i++)
	{
		data->ptr[i] = inverseTable[data->ptr[i]];
	}
}
