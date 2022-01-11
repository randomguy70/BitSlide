#include <stdio.h>
#include <stdlib.h>

#include "data_blocks.h"
#include "main.h"

int shiftCol(struct dataBlock *block, int col, int ticks, int dir)
{
	Byte *temp = malloc(block->height);
	
	// 0101 1010 1101 0100
	// 1101 0100 0101 1010
	// 0101 1010 1101 0100
	// 1101 0100 0101 1010
	
	// copy the column to preserve the wrapped bytes
	for(int i=0; i<block->height; i++)
	{
		temp[i] = getByte(block, col, i);
	}
	
	
	
	return ticks;
}

Byte getByte(struct dataBlock *block, int col, int row)
{
	return(block->data[col + row*block->width]);
}

Byte setByte(Byte value, struct dataBlock *block, int col, int row)
{
	block->data[col + row*block->width] = value;
}