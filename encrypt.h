#ifndef ENCRYPT_H
#define ENCRYPT_H

#include "main.h"

enum Options {ENCRYPT = 1, DECRYPT};

struct Data encryptData(struct Data *data, char *key);
struct Data decryptData(struct Data *data, char *key);

#endif