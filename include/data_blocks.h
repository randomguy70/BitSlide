#ifndef DATA_BLOCKS_H
#define DATA_BLOCKS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

struct DataBlock
{
	int width, height;
	Byte *data;
	struct DataBlock *next;
};

enum DIRECTION {SHIFT_UP, SHIFT_DOWN, SHIFT_LEFT, SHIFT_RIGHT};

/** 
 * Copies any given data into a linked list of data blocks and returns a pointer to the first block
 * WARNING: This function frees the data from memory, so make sure to store the returned linked list!
**/
struct DataBlock *dataToBlocks(struct Data *data);

/** 
 * Copies the data from a given linked list of data blocks into a Data struct, and returns a pointer to the struct
 * WARNING: This function frees the linked list from memory, so make sure to store the returned data struct!
**/
struct Data *blocksToData(struct DataBlock *first);

/** 
 * Frees a given linked list of data blocks from memory
 * @param first a pointer to the first element in the linked list
**/
int freeBlocks(struct DataBlock *first);

int copyBytes(Byte *dest, Byte *src, int len);
Byte getByte(struct DataBlock *block, int col, int row);
Byte setByte(Byte value, struct DataBlock *block, int col, int row);

/**
 * Shifts a column of bytes in a data block up or down a given number of bytes (wraps around)
 * @param block pointer to a data block struct
 * @param col column to shift (0 = far left)
 * @param ticks how many bytes to shift the column by
 * @param dir whether to shift the column up or down (0 = down, 1 = up)
**/
int shiftCol(struct DataBlock *block, int col, int ticks, enum DIRECTION dir);

/**
 * Prints the data stored in a linked list of dataBlocks with respect to rows and columns
 * @param first a pointer to the first element in the linked list
**/
void printBlocks(struct DataBlock *first);

#ifdef __cplusplus
}
#endif

#endif