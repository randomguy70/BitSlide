#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char Byte;

int encryptData(Byte *data, int size, char *key);
int unencryptData(Byte *data, int size, char *key);

int main(int argc, char *argv[])
{
	FILE *file;
	Byte *data;
	int size;
	
	file = fopen(argv[1], "r");
	if(!file)
	{
		printf("File not found");
		return 0;
	}
	if(argc != 3)
	{
		printf("Error. Enter the path to the file you wish to encrypt and the key to encrypt it with as arguments.");
		return 0;
	}
	
	// get the size
	fseek(file, 0L, SEEK_END);
	size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	
	data = malloc(size);
	
	fread(data, size, 1, file);
	fclose(file);
	
	printf("Encrypting file: %d bytes long", size);
	
	if(unencryptData(data, size, argv[1]) != 1)
	{
	 	printf("Horrible disaster. The encryption process failed.");
	 	return 0;
	}
	
	file = fopen(argv[1], "w");
	fseek(file, 0L, SEEK_SET);
	fwrite(data, size, 1, file);
	fclose(file);
	
	return 0;
}

/**
 * Steps:
 * looping through every data byte... 
 * 	1. += one byte from the key (increments & wraps back to beginning)
 * 	2. XOR with 0xe7
 * 	3. shift bits left 2 notches, wrap the 2 bits pushed off to the right side of the byte
*/

int encryptData(Byte *data, int size, char *key)
{
	int keyLen = strlen(key);
	int keyCursor = 0;
	Byte temp = '\0';
	
	for(int i=0; i<size; i++)
	{
		data[i] += key[keyCursor++];
		if(keyCursor > keyLen - 1) {keyCursor = 0;}
		
		data[i] ^= 0xe7;
		
		// 1100 0000
		temp = data[i];
		data[i] <<= 2;
		data[i] += (temp >> 6);
	}
	
	return 1;
}

/** does the opposite of the encryption (obv)
 * looping through every data byte...
 * 	1. shift bits right 2 notches, wrap the two bits pushed off to the left side of the byte
 * 	2. XOR with 0xe7
 * 	3. -= one byte from the key (increments & wraps around back to beginning)
**/

int unencryptData(Byte *data, int size, char *key)
{
	int keyLen = strlen(key);
	int keyCursor = 0;
	Byte temp = '\0';
	
	for(int i=0; i<size; i++)
	{
		// 0000 0011
		temp = data[i];
		data[i] >>= 2;
		data[i] += (temp << 6);
		
		data[i] ^= 0xe7;
		
		data[i] -= key[keyCursor++];
		if(keyCursor > keyLen - 1) {keyCursor = 0;}
	}
	
	return 1;
}