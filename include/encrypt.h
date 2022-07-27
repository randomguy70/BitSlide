#ifndef ENCRYPT_H
#define ENCRYPT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/data.h"

#define ENCRYPTION_ROUNDS 64

enum Options {ENCRYPT = 1, DECRYPT};

struct Data *encryptData(struct Data *data, char *key);
struct Data *decryptData(struct Data *data, char *key);

#ifdef __cplusplus
}
#endif

#endif