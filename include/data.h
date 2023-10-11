#ifndef DATA_H
#define DATA_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef unsigned char Byte;

struct Data
{
	Byte *ptr;
	uint32_t size;
};

int copyBytes(Byte *dest, Byte *src, int len);
void wipeBytes(void *ptr, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif