#ifndef DATA_BLOCKS_H
#define DATA_BLOCKS_H

#include "main.h"

struct dataBlock
{
	int width, height;
	Byte *data;
};

Byte getByte(struct dataBlock *block, int col, int row);
Byte setByte(Byte value, struct dataBlock *block, int col, int row);

/**
 * Shifts a column of bytes in a data block up or down a given number of bytes (wraps around)
 * @param block pointer to a data block struct
 * @param col column to shift (0 = far left)
 * @param ticks how many bytes to shift the column by
 * @param dir whether to shift the column up or down (0 = down, 1 = up)
**/
int shiftCol(struct dataBlock *block, int col, int ticks, int dir);

#endif