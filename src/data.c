#include "../include/data.h"

int copyBytes(Byte *dest, Byte *src, int len)
{
	int i;

	for(i=0; i<len; i++)
	{
		dest[i] = src[i];
	}

	return i;
}

void wipeBytes(void *ptr, uint32_t len)
{
	for(uint32_t i = 0; i < len; i++)
	{
		((uint8_t*) ptr)[i] = 0;
	}
}