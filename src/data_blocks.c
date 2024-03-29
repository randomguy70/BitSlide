#include "../include/data_blocks.h"

#include "../include/data.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

struct DataBlock *dataToBlocks(struct Data *data, bool dataIsEncrypted)
{
	Byte *array;
	uint32_t arraySize;
	struct DataBlock *block = NULL, *block1 = NULL;
	int numBlocks = 0;
	
	// if the data is encrypted, then just write it into blocks
	// * should to change block list into linked list only for size benefits, not an array
	
	if(dataIsEncrypted == true)
	{
		// the data length should be an exact multiple of the block data size
		if(data->size > 0 && data->size % BLOCK_DATA_SIZE != 0)
		{
			printf("Data has been tampered with\n");
			return NULL;
		}
		
		numBlocks = data->size / BLOCK_DATA_SIZE;		
		block = malloc(sizeof(struct DataBlock));
		block1 = block;
		for(int i = 0; i < numBlocks; i++)
		{
			block->data   = malloc(BLOCK_DATA_SIZE);
			
			copyBytes(block->data, data->ptr + (i*BLOCK_DATA_SIZE), BLOCK_DATA_SIZE);
			
			if(i < numBlocks - 1)
			{
				block->next = malloc(sizeof(struct DataBlock));
				block = block->next;
				continue;
			}
			else
			{
				block->next = NULL;
				break;
			}
		}
		
		return block1;
	}
	
	// if the data isn't encrypted, store its size at end of last block
	
	numBlocks = (data->size + (int)sizeof(uint32_t)) / BLOCK_DATA_SIZE;
	if((data->size + (int)sizeof(uint32_t)) % BLOCK_DATA_SIZE)
	{
		numBlocks++;
	}
	printf("data: %d bytes, number of unencrypted blocks: %d\n", data->size, numBlocks);
	
	block = malloc(sizeof(struct DataBlock));
	block1 = block;
	
	// initialise blocks
	printf("initialising blocks\n");
	
	for(int i = 0; i < numBlocks; i++)
	{
		if(i == numBlocks - 1)
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
	
	arraySize = BLOCK_DATA_SIZE * numBlocks;
	array = calloc(1, arraySize);
	
	copyBytes(array, data->ptr, data->size);
	
	// store the size at the end of the data
	
	*((uint32_t*) (array + arraySize - 1 - sizeof(uint32_t))) = data->size;
	
	// make the blocks' pointers point straight to the data in the array
	
	block = block1;
	
	for(int i = 0; i < numBlocks; i++)
	{
		block->data = array + (i * BLOCK_DATA_SIZE);
		block = block->next;
	}
	
	return block1;
}

struct Data *blocksToData(struct DataBlock *first, bool dataIsEncrypted)
{
	struct DataBlock *block = first;
	struct Data *data = malloc(sizeof(struct Data));
	uint32_t numBlocks = 1;
	uint32_t *sizePtr;
	
	// if encrypted...
	
	if(dataIsEncrypted)
	{
		numBlocks = getNumBlocks(first);
		data->size = numBlocks * BLOCK_DATA_SIZE;
		data->ptr = malloc(data->size);
		struct DataBlock *temp;
		
		for(uint32_t i = 0, dataOffset = 0; i < numBlocks; i++, dataOffset += BLOCK_DATA_SIZE)
		{
			copyBytes(data->ptr + dataOffset, first->data, BLOCK_DATA_SIZE);
			temp = first;
			first = first->next;
			free(temp);
		}
		
		return data;
	}
	
	// if not encrypted...
	// get data size (stored in uint32_t at end of last block)
	while(block->next != NULL)
	{
		block = block->next;
		numBlocks++;
	}
	
	sizePtr = (uint32_t*) (block->data + BLOCK_DATA_SIZE - 1 - sizeof(uint32_t));
	block = first; // make sure the block points to the first one
	data->size = *sizePtr;
	data->ptr = malloc(data->size);
		
	// copy all the data from every block except the last
	
	printf("copying unencrypted blocks into data\n");
	
	for(uint32_t i = 0, bytesCopied = 0; i < numBlocks - 1; i++, bytesCopied += BLOCK_DATA_SIZE)
	{
		copyBytes(data->ptr + bytesCopied, block->data, BLOCK_DATA_SIZE);
		block = block->next;
	}
	
	// copy the data from the last block (minus the last 4 bytes)
	
	printf("copying last block\n");
	
	copyBytes(data->ptr + (BLOCK_DATA_SIZE * (numBlocks - 1)), block->data, data->size - (BLOCK_DATA_SIZE * (numBlocks - 1)));
	freeBlocks(first);
	
	return data;
}

int freeBlocks(struct DataBlock *first)
{
	int i = 1;
	struct DataBlock *temp;

	// since the data in all the blocks points to the same large block in memory, freeing the 1st pointer frees it all
	
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

uint32_t shiftCol(Byte *blockPtr, uint32_t col, uint32_t ticks, enum Direction dir)
{
	Byte *tempCol;
	
	col &= (BLOCK_WIDTH - 1);
	ticks &= (BLOCK_HEIGHT - 1);
	dir &= 0x1;
	
	if(ticks == 0) return 1;
	
	tempCol = malloc(BLOCK_HEIGHT - ticks);
	
	if(dir == SHIFT_DOWN)
	{
		// save the wrapped bytes

		for(uint32_t i = BLOCK_HEIGHT - ticks, j = 0; i < BLOCK_HEIGHT; i++, j++)
		{
			tempCol[j] = getByte(blockPtr, col, i);
		}

		// copy body

		for(uint32_t i = BLOCK_HEIGHT - 1; i > ticks - 1; i--)
		{
			Byte byte = getByte(blockPtr, col, i - ticks);
			setByte(byte, blockPtr, col, i);
		}

		// copy wrapped bytes

		for(uint32_t i = 0; i < ticks; i++)
		{
			Byte byte = tempCol[i];
			setByte(byte, blockPtr, col, i);
		}
	}
	
	else if(dir == SHIFT_UP)
	{
		// save the wrapped bytes

		for(uint32_t i = 0; i < ticks; i++)
		{
			tempCol[i] = getByte(blockPtr, col, i);
		}

		// copy body

		for(uint32_t i = 0; i < BLOCK_HEIGHT - ticks; i++)
		{
			Byte byte = getByte(blockPtr, col, i + ticks);
			setByte(byte, blockPtr, col, i);
		}

		// copy wrapped bytes

		for(uint32_t i = BLOCK_HEIGHT - ticks, j = 0; i < BLOCK_HEIGHT; i++, j++)
		{
			Byte byte = tempCol[j];
			setByte(byte, blockPtr, col, i);
		}
	}
	
	return 1;
}

int shiftRow(Byte *blockPtr, uint32_t row, uint32_t ticks, enum Direction dir)
{	
	Byte *tempRow;
	Byte byte;
	uint32_t i, j;
	
	row &= (BLOCK_HEIGHT - 1);
	ticks &= (BLOCK_WIDTH - 1);
	dir &= 0x1;
	
	if(ticks == 0) return 1;
	
	tempRow = malloc(ticks);
	
	if(dir == SHIFT_LEFT)
	{
		// save the wrapped bytes
		
		for(i = 0; i < ticks; i++)
		{
			tempRow[i] = getByte(blockPtr, i, row);
		}
		
		// copy body
		
		for(i = 0; i < BLOCK_WIDTH - ticks; i++)
		{
			byte = getByte(blockPtr, i + ticks, row);
			setByte(byte, blockPtr, i, row);
		}
		
		// copy wrapped bytes
		
		for(i = BLOCK_WIDTH - ticks, j = 0; i < BLOCK_WIDTH; i++, j++)
		{
			byte = tempRow[j];
			setByte(byte, blockPtr, i, row);
		}
	}
	
	if(dir == SHIFT_RIGHT)
	{
		// save the wrapped bytes
		
		for(i = BLOCK_WIDTH - ticks, j = 0; i < BLOCK_WIDTH; i++, j++)
		{
			tempRow[j] = getByte(blockPtr, i, row);
		}
		
		// copy body
		
		for(i = BLOCK_WIDTH - 1; i >= ticks; i--)
		{
			byte = getByte(blockPtr, i - ticks, row);
			setByte(byte, blockPtr, i, row);
		}
		
		// copy wrapped bytes
		
		for(i = 0; i < ticks; i++)
		{
			byte = tempRow[i];
			setByte(byte, blockPtr, i, row);
		}
	}
	
	return 1;
}

Byte getByte(Byte *blockPtr, int col, int row)
{
	return(blockPtr[col + (row*BLOCK_WIDTH)]);
}

void setByte(Byte value, Byte *blockPtr, int col, int row)
{
	blockPtr[col + (row*BLOCK_WIDTH)] = value;
}

void printBlocks(struct DataBlock *first)
{
	printf("printing all block data in ASCII format:\n");
	while(1) {
		for(int i = 0; i < BLOCK_DATA_SIZE; i++)
		{
			printf("%c", first->data[i]);
		}
		if(first->next != NULL)
		{
			first = first->next;
		}
		else
		{
			return;
		}
	}
}

uint32_t getNumBlocks(struct DataBlock *first)
{
	uint32_t num = 1;
	while(first->next != NULL)
	{
		first = first->next;
		num++;
	}
	return num;
}
