#include <stdio.h>
#include <stdlib.h>

#include "../include/data_blocks.h"
#include "../include/main.h"

struct DataBlock *dataToBlocks(struct Data *data)
{
	Byte *array;
	int arraySize;
	struct DataBlock *block1, *block;
	const int width = 12, height = 10; // width should be a multiple of sizeof(int), and height should be given as a parameter
	const int blockSize = width * height;
	int numBlocks = (data->size + sizeof(int)) / blockSize;

	if((data->size + sizeof(int)) % blockSize)
	{
		numBlocks++;
	}

	printf("numBlocks: %d\n", numBlocks);

	block1 = malloc(sizeof(struct DataBlock));
	block = block1;

	// initialise block properties

	printf("initialising blocks\n");

	for(int i=1; i <= numBlocks; i++)
	{
		block->width = width;
		block->height = height;

		if(i == numBlocks)
		{
			block->next = NULL;
			break;
		}
		else
		{
			block->next = malloc(sizeof(struct DataBlock));
			block = block->next;
		}
	}

	// copy data into blocks

	printf("copying data into blocks\n");

	arraySize = blockSize * numBlocks;
	array = malloc(arraySize);

	copyBytes(array, data->ptr, data->size);

	// fill the end of the data with 0's

	for(Byte *i = array + data->size; i < array + arraySize - 1 - sizeof(int); i++)
	{
		*i = 0;
	}

	// store the size at the end of the data

	*((int*) (array + arraySize - 1 - sizeof(int))) = data->size;

	block = block1;

	// make the blocks' pointers point straight to the data

	for(int i = 0; i < numBlocks; i++)
	{
		block->data = array + (i * blockSize);

		block = block->next;
	}

	return block1;
}

struct Data *blocksToData(struct DataBlock *first)
{
	struct DataBlock *block = first;
	struct Data *data = malloc(sizeof(struct Data));
	const int blockSize = first->width * first->height;
	int bytesCopied = 0;
	int numBlocks = 1;
	int *sizePtr;

	// get data size (stored in the last 4 bytes of the last block)

	while(block->next != NULL)
	{
		block = block->next;
		numBlocks++;
	}
	sizePtr = (int*) (block->data + blockSize - 1 - sizeof(int));
	data->size = *sizePtr;
	data->ptr = malloc(data->size);

	printf("counted %d blocks\n", numBlocks);
	printf("data size: %d\n", data->size);

	// copy the data from every block except the last

	printf("copying blocks into data struct\n");

	block = first;

	for(int i = 0; i < numBlocks - 1; i++)
	{
		copyBytes(data->ptr + bytesCopied, block->data, blockSize);
		bytesCopied += blockSize;
		block = block->next;
	}

	// copy the data from the last block (minus the last 4 bytes)

	printf("copying last block\n");

	copyBytes(data->ptr + bytesCopied, block->data, blockSize - sizeof(int));

	freeBlocks(first);

	return data;
}

int freeBlocks(struct DataBlock *first)
{
	int i = 1;
	struct DataBlock *temp;

	// since the data in all the blocks is actually a single block in memory, the first block's data pointer is technically the pointer to all of the data

	free(first->data);

	while(first->next != NULL)
	{
		temp = first->next;
		free(first);
		first = temp;
		i++;
	}

	return i;
}

int copyBytes(Byte *dest, Byte *src, int len)
{
	int i;

	for(i=0; i<len; i++)
	{
		dest[i] = src[i];
	}

	return i;
}

int shiftCol(struct DataBlock *block, int col, int ticks, enum DIRECTION dir)
{
	Byte *tempCol = malloc(block->height);

	if(ticks == block->height)
	{
		return 0;
	}
	else if(ticks > block->height)
	{
		ticks %= block->height;
	}

	// 0101 1010 1101 0100
	// 1101 0100 0101 1010
	// 0101 1010 1101 0100
	// 1101 0100 0101 1010
	// 1101 0100 0101 1010
	// 1101 0100 0101 1010

	// copy the column to preserve the wrapped bytes
	for(int i=0; i<block->height; i++)
	{
		tempCol[i] = getByte(block, col, i);
	}

	// shift down (untested)
	if(dir == SHIFT_DOWN)
	{
		for(int i=block->height - ticks - 1; i >= 0; i-=1)
		{
			Byte byte = getByte(block, col, i);
			setByte(byte, block, col, i+ticks);
		}
		// wrap bytes
		for(int i=ticks - 1, j = block->height - 1; i >=0; i--, j--)
		{
			Byte byte = tempCol[j];
			setByte(byte, block, col, i);
		}
	}

	// 0101 1010 1101 0100
	// 1101 0100 0101 1010
	// 0101 1010 1101 0100
	// 1101 0100 0101 1010
	// 1101 0100 0101 1010
	// 1101 0100 0101 1010

	// shift up (untested)
	else if(dir == SHIFT_UP)
	{
		for(int i=ticks; i < block->height - 1; i++)
		{
			Byte byte = getByte(block, col, i);
			setByte(byte, block, col, i-ticks);
		}
		// wrap bytes
		for(int i=block->height - ticks, j=0; i < block->height - 1; i++, j++)
		{
			Byte byte = tempCol[j];
			setByte(byte, block, col, i);
		}
	}

	return ticks;
}

Byte getByte(struct DataBlock *block, int col, int row)
{
	return(block->data[col + row*block->width]);
}

Byte setByte(Byte value, struct DataBlock *block, int col, int row)
{
	block->data[col + row*block->width] = value;
	return value;
}

void printBlocks(struct DataBlock *first)
{
	char *string = malloc(first->width + 1);
	int numBlocks = 0;

	string[first->width] = '\0';

	while(1)
	{
		printf("Block %d", numBlocks);

		for(int i = 0; i < first->height; i++)
		{
			for(int ii = 0; ii < first->width; ii++)
			{
				string[i] = (char) getByte(first, ii, i);
			}

			printf("%s\n", string + 1);
		}

		if(first->next == NULL)
		{
			return;
		}
		else
		{
			first = first->next;
		}

		numBlocks++;
	}
}