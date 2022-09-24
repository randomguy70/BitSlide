#include "../include/args.h"

#include "../include/file.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void argHelp(void);

int parseArgs(int argc, char *argv[], struct args *args)
{
	char *helpVariations[] = {"Help", "-Help", "help", "-help", "H", "-H", "h", "-h", "Syntax", "-Syntax", "syntax", "-syntax", "assistance", "ayuda", "please"};
	const int numHelpVariations = sizeof(helpVariations) / sizeof(char*);
	FILE *file = NULL;
		
	// no args passed
	
	if(argc < 2)
	{
		printf("No args given\n");
		argHelp();
		return 0;
	}
	
	// help args
	
	for(int i=0; i < numHelpVariations - 1; i++)
	{
		if(strcmp(argv[1], helpVariations[i]) == 0)
		{
			argHelp();
			printf("Email me at onlychessfreeze0515@gmail.com for additional help or if you have suggestions / constructive criticism\nIf you found a bug, please submit a report on Github at\n");
			return 0;
		}
	}
	
	// get filename, password, and option
	
	for(int i=1; i<argc;)
	{
		// filename
		
		if(strcmp(argv[i], "-i") == 0)
		{
			if(i + 1 > argc)
			{
				fprintf(stderr, "No File inputted\n");
				argHelp();
				return 0;
			}
			else
			{
				args->fileName = malloc(strlen(argv[i + 1]) + 1);
				strcpy(args->fileName, argv[i + 1]);
				
				i += 2;
				if(i >= argc)
				{
					break;
				}
			}
		}
		
		// password
		
		else if(strcmp(argv[i], "-p") == 0)
		{
			if(i + 1 > argc)
			{
				fprintf(stderr, "No password inputted\n");
				argHelp();
				return 0;
			}
			else
			{
				args->pwString = malloc(strlen(argv[i + 1]) + 1);
				strcpy(args->pwString, argv[i + 1]);
				
				i += 2;
				if(i >= argc)
				{
					break;
				}
			}
		}
		
		// option (encrypt / decrypt)
		
		else if(strcmp(argv[i], "-o") == 0)
		{
			i++;
			if(i >= argc)
			{
				fprintf(stderr, "You didn't specify whether to encrypt or decrypt the file\n");
				argHelp();
				return 0;
			}
			else if(strcmp(argv[i], "encrypt") == 0)
			{
				args->option = ENCRYPT;
				i++;
				if(i >= argc)
				{
					break;
				}
			}
			else if(strcmp(argv[i], "decrypt") == 0)
			{
				args->option = DECRYPT;
				i++;
				if(i >= argc)
				{
					break;
				}
			}
			else
			{
				fprintf(stderr, "%s is not a valid option", argv[i]);
				return 0;
			}
		}
		
		// invalid arg
		
		else
		{
			printf("%s is not a valid argument\n", argv[i]);
			argHelp();
			return 0;
		}
	}
	
	// make sure we have all necessary args
	
	if(args->fileName == NULL)
	{
		fprintf(stderr, "No file inputted\n");
		argHelp();
		return 0;
	}
	else if(args->pwString == NULL)
	{
		fprintf(stderr, "No password inputted\n");
		argHelp();
		return 0;
	}
	else if(args->option == 0)
	{
		fprintf(stderr, "You didn't specify whether to encrypt or decrypt the file\n");
		argHelp();
		return 0;
	}
	
	// password requirements
	
	else if(strlen(args->pwString) < 8)
	{
		fprintf(stderr, "WARNING: Your password should be at least 8 characters for security reasons!\n");
	}
	else if(strlen(args->pwString) > 100)
	{
		fprintf(stderr, "Password can't be longer than 100 digits.\nYour password was %u digits", (unsigned) strlen(args->pwString));
		return 0;
	}
	
	// file requirements
	
	if(args->fileName != NULL)
	{
		file = fopen(args->fileName, "r");
		if(!file)
		{
			fprintf(stderr, "File %s not found\n", args->fileName);
			return 0;
		}
		else if(getFileSize(file) == 0)
		{
			fprintf(stderr, "File %s is empty\n", args->fileName);
			return 0;
		}
	}
	
	// SUCCESS!!
	
	return 1;
}

void argHelp(void)
{
	printf("\nArgument Syntax:\n -i <input>\n -p <password>\n -o <option> (encrypt or decrypt)\n\n");
	printf("Email me at onlychessfreeze0515@gmail.com for additional help, if you found a bug, or if you have suggestions / constructive criticism\n");
}