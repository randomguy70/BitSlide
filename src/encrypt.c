#include "../include/encrypt.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/data.h"
#include "../include/data_blocks.h"

struct Data *encryptData(struct Data *data, char *key)
{
	struct Data *ret;
	// int keyLen = strlen(key);
	// int keyCursor = 0;
	// Byte temp;
	struct DataBlock *block1;
	
	key = NULL; // only to prevent the parameter warning

	// for(int i=0; i<data->size; i++)
	// {
		// data->ptr[i] += key[keyCursor++];
		// if(keyCursor > keyLen - 1) {keyCursor = 0;}
		//
		// data->ptr[i] ^= 0xe7;
		//
		// temp = data->ptr[i];
		// data->ptr[i] <<= 2;
		// data->ptr[i] += (temp >> 6);
	// }
	
	block1 = dataToBlocks(data);
	printf("converted data to blocks\n");
	
	printf("shifting left\n");
	shiftRow(block1, 1, 1, SHIFT_RIGHT);
	shiftRow(block1, 1, 11, SHIFT_RIGHT);
	
	/*
	success = shiftRow(block1, 1, 1, SHIFT_RIGHT);
	if(!success)
	{
		printf("shifting right failed\n");
		return 0;
	}
	*/
	
	ret = blocksToData(block1);
	printf("converted blocks to data\n");
	
	return ret;
}

struct Data *decryptData(struct Data *data, char *key)
{
	struct Data *ret = malloc(sizeof(struct Data));
	int keyLen = strlen(key);
	int keyCursor = 0;
	Byte temp = '\0';

	ret->ptr = NULL;
	ret->size = 0;

	for(int i=0; i<data->size; i++)
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