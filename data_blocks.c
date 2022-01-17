#include <stdio.h>
#include <stdlib.h>

#include "data_blocks.h"
#include "main.h"

// copies any given data into a linked list of data blocks and returns a pointer to the first block
struct DataBlock *dataToBlocks(struct Data *data)
{
	struct DataBlock *block1, *block;
	const int width = 15, height = 10;             // will be derived from key in future
	int bytesCopied = 0;
	
	block1 = malloc(sizeof(struct DataBlock));
	block = block1;
	
	while(bytesCopied < data->size)
	{
		block->width  = width;                      // should be derived from key
		block->height = height;                     // should be derived from key
		block->data   = malloc(block->width * block->height);
		
		// if the rest of the data will fit in this block
		if(bytesCopied + block->width * block->height >= data->size)
		{
			// if the 4 bytes to store the size don't fit at the end, then copy the rest of the data into the block,
			// pad 0's into the end of the block, create a new block, fill it with 0's, and store the size at the end of it
			if(bytesCopied + block->width * block->height - sizeof(int) < data->size)
			{
				Byte temp[sizeof(int)] = {'\0'};
				const int padNum = bytesCopied + block->width * block->height - data->size; // number of bytes to wipe at the end of the block
				
				copyBytes(block->data, data->ptr + bytesCopied, data->size - bytesCopied);
				copyBytes(block->data + (block->width * block->height - 1) - padNum, temp, padNum);
				
				block->next = malloc(sizeof(struct DataBlock));
				block = block->next;
				
				block->next = NULL;
				block->width = width;
				block->height = height;
				block->data = malloc(block->width * block->height);
				int i;
				for(i=0; i<block->width * block->height - 1 - sizeof(int); i++)
				{
					block->data[i] = '\0';
				}
				// store data size
				int *tempPtr = (int*)(block->data + i);
				*tempPtr = data->size;
				
				return block1;
			}
			
			else
			{
				const int padNum = bytesCopied + block->width * block->height - data->size; // number of empty bytes at the end of the block
				
				copyBytes(block->data, data->ptr + bytesCopied, data->size - bytesCopied);
				for(int i = block->width * block->height - 1 - padNum; i <= block->width * block->height - 1; i++)
				{
					block->data[i] = '\0';
				}
				
				// store the size
				int *tempPtr = (int*)(block->data + block->width * block->height - 1 - sizeof(int));
				*tempPtr = data->size;
				
				block->next = NULL;
				
				return block1;
			}
		}
		
		else
		{
			copyBytes(block->data, data->ptr + bytesCopied, block->width*block->height);
			bytesCopied += block->width*block->height;
			
			block->next = malloc(sizeof(struct DataBlock));
			block = block->next;
		}
	}
	
	return block1;
}

// copies the data from a given linked list of data blocks into a Data struct, and returns a pointer to the struct
struct Data *blocksToData(struct DataBlock *first)
{
	struct DataBlock *block = first, *temp;
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