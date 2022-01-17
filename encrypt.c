#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "encrypt.h"
#include "data_blocks.h"

/**
 * Steps:
 * looping through every data byte... 
 * 1	1. += one byte from the key (increments & wraps back to beginning)
 * 1	2. XOR with 0xe7
 * 1	3. shift bits left 2 notches, wrap the 2 bits pushed off to the right side of the byte
 * 0	4. separate bytes into blocks, shift around the rows and columns
*/
// XXX
struct Data encryptData(struct Data *data, char *key)
{
	struct Data ret;
	int keyLen = strlen(key);
	int keyCursor = 0;
	Byte temp;
	struct DataBlock *block;
	
	
	
	for(int i=0; i<data->size; i++)
	{
		data->ptr[i] += key[keyCursor++];
		if(keyCursor > keyLen - 1) {keyCursor = 0;}
		
		data->ptr[i] ^= 0xe7;
		
		temp = data->ptr[i];
		data->ptr[i] <<= 2;
		data->ptr[i] += (temp >> 6);
	}
	
	return ret;
}

/** does the opposite of the encryption (obv)
 * looping through every data byte...
 * 	1. shift bits right 2 notches, wrap the two bits pushed off to the left side of the byte
 * 	2. XOR with 0xe7
 * 	3. -= one byte from the key (increments & wraps around back to beginning)
**/

struct Data decryptData(struct Data *data, char *key)
{
	struct Data ret = {.ptr = NULL, .size = 0};
	int keyLen = strlen(key);
	int keyCursor = 0;
	Byte temp = '\0';
	
	for(int i=0; i<data->size; i++)
	{
		// 0000 0011
		temp = data->ptr[i];
		data->ptr[i] >>= 2;
		data->ptr[i] += (temp << 6);
		
		data->ptr[i] ^= 0xe7;
		
		data->ptr[i] -= key[keyCursor++];
		if(keyCursor > keyLen - 1) {keyCursor = 0;}
	}
	
	return ret;
}