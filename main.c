#include <stdio.h>
#include <string.h>

#include "main.h"
#include "encrypt.h"
#include "file.h"

int main(int argc, char *argv[])
{
	char *fileName = NULL;
	char *password = NULL;
	enum Options option = 0;
	FILE *file;
	struct Data data = {.ptr = NULL, .size = 0};
	int i;
	
	// parse args
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
		if(strcmp(argv[i], "-o") == 0)
		{
			if(i + 1 > argc)
			{
				printf("You didn't specify whether to encrypt or decrypt the file\n");
				printf("Argument Syntax:\n -i <input>\n -p <password>\n -o <option> (encrypt or decrypt)\n");
				return 0;
			}
			if(strcmp(argv[i+1], "encrypt") == 0)
			{
				option = ENCRYPT;
			}
			else if(strcmp(argv[i+1], "decrypt") == 0)
			{
				option = DECRYPT;
			}
			else
			{
				printf("%s is not a valid option", argv[i + 1]);
				return 0;
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
	
	file = fopen(fileName, "r");
	if(!file)
	{
		printf("File %s not found\n", fileName);
		return 0;
	}
	
	data.size = getFileSize(file);
	fclose(file);
	
	if(option == ENCRYPT)
	{
		encryptData(&data, password);
		
		file = fopen(fileName, "w");
		if(!file)
		{
			printf("Original file not found");
			return 0;
		}
		fseek(file, 0L, SEEK_SET);
		fwrite(data.ptr, data.size, 1, file);
		fclose(file);
		
		printf("Encrypted file: %zu bytes int", data.size);
		return 0;
	}
	else if(option == DECRYPT)
	{
		decryptData(&data, password);
		
		file = fopen(fileName, "w");
		if(!file)
		{
			printf("Original file not found");
			return 0;
		}
		fseek(file, 0L, SEEK_SET);
		fwrite(data.ptr, data.size, 1, file);
		fclose(file);
		
		printf("Decrypted data %zu bytes int", data.size);
		return 0;
	}
	else {
	 	printf("Horrible disaster. Exiting...");
	 	return 0;
	}
	
	return 0;
}