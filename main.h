#ifndef MAIN_H
#define MAIN_H

typedef unsigned char Byte;
enum Options {ENCRYPT = 1, DECRYPT};

int encryptData(Byte *data, int size, char *key);
int decryptData(Byte *data, int size, char *key);

#endif