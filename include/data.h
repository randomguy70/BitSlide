#ifndef DATA_H
#define DATA_H

#ifdef __cplusplus
extern "C"
{
#endif

/* The size limit for any file you want to encrypt */
/* To encrypt larger files, simply change this number to whatever you want */
#define MAX_DATA_SIZE 10000000  // 10 Megabytes

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