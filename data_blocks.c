#include <stdio.h>
#include <stdlib.h>

#include "data_blocks.h"
#include "main.h"

int shiftCol(struct DataBlock *block, int col, int ticks, enum ShiftDir dir)
{
	Byte *tempCol = malloc(block->height);
	
	// 0101 1010 1101 0100
	// 1101 0100 0101 1010
	// 0101 1010 1101 0100
	// 1101 0100 0101 1010
	
	// copy the column to preserve the wrapped bytes
	for(int i=0; i<block->height; i++)
	{
		tempCol[i] = getByte(block, col, i);
	}
	
	// shift down XXX (I haven't finished it)
	if(dir == 1)
	{
		for(int i=block->height - ticks; i > -1; i-=ticks)
		{
			Byte byte = getByte(block, col, i);
			setByte(byte, block, col, i+1);
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