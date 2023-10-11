#ifndef ENCRYPT_H
#define ENCRYPT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/file.h"
#include "../include/data.h"
#include "../include/sha256.h"
#include "../include/password.h"

#define ENCRYPTION_ROUNDS         32

enum Options {ENCRYPT = 1, DECRYPT = 2};

uint32_t encryptData(struct file* file, struct password *password);
uint32_t decryptData(struct file* file, struct password *password);

void doByteSubstitution(struct Data *data, uint8_t *shaArray);
void undoByteSubstitution(struct Data *data, uint8_t *shaArray);

#ifdef __cplusplus
}
#endif

#endif