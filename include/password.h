#ifndef PASSWORD_H
#define PASSWORD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/sha256.h"

#include <stdio.h>

#define ITERATIONS_PER_SHA_ARRAY                  8
#define SHA_ARRAY_SIZE_BYTES                      (SHA256_SIZE_BYTES * ITERATIONS_PER_SHA_ARRAY)
#define SHA_ARRAY_WORD_LEN                        (SHA_ARRAY_SIZE_BYTES / sizeof(uint32_t))

struct password
{	
	char*    string;                           // original password as ASCII string
	uint8_t  salt        [SHA256_SIZE_BYTES ]; // randomly generated salt
	uint8_t  hash        [SHA256_SIZE_BYTES ]; // hash of salt + password                   (iteration 1)
	uint32_t shaArray    [SHA_ARRAY_WORD_LEN]; // holds 8 hash iterations                   (iteration 2-9)
	uint8_t  checkHash   [SHA256_SIZE_BYTES ]; // 10th hash iteration of salt + password    (iteration 10)
};

void saltPassword(struct password *password);
void hashPassword(struct password *password);
void printPasswordData(struct password *password);
#ifdef __cplusplus
}
#endif

#endif