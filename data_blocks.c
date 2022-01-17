#include <stdio.h>
#include <stdlib.h>

#include "data_blocks.h"
#include "main.h"

struct DataBlock *dataToBlocks(struct Data *data)
{
	struct DataBlock *block1, *block;
	const int width = 15, height = 10;             // should be derived from key or hash
	int i = 0, j = 0;
	Byte *ptr = data->ptr;
	
	block1 = malloc(sizeof(struct DataBlock));
	block = block1;
	
	while(1)
	{
		block->data = ptr;
		block->width = width;
		block->height = height;
		
		// if the block has extra space left
		if(ptr + block->width * block->height - 1 > data->ptr + data->size - 1)
		{
			j = 0;
			while(ptr <= data->ptr + data->size - 1)
			{
				block->data[j++] = *ptr++;
			}
			while(j <= block->width * block->height - 1)
			{
				block->data[j++] = 0;
			}
			
			// store data size at the end of the last block
			int *tempPtr = (int*)block->data + block->width * block->height - 1 - sizeof(int);
			*tempPtr = data->size;
			
			// show that it's the last block
			block->next = NULL;
			
			return block1;
		}
		
		// copy data into block
		for(i=0; i <= block->width * block->height - 1; i++)
		{
			block->data[i] = *ptr++;
		}
		
		block->next = malloc(sizeof(struct DataBlock));
		block = block->next;
	}
	
	return block1;
}

struct Data *blocksToData(struct DataBlock *first)
{
	struct DataBlock *block = first, *temp;
	Byte *data = NULL;
	int size = 0;
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
	size = *tempPtr;
	
	data = malloc(size);
	block = first;
	
	for(int i=1; i <= numBlocks; i++)
	{
		if(i < numBlocks)
		{
			copyBytes(data + bytesCopied, block->data, block->width * block->height);
		}
		else if(i == numBlocks)
		{
			copyBytes(data + bytesCopied, block->data, size - bytesCopied);
			
			return data;
		}
	}
	
	return data;
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

int shiftCol(struct DataBlock *block, int col, int ticks, enum ShiftDir dir)
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
}