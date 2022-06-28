#include "../include/file.h"

#include <stdio.h>
#include <stdlib.h>

#include "../include/data.h"

int getFileSize(FILE *file)
{
	int size;
	long tell;
	
	tell = ftell(file);
	fseek(file, 0L, SEEK_END);
	size = ftell(file);
	fseek(file, tell, SEEK_SET);
	
	return size;
}

struct Data *getFileData(FILE *file)
{
	struct Data *ret;
	long tell;
	
	tell = ftell(file);
	
	ret = malloc(sizeof(struct Data));
	ret->size = getFileSize(file);
	ret->ptr = malloc(ret->size);
	
	fseek(file, 0L, SEEK_SET);
	fread(ret->ptr, ret->size, 1, file);
	fseek(file, tell, SEEK_SET);
	
	return ret;
}