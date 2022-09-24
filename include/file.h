#ifndef FILE_H
#define FILE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/data.h"
#include "../include/sha256.h"
#include "../include/password.h"

#include <stdio.h>
#include <stdbool.h>

#define BITSLIDE_SIGNATURE_SIZE_BYTES     32

#define HEADER_SIZE_BYTES                 72

#define BITSLIDE_SIGNATURE_OFFSET         0
#define BITSLIDE_SIGNATURE_SIZE_BYTES     32
#define SALT_OFFSET                       32
#define SALT_SIZE_BYTES                   32
#define SHA_VERIFICATION_OFFSET           64
#define SHA_VERIFICATION_SIZE_BYTES       8

#define ENCRYPTED_DATA_OFFSET             72

#define MIN_FILE_SIZE                     HEADER_SIZE_BYTES + 256
#define MAX_INPUT_DATA_SIZE               1000000000 // 1 Gigabyte
#define MAX_ENCRYPTED_FILE_SIZE           MAX_INPUT_DATA_SIZE + 256 + HEADER_SIZE_BYTES

/*             Layout of Encrypted File
	+---------------------------------------------+
	| Offset | Size | Name                        |
	+--------+------+-----------------------------+
	| 0      | 32   | BitSlide Signature          | <--|
	| 32     | 32   | Salt Bytes                  | <--| Header (72 Bytes)
	| 64     | 8    | Password Verification Bytes | <--|
	| 72     | n + 4| Encrypted Data              |
	+---------------------------------------------+
*/

struct file
{
	char *name;
	uint32_t size;
	struct Data data;
	struct DataBlock *firstBlock;
	
	uint8_t signature[32]; // BitSlide signature read from file
	uint8_t vHash[SHA_VERIFICATION_SIZE_BYTES]; // 8 bytes password verification bytes read from file
};

// returns the size of any file and preserves the ftell location
int getFileSize(FILE *file);

// loads the stored salt and verification hash from an opened file
// stores the salt in a password struct and the verification hash into a file struct
void loadFileHeader(FILE *fptr, struct file *file, struct password *password);

// checks if first 8 bytes of hash of the salt + password == 8 bytes stored in file
uint32_t verifyFileHeader(struct file *file, struct password *password);

uint32_t loadUnencryptedFileData(struct file *file);

uint32_t loadEncryptedFileData(struct file *file, struct password *password);

// stores the metadata and encrypted data of an encrypted file
uint32_t writeEncryptedFile(struct file *file, struct password *password);

// does what it says
uint32_t writeDecryptedFile(struct file *file);

#ifdef __cplusplus
}
#endif

#endif