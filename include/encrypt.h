#ifndef ENCRYPT_H
#define ENCRYPT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

enum Options {ENCRYPT = 1, DECRYPT};

/** XXX
 * Steps:
 * looping through every data byte... 
 * 1	1. += one byte from the key (increments & wraps back to beginning)
 * 1	2. XOR with 0xe7
 * 1	3. shift bits left 2 notches, wrap the 2 bits pushed off to the right side of the byte
 * 0	4. separate bytes into blocks, shift around the rows and columns
*/
struct Data *encryptData(struct Data *data, char *key);

/** does the opposite of the encryption (obv)
 * looping through every data byte...
 * 	1. shift bits right 2 notches, wrap the two bits pushed off to the left side of the byte
 * 	2. XOR with 0xe7
 * 	3. -= one byte from the key (increments & wraps around back to beginning)
**/
struct Data *decryptData(struct Data *data, char *key);

#ifdef __cplusplus
}
#endif

#endif