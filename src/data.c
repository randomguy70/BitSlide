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