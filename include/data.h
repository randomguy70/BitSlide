#ifndef DATA_H
#define DATA_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char Byte;

struct Data
{
	Byte *ptr;
	int size;
};

#ifdef __cplusplus
}
#endif

#endif