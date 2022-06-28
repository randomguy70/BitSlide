#include "../include/encrypt.h"

#include "../include/data.h"
#include "../include/data_blocks.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct Data *encryptData(struct Data *data, char *key)
{
	struct Data *ret;
	struct DataBlock *block1;
	
	block1 = dataToBlocks(data, false);
	scrambleBlockData(block1, key);
	ret = blocksToData(block1, true);
	if(ret == NULL)
	{
		printf("Bad return. Encryption failed\n");
		return NULL;
	}
	return ret;
}

struct Data *decryptData(struct Data *data, char *key)
{
	struct Data *ret;
	struct DataBlock *block1;
	
	printf("data to blocks\n");
	block1 = dataToBlocks(data, true);
	printf("made data into blocks\n");
	if(block1 == NULL)
	{
		printf("block1 contains null pointer\n");
		return NULL;
	}
	printf("unscramble data\n");
	// unscrambleBlockData(block1, key);
	ret = blocksToData(block1, false);
	
	return ret;
}