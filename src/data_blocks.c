#include "../include/data_blocks.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/data.h"

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

int shiftCol(struct DataBlock *block, unsigned int col, unsigned int ticks, enum DIRECTION dir)
{
	Byte *tempCol;

	if(col < 0 || col >= block->width || (dir != SHIFT_UP && dir != SHIFT_DOWN))
	{
		return 0;
	}

	ticks %= block->height;

	tempCol = malloc(block->height - ticks);

	if(dir == SHIFT_DOWN)
	{
		// save the wrapped bytes

		for(unsigned int i = block->height - ticks, j = 0; i < block->height; i++, j++)
		{
			tempCol[j] = getByte(block, col, i);
		}

		// copy body

		for(unsigned int i = block->height - 1; i > ticks - 1; i--)
		{
			Byte byte = getByte(block, col, i - ticks);
			setByte(byte, block, col, i);
		}

		// copy wrapped bytes

		for(unsigned int i = 0; i < ticks; i++)
		{
			Byte byte = tempCol[i];
			setByte(byte, block, col, i);
		}
	}

	else if(dir == SHIFT_UP)
	{
		// save the wrapped bytes

		for(unsigned int i = 0; i < ticks; i++)
		{
			tempCol[i] = getByte(block, col, i);
		}

		// copy body

		for(unsigned int i = 0; i < block->height - ticks - 1; i++)
		{
			Byte byte = getByte(block, col, i + ticks);
			setByte(byte, block, col, i);
		}

		// copy wrapped bytes

		for(unsigned int i = block->height - ticks, j = 0; i < block->height; i++, j++)
		{
			Byte byte = tempCol[j];
			setByte(byte, block, col, i);
		}
	}

	return 1;
}

int shiftRow(struct DataBlock *block, unsigned int row, unsigned int ticks, enum DIRECTION dir)
{
	Byte *tempRow;
	Byte byte;
	unsigned int i, j;

	if(row < 0 || row >= block->width || (dir != SHIFT_LEFT && dir != SHIFT_RIGHT))
	{
		return 0;
	}
	
	ticks %= block->width;
	
	tempRow = malloc(block->width - ticks);
	
	/**
	 * 10101 10101    10101 10101    10101 10101    10101 10101    10101 10101    10101 10101
	 * 10101 10101    10101 10101    10101 10101    10101 10101    10101 10101    10101 10101
	 * 10101 10101    10101 10101    10101 10101    10101 10101    10101 10101    10101 10101
	 * 10101 10101    10101 10101    10101 10101    10101 10101    10101 10101    10101 10101
	 * 10101 10101    10101 10101    10101 10101    10101 10101    10101 10101    10101 10101
	 * 10101 10101    10101 10101    10101 10101    10101 10101    10101 10101    10101 10101
	 * 10101 10101    10101 10101    10101 10101    10101 10101    10101 10101    10101 10101
	**/
	if(dir == SHIFT_LEFT)
	{
		// save the wrapped bytes
		
		for(i = 0; i < ticks; i++)
		{
			tempRow[i] = getByte(block, i, row);
		}
		
		// copy body
		
		for(i = ticks; i < block->width - 1; i++)
		{
			byte = getByte(block, i, row);
			setByte(byte, block, i - ticks, row);
		}
		
		// copy wrapped bytes
		
		for(i = block->width - ticks, j = 0; i < block->width - 1; i++, j++)
		{
			byte = tempRow[j];
			setByte(byte, block, i, row);
		}
	}
	
	else if(dir == SHIFT_RIGHT)
	{
		// save the wrapped bytes
		
		for(i = block->width - ticks, j = 0; i < block->width - 1; i++, j++)
		{
			tempRow[j] = getByte(block, i, row);
		}
		
		// copy body
		
		for(i = block->width - ticks - 1; i >= 0; i--)
		{
			byte = getByte(block, i, row);
			setByte(byte, block, i + ticks, row);
		}
		
		// copy wrapped bytes
		
		for(i = 0; i < ticks; i++)
		{
			byte = tempRow[i];
			setByte(byte, block, i, row);
		}
	}
	
	return 1;
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
	
		for(unsigned int i = 0; i < first->height; i++)
		{
			for(unsigned int ii = 0; ii < first->width; ii++)
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

void scrambleBlockData(struct DataBlock *first, char *key)
{
	printf("\nScrambling data\n");
	const int keyLen = strlen(key);
	unsigned int numBlocks = 1;
	const unsigned int width = first->width;
	// const unsigned int height = first->height;
	unsigned int col;
	unsigned int ticks, direction;
	unsigned int success;

	static const unsigned char constants1[100] = {41, 73, 123, 164, 231, 4, 56, 79, 123, 182, 200, 252, 28, 43, 73, 116, 156, 169, 206, 230, 242, 20, 43, 75, 116, 136, 146, 166, 176, 195, 2, 20, 46, 54, 96, 104, 128, 152, 168, 191, 213, 221, 2, 9, 23, 30, 72, 113, 126, 133, 146, 165, 172, 204, 223, 241, 4, 10, 28, 40, 46, 75, 116, 127, 133, 144, 183, 199, 226, 232, 242, 2, 23, 39, 54, 65, 80, 100, 110, 130, 154, 159, 184, 188, 203, 212, 226, 245, 255, 3, 13, 40, 58, 67, 85, 94, 108, 134, 138, 177};
	static const unsigned char constants2[100] = {64, 85, 111, 128, 152, 161, 176, 183, 194, 209, 213, 225, 232, 235, 241, 250, 2, 4, 12, 16, 18, 24, 28, 34, 41, 44, 46, 49, 51, 54, 64, 67, 71, 72, 78, 79, 83, 87, 89, 92, 95, 96, 102, 103, 105, 106, 111, 117, 119, 119, 121, 124, 125, 129, 131, 134, 136, 137, 139, 141, 141, 145, 150, 151, 152, 153, 158, 160, 163, 164, 165, 167, 170, 171, 173, 174, 176, 179, 180, 182, 185, 185, 188, 189, 190, 191, 193, 195, 196, 196, 197, 201, 202, 203, 205, 206, 208, 211, 211, 215};
	
	while(1)
	{
		for(col = 0; col < width; col++)
		{
			ticks = key[col] ^ (constants1[col] & constants2[col]);
			direction = (key[col] + numBlocks + keyLen) % 2;
			
			printf("moving %u ticks\n", ticks);
			success = shiftCol(first, col, ticks, direction);
			
			if(!success)
			{
				printf("scramble failure\n");
				return;
			}
		}
		
		numBlocks++;

		if(first->next == NULL)
		{
			return;
		}
		first = first->next;
	}
	
	return;
}

void unscrambleBlockData(struct DataBlock *first, char *key)
{
	printf("\nUnscrambling data\n");
	const int keyLen = strlen(key);
	unsigned int numBlocks = 1;
	const unsigned int width = first->width;
	// const unsigned int height = first->height;
	unsigned int col;
	unsigned int ticks, direction;
	unsigned int success;

	const unsigned char constants1[100] = {41, 73, 123, 164, 231, 4, 56, 79, 123, 182, 200, 252, 28, 43, 73, 116, 156, 169, 206, 230, 242, 20, 43, 75, 116, 136, 146, 166, 176, 195, 2, 20, 46, 54, 96, 104, 128, 152, 168, 191, 213, 221, 2, 9, 23, 30, 72, 113, 126, 133, 146, 165, 172, 204, 223, 241, 4, 10, 28, 40, 46, 75, 116, 127, 133, 144, 183, 199, 226, 232, 242, 2, 23, 39, 54, 65, 80, 100, 110, 130, 154, 159, 184, 188, 203, 212, 226, 245, 255, 3, 13, 40, 58, 67, 85, 94, 108, 134, 138, 177};
	const unsigned char constants2[100] = {64, 85, 111, 128, 152, 161, 176, 183, 194, 209, 213, 225, 232, 235, 241, 250, 2, 4, 12, 16, 18, 24, 28, 34, 41, 44, 46, 49, 51, 54, 64, 67, 71, 72, 78, 79, 83, 87, 89, 92, 95, 96, 102, 103, 105, 106, 111, 117, 119, 119, 121, 124, 125, 129, 131, 134, 136, 137, 139, 141, 141, 145, 150, 151, 152, 153, 158, 160, 163, 164, 165, 167, 170, 171, 173, 174, 176, 179, 180, 182, 185, 185, 188, 189, 190, 191, 193, 195, 196, 196, 197, 201, 202, 203, 205, 206, 208, 211, 211, 215};

	while(1)
	{
		for(col = width - 1; col >= 0; col--)
		{
			ticks = key[col] ^ (constants1[col] & constants2[col]);
			direction = (key[col] + numBlocks + keyLen + 1) % 2;

			printf("moving %u ticks\n", ticks);
			success = shiftCol(first, col, ticks, direction);

			if(!success)
			{
				printf("scramble failure\n");
				return;
			}
		}

		numBlocks++;
		printf("numBlocks %d", numBlocks);

		if(first->next == NULL)
		{
			return;
		}
		printf("%p", (void*)first->next);
		first = first->next;
	}

	return;
}

/**
void randomizeBytes(struct DataBlock *first, char *key)
{
	int keyCursor = 0;
	int keyLen = strlen(key);
	int numBlocks = 1;

	const char constants1[100] = {41, 73, 123, 164, 231, 4, 56, 79, 123, 182, 200, 252, 28, 43, 73, 116, 156, 169, 206, 230, 242, 20, 43, 75, 116, 136, 146, 166, 176, 195, 2, 20, 46, 54, 96, 104, 128, 152, 168, 191, 213, 221, 2, 9, 23, 30, 72, 113, 126, 133, 146, 165, 172, 204, 223, 241, 4, 10, 28, 40, 46, 75, 116, 127, 133, 144, 183, 199, 226, 232, 242, 2, 23, 39, 54, 65, 80, 100, 110, 130, 154, 159, 184, 188, 203, 212, 226, 245, 255, 3, 13, 40, 58, 67, 85, 94, 108, 134, 138, 177};
	const char constants2[100] = {64, 85, 111, 128, 152, 161, 176, 183, 194, 209, 213, 225, 232, 235, 241, 250, 2, 4, 12, 16, 18, 24, 28, 34, 41, 44, 46, 49, 51, 54, 64, 67, 71, 72, 78, 79, 83, 87, 89, 92, 95, 96, 102, 103, 105, 106, 111, 117, 119, 119, 121, 124, 125, 129, 131, 134, 136, 137, 139, 141, 141, 145, 150, 151, 152, 153, 158, 160, 163, 164, 165, 167, 170, 171, 173, 174, 176, 179, 180, 182, 185, 185, 188, 189, 190, 191, 193, 195, 196, 196, 197, 201, 202, 203, 205, 206, 208, 211, 211, 215};


}
**/