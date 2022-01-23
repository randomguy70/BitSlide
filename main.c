#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "encrypt.h"
#include "file.h"

int main(int argc, char *argv[])
{
	char *fileName = NULL;
	char *password = NULL;    // might turn this into a Data struct
	enum Options option = 0;
	FILE *file;
	struct Data *data = malloc(sizeof(struct Data));
	int i = 0;
	
	if(argc < 2)
	{
		printf("No input file or args given\n");
		printf("Argument Syntax:\n -i <input>\n -p <password>\n -o <option> (encrypt or decrypt)\n");
		return 0;
	}
	
	// parse args
	if(strcmp(argv[1], "Help") == 0 || strcmp(argv[1], "h") == 0 || strcmp(argv[1], "H") == 0 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-H") == 0 || strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "-Help")== 0 )
	{
		printf("Argument Syntax:\n -i <input>\n -p <password>\n -o <option> (encrypt or decrypt)\n");
		return 0;
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
			else
			{
				fileName = argv[i + 1];
				i += 2;
				if(i >= argc)
				{
					break;
				}
			}
		}
		else if(strcmp(argv[i], "-p") == 0)
		{
			if(i + 1 > argc)
			{
				printf("No password inputted\n");
				printf("Argument Syntax:\n -i <input>\n -p <password>\n -o <option> (encrypt or decrypt)\n");
				return 0;
			}
			else
			{
				password = argv[i + 1];
				i += 2;
				if(i >= argc)
				{
					break;
				}
			}
		}
		else if(strcmp(argv[i], "-o") == 0)
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
				i += 2;
				if(i >= argc)
				{
					break;
				}
			}
			else if(strcmp(argv[i+1], "decrypt") == 0)
			{
				option = DECRYPT;
				i += 2;
				if(i >= argc)
				{
					break;
				}
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
	
	if(file)
	{
		data->size = getFileSize(file);
		data->ptr = malloc(data->size);
		fclose(file);
	}
	else
	{
		printf("File %s not found\n", fileName);
		return 0;
	}
	
	// encrypting
	if(option == ENCRYPT)
	{
		// data = encryptData(data, password);
		data->ptr = NULL;
		data->size = 0;
		
		file = fopen(fileName, "w");
		
		if(file)
		{
			fseek(file, 0L, SEEK_SET);
			fwrite(data->ptr, data->size, 1, file);
			fclose(file);
			
			printf("Encrypted data %d bytes\nWrote to file %s", data->size, fileName);
		}
		else
		{
			printf("Not able to write to overwrite file %s", fileName);
		}
		
		return 0;
	}
	
	// decrypting
	else if(option == DECRYPT)
	{
		// decryptData(&data, password);
		
		file = fopen(fileName, "w");
		
		if(file)
		{
			fseek(file, 0L, SEEK_SET);
			fwrite(data->ptr, data->size, 1, file);
			fclose(file);
			
			printf("Decrypted data %d bytes\nWrote to file %s", data->size, fileName);
		}
		else
		{
			printf("Original file not found");
		}
		
		return 0;
	}
	
	// if option is not valid (it never will get to this statement, but just in case)
	else
	{
	 	printf("Horrible disaster. Exiting...");
	 	return 0;
	}
	
	return 0;
}