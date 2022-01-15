#include <stdlib.h>

#include "main.h"
#include "encrypt.h"
#include "data_blocks.h"

/**
 * Steps:
 * looping through every data byte... 
 * 1	1. += one byte from the key (increments & wraps back to beginning)
 * 1	2. XOR with 0xe7
 * 1	3. shift bits left 2 notches, wrap the 2 bits pushed off to the right side of the byte
 * 0	4. separate bytes into blocks, padd 0's to the end of the block, store the actual data's size in the last 3 bytes, shift around the rows and columns
*/

struct Data encryptData(struct Data *data, char *key)
{
	int keyLen = strlen(key);
	int keyCursor = 0;
	Byte temp = '\0';
	struct DataBlock *block;
	int numBlocks;
	struct DataBlock block = {.width = 10, .height = 10}; // XXX should be derived from key
	
	for(int i=0; i<data.size; i++)
	{
		data->ptr[i] += key[keyCursor++];
		if(keyCursor > keyLen - 1) {keyCursor = 0;}
		
		data->ptr[i] ^= 0xe7;
		
		temp = data->ptr[i];
		data->ptr[i] <<= 2;
		data->ptr[i] += (temp >> 6);
	}
	
	// numBlocks = size / (blockWidth * blockHeight) + 1;
	// block = malloc(numBlocks * sizeof(struct DataBlock));
	block->data = malloc(block.width * block.height);
	
	int i;
	
	for(i=0; i<data.size; i++)
	{
		setByte(data->ptr[i], block, i%blockWidth, i/blockHeight);
	}
	
	// padding
	for(i=i; i<blockWidth * blockHeight - sizeof(int); i++)
	{
		setByte('\0', block, i%blockWidth, i/blockHeight);
	}
	*((int) block->data[i]) = data.size;
	
	// scramble data
	// shiftCol(block, 0, 1, SHIFT_UP);
	
	struct Data ret = {.ptr = block->data, .size = block->width * block->height};
	
	return Data;
}

/** does the opposite of the encryption (obv)
 * looping through every data byte...
 * 	1. shift bits right 2 notches, wrap the two bits pushed off to the left side of the byte
 * 	2. XOR with 0xe7
 * 	3. -= one byte from the key (increments & wraps around back to beginning)
**/

struct Data decryptData(struct Data *data, char *key)
{
	int keyLen = strlen(key);
	int keyCursor = 0;
	Byte temp = '\0';
	
	for(int i=0; i<data.size; i++)
	{
		// 0000 0011
		temp = data->ptr[i];
		data->ptr[i] >>= 2;
		data->ptr[i] += (temp << 6);
		
		data->ptr[i] ^= 0xe7;
		
		data->ptr[i] -= key[keyCursor++];
		if(keyCursor > keyLen - 1) {keyCursor = 0;}
	}
	
	return ret;
}