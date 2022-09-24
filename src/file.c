#include "../include/file.h"

#include "../include/data.h"
#include "../include/sha256.h"
#include "../include/password.h"
#include "../include/data.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// sha256 hash of "BitSlide"
// shows that a file was encrypted by my encrypter (BitSlide)

static const uint8_t bitSlideSignature[BITSLIDE_SIGNATURE_SIZE_BYTES] =
{
	0xf1, 0x00, 0xf3, 0xc8, 0x02, 0x58, 0xff, 0xa1,
	0xdd, 0x16, 0xe4, 0x9b, 0xd0, 0x84, 0x66, 0x7f,
	0x83, 0x65, 0xdf, 0xb6, 0xdc, 0x3f, 0x6d, 0x6f,
	0xcd, 0xd0, 0xdc, 0xed, 0xa1, 0xad, 0xb3, 0x95,
};

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

void loadFileHeader(FILE *fptr, struct file *file, struct password *password)
{
	fseek(fptr, BITSLIDE_SIGNATURE_OFFSET, SEEK_SET);
	fread(file->signature, BITSLIDE_SIGNATURE_SIZE_BYTES, 1, fptr);
	fseek(fptr, SALT_OFFSET, SEEK_SET);
	fread(password->salt, SALT_SIZE_BYTES, 1, fptr);
	fseek(fptr, SHA_VERIFICATION_OFFSET, SEEK_SET);
	fread(file->vHash, SHA_VERIFICATION_SIZE_BYTES, 1, fptr);
}

uint32_t verifyFileHeader(struct file *file, struct password *password)
{
	if(memcmp(file->vHash, password->checkHash, SHA_VERIFICATION_SIZE_BYTES) != 0)
	{
		printf("Either your password is wrong, or file %s has been tampered with since encryption.\n", file->name);
		return 0;
	}
	if(memcmp(file->signature, bitSlideSignature, BITSLIDE_SIGNATURE_SIZE_BYTES) != 0)
	{
		printf("File %s was not encrypted with BitSlide\n", file->name);
		return 0;
	}
	
	return 1;
}

uint32_t loadUnencryptedFileData(struct file *file)
{
	FILE *fptr;
	uint32_t dataSize;
	
	fptr = fopen(file->name, "r");
	if(!fptr)
	{
		printf("File %s not found\n", file->name);
		return 0;
	}
	
	dataSize = getFileSize(fptr);
	if(dataSize == 0)
	{
		fprintf(stderr, "File %s is empty\n", file->name);
		return 0;
	} else if(dataSize > MAX_INPUT_DATA_SIZE)
	{
		fprintf(stderr, "File %s is too big to encrypt. Max input size is %d bytes.\n", file->name, MAX_INPUT_DATA_SIZE);
		return 0;
	}
	
	// increase the data array to append the size at the end
	// and increase the data size to a multiple of 256
	// XXX change 256's to DATA_BLOCK_SIZE_BYTES
	
	file->data.size = dataSize + sizeof(uint32_t);
	if(file->data.size % 256 != 0)
	{
		file->data.size += 256 - (file->data.size % 256);
	}
	
	file->data.ptr = malloc(file->data.size);
	fread(file->data.ptr, dataSize, 1, fptr);
	fclose(fptr);
	
	// write the size to the end of the data array
	
	*(uint32_t*)(file->data.ptr + file->data.size - (uint32_t)(sizeof(uint32_t))) = dataSize;
	
	return 1;
}

uint32_t loadEncryptedFileData(struct file *file, struct password *password)
{	
	FILE *fptr = fopen(file->name, "r");
	if(!fptr) return 0;
	
	uint32_t fileSize = getFileSize(fptr);
	uint32_t dataSize = fileSize - HEADER_SIZE_BYTES;
	
	if(fileSize < MIN_FILE_SIZE || fileSize > MAX_ENCRYPTED_FILE_SIZE || dataSize % 256 != 0)
	{
		printf("File %s is either too big or has been tampered with.\nCan not decrypt.\n", file->name);
		return 0;
	}
	
	file->data.size = dataSize;
	file->data.ptr = malloc(dataSize);
	
	// load header
	
	fseek(fptr, 0, SEEK_SET);
	fread(file->signature, BITSLIDE_SIGNATURE_SIZE_BYTES, 1, fptr);
	fread(password->salt, SALT_SIZE_BYTES, 1, fptr);
	fread(file->vHash, SHA_VERIFICATION_SIZE_BYTES, 1, fptr);
	
	// load data
	
	fread(file->data.ptr, dataSize, 1, fptr);
	
	return 1;
}

uint32_t writeEncryptedFile(struct file *file, struct password *password)
{
	FILE *fptr;
	
	fptr = fopen(file->name, "w+");
	if(!fptr)
	{
		printf("Not able to overwrite file %s\n", file->name);
		return 0;
	}
	
	fwrite(bitSlideSignature, BITSLIDE_SIGNATURE_SIZE_BYTES, 1, fptr);
	fwrite(password->salt, SALT_SIZE_BYTES, 1, fptr);
	fwrite(password->checkHash, 8, 1, fptr);
	fwrite(file->data.ptr, file->data.size, 1, fptr);
	fclose(fptr);
	
	printf("Wrote %d bytes to file %s\n", HEADER_SIZE_BYTES + file->data.size, file->name);
	
	return 1;
}

uint32_t writeDecryptedFile(struct file *file)
{
	FILE *fptr;
	
	if(file->data.size > MAX_INPUT_DATA_SIZE)
	{
		printf("Output is too large\nTry editing the MAX_INPUT_DATA_SIZE in the source code and recompiling\n");
		return 0;
	}
	
	fptr = fopen(file->name, "w+");
	if(!fptr)
	{
		printf("Not able to overwrite file %s\n", file->name);
		return 0;
	}
	
	fseek(fptr, 0, SEEK_SET);
	fwrite(file->data.ptr, file->data.size, 1, fptr);
	fclose(fptr);
	
	printf("Wrote %d bytes to file %s\n", file->data.size, file->name);
	
	return 1;
}