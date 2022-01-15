#ifndef DATA_BLOCKS_H
#define DATA_BLOCKS_H

#include "main.h"

struct DataBlock
{
	int width, height;
	Byte *data;
};

Byte getByte(struct DataBlock *block, int col, int row);
Byte setByte(Byte value, struct DataBlock *block, int col, int row);

enum ShiftDir {SHIFT_UP, SHIFT_DOWN, SHIFT_LEFT, SHIFT_RIGHT};

/**
 * Shifts a column of bytes in a data block up or down a given number of bytes (wraps around)
 * @param block pointer to a data block struct
 * @param col column to shift (0 = far left)
 * @param ticks how many bytes to shift the column by
 * @param dir whether to shift the column up or down (0 = down, 1 = up)
**/
int shiftCol(struct DataBlock *block, int col, int ticks, enum ShiftDir dir);

#endif