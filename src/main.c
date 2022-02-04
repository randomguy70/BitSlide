#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/main.h"
#include "../include/file.h"
#include "../include/encrypt.h"

void argHelp(void);

int main(int argc, char *argv[])
{
	char *fileName = NULL;
	char *password = NULL;    // might turn this into a Data struct
	enum Options option = 0;
	FILE *file;
	struct Data *data = malloc(sizeof(struct Data));
	
	// no args passed
	if(argc < 2)
	{
		printf("No args given\n");
		argHelp();
		return 0;
	}
	
	// help arg(s)
	{
		char *variations[] = {"Help", "-Help", "help", "-help", "H", "-H", "h", "-h", "Syntax", "-Syntax", "syntax", "-syntax", "assistance", "ayuda", "please"};
		// const int numVariations = sizeof(variations) / sizeof(char*);
		int numVariations = 15;
		
		for(int i=0; i < numVariations - 1; i++)
		{
			if(strcmp(argv[1], variations[i]) == 0)
			{
				argHelp();
				printf("Email me at onlychessfreeze0515@gmail.com for additional help, if you found a bug, or if you have suggestions / constructive criticism\n");
				return 0;
			}
		}
	}
	
	for(int i=1; i<argc;)
	{
		if(strcmp(argv[i], "-i") == 0)
		{
			if(i + 1 > argc)
			{
				printf("No File inputted\n");
				argHelp();
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
				argHelp();
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
			i++;
			if(i >= argc)
			{
				printf("You didn't specify whether to encrypt or decrypt the file\n");
				argHelp();
				return 0;
			}
			if(strcmp(argv[i], "encrypt") == 0)
			{
				option = ENCRYPT;
				i++;
				if(i >= argc)
				{
					break;
				}
			}
			else if(strcmp(argv[i], "decrypt") == 0)
			{
				option = DECRYPT;
				i++;
				if(i >= argc)
				{
					break;
				}
			}
			else
			{
				printf("%s is not a valid option", argv[i]);
				return 0;
			}
		}
		else
		{
			printf("%s is not a valid argument\n", argv[i]);
			argHelp();
			return 0;
		}
	}
	
	if(fileName == NULL)
	{
		printf("No file inputted\n");
		argHelp();
		return 0;
	}
	if(password == NULL)
	{
		printf("No password inputted\n");
		argHelp();
		return 0;
	}
	if(option == 0)
	{
		printf("You didn't specify whether to encrypt or decrypt the file\n");
		argHelp();
		return 0;
	}
	
	file = fopen(fileName, "r");
	if(!file)
	{
		printf("File %s not found\n", fileName);
		return 0;
	}
	
	data = getFileData(file);
	fclose(file);
	
	// encrypting
	if(option == ENCRYPT)
	{
		data = encryptData(data, password);
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

void argHelp(void)
{
	printf("\nArgument Syntax:\n -i <input>\n -p <password>\n -o <option> (encrypt or decrypt)\n\n");
	printf("Email me at onlychessfreeze0515@gmail.com for additional help, if you found a bug, or if you have suggestions / constructive criticism\n");
}