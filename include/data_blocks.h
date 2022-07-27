#ifndef DATA_BLOCKS_H
#define DATA_BLOCKS_H

#include "../include/data.h"

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#define BLOCK_WIDTH            16
#define BLOCK_HEIGHT           16
#define BLOCK_DATA_SIZE        (BLOCK_WIDTH * BLOCK_HEIGHT)

struct DataBlock
{
	Byte *data;
	struct DataBlock *next;
};

enum Direction {SHIFT_UP = 0, SHIFT_DOWN = 1, SHIFT_LEFT = 0, SHIFT_RIGHT = 1};

/**
 * Copies any given data into a linked list of data blocks and returns a pointer to the first block
 * WARNING: This function frees the data from memory, so make sure to store the returned linked list!
**/
struct DataBlock *dataToBlocks(struct Data *data, bool dataIsEncrypted);

/**
 * Copies the data from a given linked list of data blocks into a Data struct, and returns a pointer to the struct
 * WARNING: This function frees the linked list from memory, so make sure to store the returned data struct!
**/
struct Data *blocksToData(struct DataBlock *first, bool dataIsEncrypted);

/**
 * Frees a given linked list of data blocks from memory
 * @param first a pointer to the first element in the linked list
**/
int freeBlocks(struct DataBlock *first);

/**
 * Returns the number of datablocks in a linked list, starting with 1
**/
uint32_t getNumBlocks(struct DataBlock *first);

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
int shiftCol(struct DataBlock *block, uint32_t col, uint32_t ticks, enum Direction dir);

/**
 * Shifts a row of bytes in a data block left or right a given number of ticks (wraps around)
 * @param block pointer to a data block struct
 * @param col row to shift (0 indexed)
 * @param ticks how many bytes to shift the row by
 * @param dir whether to shift the left or right (SHIFT_LEFT, SHIFT_RIGHT)
**/
int shiftRow(struct DataBlock *block, uint32_t row, uint32_t ticks, enum Direction dir);

// Prints the data stored in a linked list of DataBlocks
void printBlocks(struct DataBlock *first);

// changes the value of each byte in a given linked list of data blocks
void randomizeBytes(struct DataBlock *first, char *pw);

#ifdef __cplusplus
}
#endif

#endif