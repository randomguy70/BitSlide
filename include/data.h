#ifndef DATA_H
#define DATA_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_DATA_SIZE 10000000

typedef unsigned char Byte;

struct Data
{
	Byte *ptr;
	unsigned int size;
};

#ifdef __cplusplus
}
#endif

#endif