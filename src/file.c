#include "../include/file.h"

#include "../include/data.h"
#include "../include/sha256.h"
#include "../include/password.h"
#include "../include/data.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const uint8_t K[] = {
    0x42, 0x8a, 0x2f, 0x98, 0x71, 0x37, 0x44, 0x91, 0xb5, 0xc0, 0xfb, 0xcf, 0xe9, 0xb5, 0xdb, 0xa5,
    0x39, 0x56, 0xc2, 0x5b, 0x59, 0xf1, 0x11, 0xf1, 0x92, 0x3f, 0x82, 0xa4, 0xab, 0x1c, 0x5e, 0xd5,
    0xd8, 0x07, 0xaa, 0x98, 0x12, 0x83, 0x5b, 0x01, 0x24, 0x31, 0x85, 0xbe, 0x55, 0x0c, 0x7d, 0xc3,
    0x72, 0xbe, 0x5d, 0x74, 0x80, 0xde, 0xb1, 0xfe, 0x9b, 0xdc, 0x06, 0xa7, 0xc1, 0x9b, 0xf1, 0x74,
    0xe4, 0x9b, 0x69, 0xc1, 0xef, 0xbe, 0x47, 0x86, 0x0f, 0xc1, 0x9d, 0xc6, 0x24, 0x0c, 0xa1, 0xcc,
    0x2d, 0xe9, 0x2c, 0x6f, 0x4a, 0x74, 0x84, 0xaa, 0x5c, 0xb0, 0xa9, 0xdc, 0x76, 0xf9, 0x88, 0xda,
    0x98, 0x3e, 0x51, 0x52, 0xa8, 0x31, 0xc6, 0x6d, 0xb0, 0x03, 0x27, 0xc8, 0xbf, 0x59, 0x7f, 0xc7,
    0xc6, 0xe0, 0x0b, 0xf3, 0xd5, 0xa7, 0x91, 0x47, 0x06, 0xca, 0x63, 0x51, 0x14, 0x29, 0x29, 0x67,
    0x27, 0xb7, 0x0a, 0x85, 0x2e, 0x1b, 0x21, 0x38, 0x4d, 0x2c, 0x6d, 0xfc, 0x53, 0x38, 0x0d, 0x13,
    0x65, 0x0a, 0x73, 0x54, 0x76, 0x6a, 0x0a, 0xbb, 0x81, 0xc2, 0xc9, 0x2e, 0x92, 0x72, 0x2c, 0x85,
    0xa2, 0xbf, 0xe8, 0xa1, 0xa8, 0x1a, 0x66, 0x4b, 0xc2, 0x4b, 0x8b, 0x70, 0xc7, 0x6c, 0x51, 0xa3,
    0xd1, 0x92, 0xe8, 0x19, 0xd6, 0x99, 0x06, 0x24, 0xf4, 0x0e, 0x35, 0x85, 0x10, 0x6a, 0xa0, 0x70,
    0x19, 0xa4, 0xc1, 0x16, 0x1e, 0x37, 0x6c, 0x08, 0x27, 0x48, 0x77, 0x4c, 0x34, 0xb0, 0xbc, 0xb5,
    0x39, 0x1c, 0x0c, 0xb3, 0x4e, 0xd8, 0xaa, 0x4a, 0x5b, 0x9c, 0xca, 0x4f, 0x68, 0x2e, 0x6f, 0xf3,
    0x74, 0x8f, 0x82, 0xee, 0x78, 0xa5, 0x63, 0x6f, 0x84, 0xc8, 0x78, 0x14, 0x8c, 0xc7, 0x02, 0x08,
    0x90, 0xbe, 0xff, 0xfa, 0xa4, 0x50, 0x6c, 0xeb, 0xbe, 0xf9, 0xa3, 0xf7, 0xc6, 0x71, 0x78
};

// sha256 hash of "BitSlide"
// shows that a file was encrypted by my encrypter (BitSlide)

static const uint8_t bitSlideSignature[BITSLIDE_SIGNATURE_SIZE_BYTES] = {
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
	
	// fill the empty part at the end of the array with pseudo-random constants
	
	for(unsigned int i = dataSize, rand = 0; i < file->data.size; i++, rand++)
	{
		file->data.ptr[i] = K[rand];
	}
	
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