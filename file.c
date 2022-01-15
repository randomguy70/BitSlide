#include <stdio.h>
#include <stdlib.h>

#include "file.h"

int getFileSize(FILE *file)
{
	int size;
	
	fseek(file, 0L, SEEK_END);
	size = ftell(file);
	fseek(file, 0L, SEEK_SET);	
	fclose(file);
	
	return size;
}