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
	shiftCol(block1, 1, 1, SHIFT_UP);
	shiftCol(block1, 1, 1, SHIFT_DOWN);

	printBlocks(block1);

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