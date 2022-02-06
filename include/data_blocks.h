#ifndef DATA_BLOCKS_H
#define DATA_BLOCKS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/data.h"

struct DataBlock
{
	unsigned int width, height;
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
 * Shifts a column of bytes in a data block up or down a given number of ticks (wraps around)
 * @param block pointer to a data block struct
 * @param col column to shift (0 indexed)
 * @param ticks how many bytes to shift the column by
 * @param dir whether to shift the column up or down (SHIFT_UP, SHIFT_DOWN)
**/
int shiftCol(struct DataBlock *block, unsigned int col, unsigned int ticks, enum DIRECTION dir);

/**
 * Shifts a row of bytes in a data block left or right a given number of ticks (wraps around)
 * @param block pointer to a data block struct
 * @param col row to shift (0 indexed)
 * @param ticks how many bytes to shift the row by
 * @param dir whether to shift the left or right (SHIFT_LEFT, SHIFT_RIGHT)
**/
int shiftRow(struct DataBlock *block, unsigned int row, unsigned int ticks, enum DIRECTION dir);

// Prints the data stored in a linked list of dataBlocks
void printBlocks(struct DataBlock *first);

// shifts the rows and columns in a given linked list of data blocks
void scrambleBlockData(struct DataBlock *first, char *key);
void unscrambleBlockData(struct DataBlock *first, char *key);

// changes the value of each byte in a given linked list of data blocks
void randomizeBytes(struct DataBlock *first, char *pw);

#ifdef __cplusplus
}
#endif

#endif