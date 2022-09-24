#include "../include/data.h"
#include "../include/file.h"
#include "../include/encrypt.h"
#include "../include/password.h"
#include "../include/args.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

void logRunTime(void);

int main(int argc, char *argv[])
{   
	struct file file;
	struct password password;
	struct args args;
	
	if(!parseArgs(argc, argv, &args)) return 0;
	
	password.string = args.pwString;
	file.name = args.fileName;
	
	if(args.option == ENCRYPT)
	{
		saltPassword(&password);
		hashPassword(&password);
		
		// load the file data, encrypt it, and overwrite the file's data
		
		if(!loadUnencryptedFileData(&file))       return 0;
		if(!encryptData(&file, &password))        return 0;
		if(!writeEncryptedFile(&file, &password)) return 0;
		
		logRunTime();
		return 0;
	}
	
	// decrypting
	else if(args.option == DECRYPT)
	{
		if(!loadEncryptedFileData(&file, &password)) return 0;
		
		hashPassword(&password);
		if(!verifyFileHeader(&file, &password)) return 0;
		
		decryptData(&file, &password);
		writeDecryptedFile(&file);
		
		logRunTime();
		return 1;
	}
	
	// shouldn't ever get here, but just in case...
	
	else
	{
	 	printf("Please submit an error report on Github if this message ever shows!\nThe program will quit now...\n");
		logRunTime();
	 	return 0;
	}
	
	return 0;
}

void logRunTime(void)
{
	printf("Execution time: %lu MS.\n", clock()/(CLOCKS_PER_SEC / 1000));
}