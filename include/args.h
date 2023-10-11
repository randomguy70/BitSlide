#ifndef ARGS_H
#define ARGS_H

#include "../include/encrypt.h"

#ifdef __cplusplus
extern "C" {
#endif

struct args
{
	char *fileName;
	char *pwString;
	enum Options option;
};

int parseArgs(int argc, char *argv[], struct args *args);

#ifdef __cplusplus
}
#endif

#endif