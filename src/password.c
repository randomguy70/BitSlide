#include "../include/password.h"

#include "../include/sha256.h"
#include "../include/data.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

void saltPassword(struct password *password)
{
	// Yeah yeah, I know rand() isn't cryptographically secure but a salt
	// just has to be unique to prevent a common hash dictionary lookup.
	// However, if I'm wrong, submit an error on the Github repo.
	
	time_t t;
	srand(time(&t));
	
	for(uint32_t i = 0; i < (SHA256_SIZE_BYTES / (uint32_t) sizeof(uint32_t)); i++)
	{
		((uint32_t *) password->salt)[i] = (uint32_t) rand();
	}
	
	// make it a teeny bit more random by replacing it with it's hash
	
	sha256(password->salt, SHA256_SIZE_BYTES, password->salt);
}

void hashPassword(struct password *password)
{	
	// copy the salt + password into an array
	
	uint32_t arrSize = SHA256_SIZE_BYTES + strlen(password->string);
	uint8_t *saltedPw = calloc(1, arrSize);
	
	memcpy(saltedPw, password->salt, SHA256_SIZE_BYTES);
	memcpy(saltedPw + SHA256_SIZE_BYTES, password->string, strlen(password->string));
	
	// hash the whole array
	
	sha256(saltedPw, arrSize, password->hash);
	
	// initialise the hash table
	
	wipeBytes(password->shaArray, 256);
	
	// create a hash data array with hash iterations 1-8 of the password hash
	
	sha256(password->hash, SHA256_SIZE_BYTES, (uint8_t*) password->shaArray);
		
	for(int i = 0; i < 7; i++)
	{
		sha256((uint8_t*) password->shaArray + (i * SHA256_SIZE_BYTES), SHA256_SIZE_BYTES, (uint8_t*) password->shaArray + (i + 1) * SHA256_SIZE_BYTES);
	}
	
	// create the password verification hash
		
	sha256((uint8_t*) password->shaArray + 256 - 32, SHA256_SIZE_BYTES, password->checkHash);
}

void printPasswordData(struct password *password)
{
	printf("\nSalt:");
	for(int i = 0; i < 32; i++)
	{
		if(i % 8 == 0) printf("\n");
		printf("%x", password->salt[i]);
	}
	printf("\n\n");
	printf("Password Hash");
	for(int i = 0; i < 32; i++)
	{
		if(i % 8 == 0) printf("\n");
		printf("%x", password->hash[i]);
	}
	printf("\n\n");
	printf("Password Check Hash");
	for(int i = 0; i < 32; i++)
	{
		if(i % 8 == 0) printf("\n");
		printf("%x", password->checkHash[i]);
	}
	printf("\n\n");
	printSHA256Table(password->shaArray, 256);
}