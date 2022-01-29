#include <stdio.h>
#include <stdlib.h>

#include "../include/data_blocks.h"
#include "../include/main.h"

struct DataBlock *dataToBlocks(struct Data *data)
{
	struct DataBlock *block1, *block;
	const int width = 12, height = 10; // width should be a multiple of sizeof(int), and height should be given as a parameter
	const int blockSize = width * height;
	const int numBlocks = (data->size + sizeof(int)) % blockSize ? (data->size + sizeof(int)) / blockSize : (data->size + sizeof(int)) / blockSize + 1;
	int bytesCopied = 0;
	
	block1 = malloc(sizeof(struct DataBlock));
	block = block1;
	
	// initialise block properties
	
	for(int i=1; i <= numBlocks; i++)
	{
		block->width  = width;
		block->height = height;
		block->data = malloc(blockSize);
		
		if(i ==  numBlocks)
		{
			block->next = NULL;
		}
		else
		{
			block->next = malloc(sizeof(struct DataBlock));
			block = block->next;
		}
	}
	
	// copy data into blocks
	
	block = block1;
	
	for(int i = 0; i < numBlocks - 1; i++)
	{
		if(block->next == NULL)
		{
			break;
		}
		copyBytes(block->data, data->ptr + bytesCopied, blockSize);
		bytesCopied += blockSize;
		block = block->next;
	}
	
	// copy data size into last 4 bytes of last block
	
	copyBytes(block->data, data->ptr+((numBlocks-1) * blockSize), blockSize - sizeof(int));
	*((int *)(data->ptr+(numBlocks * blockSize) - sizeof(int))) = data->size;
	
	return block1;
}

struct Data *blocksToData(struct DataBlock *first)
{
	struct DataBlock *block = first;
	struct Data *data = malloc(sizeof(struct Data));
	int bytesCopied = 0;
	int *tempPtr = NULL;
	int numBlocks = 1;
	
	//  get data size (stored at end of last block)
	while(block->next != NULL)
	{
		block = block->next;
		numBlocks++;
	}
	
	tempPtr = (int*)(block->data + block->height * block->width - 1 - sizeof(int));
	data->size = *tempPtr;
	
	data->ptr = malloc(data->size);
	block = first;
	
	for(int i=1; i <= numBlocks; i++)
	{
		if(i < numBlocks)
		{
			copyBytes(data->ptr + bytesCopied, block->data, block->width * block->height);
		}
		else if(i == numBlocks)
		{
			copyBytes(data->ptr + bytesCopied, block->data, data->size - bytesCopied);
			
			freeBlocks(first);
			return data;
		}
	}
	
	freeBlocks(first);
	return data;
}

int freeBlocks(struct DataBlock *first)
{
	int i = 1;
	struct DataBlock *temp;
	
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