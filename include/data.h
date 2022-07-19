#ifndef DATA_H
#define DATA_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* The size limit for any file you want to encrypt */
/* To encrypt larger files, simply change this number to whatever you want */
#define MAX_DATA_SIZE 50000000  // 50 Megabytes

typedef unsigned char Byte;

struct Data
{
	Byte *ptr;
	uint32_t size;
};

#ifdef __cplusplus
}
#endif

#endif