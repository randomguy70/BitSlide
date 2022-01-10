#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char Byte;
enum options {ENCRYPT = 1, DENCRYPT};

int encryptData(Byte *data, int size, char *key);
int decryptData(Byte *data, int size, char *key);

int main(int argc, char *argv[])
{
	char *fileName = NULL;
	char *password = NULL;
	enum option = 0;
	
	FILE *file;
	Byte *data;
	int size;
	int i;
	
	if(strcmp(argv[1], "Help") == 0 || strcmp(argv[i], "h") == 0 || strcmp(argv[i], "H") == 0 || strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "-H") == 0 || strcmp(argv[i], "-help") == 0 || strcmp(argv[i], "-Help")== 0 )
	{
		printf("Argument Syntax:\n -i <input>\n -p <password>\n -o <option> (encrypt or decrypt)\n");
	}
	
	for(i=1; i<argc; i++)
	{
		if(strcmp(argv[i], "-i") == 0)
		{
			if(i + 1 > argc)
			{
				printf("No File inputted\n");
				printf("Argument Syntax:\n -i <input>\n -p <password>\n -o <option> (encrypt or decrypt)\n");
				return 0;
			}
			
			fileName = argv[i + 1];
		}
		if(strcmp(argv[i], "-p") == 0)
		{
			if(i + 1 > argc)
			{
				printf("No password inputted\n");
				printf("Argument Syntax:\n -i <input>\n -p <password>\n -o <option> (encrypt or decrypt)\n");
				return 0;
			}
			
			password = argv[i + 1];
		}
		if(strcmp(argv[i], '-o') == 0)
		{
			if(i + 1 > argc)
			{
				printf("You didn't specify whether to encrypt or decrypt the file\n");
				printf("Argument Syntax:\n -i <input>\n -p <password>\n -o <option> (encrypt or decrypt)\n");
				return 0;
			}
			if(strcmp(argv[i+1], 'encrypt') == 0)
			{
				option = ENCRYPT;
			}
		}
	}
	
	if(fileName == NULL)
	{
		printf("No file inputted\n");
		printf("Argument Syntax:\n -i <input>\n -p <password>\n -o <option> (encrypt or decrypt)\n");
		return 0;
	}
	if(password == NULL)
	{
		printf("No password inputted\n");
		printf("Argument Syntax:\n -i <input>\n -p <password>\n -o <option> (encrypt or decrypt)\n");
		return 0;
	}
	if(option == 0)
	{
		printf("You didn't specify whether to encrypt or decrypt the file\n");
		printf("Argument Syntax:\n -i <input>\n -p <password>\n -o <option> (encrypt or decrypt)\n");
		return 0;
	}
	
	file = fopen(argv[1], "r");
	if(!file)
	{
		printf("File %s not found\n", fileName);
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

int decryptData(Byte *data, int size, char *key)
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