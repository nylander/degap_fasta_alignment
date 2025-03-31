#ifndef DFA_H
#define DFA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <libgen.h>
#include <zlib.h>

#define bit_array_t unsigned char
#define VERSION_STR "0.1.1"
#define DEFAULT_MISSING_CHARS "Nn?Xx-"
#define DEFAULT_WRAP_LENGTH 60

void printUsage(const char *program_name);
void printVersion();
long int getSeqLength(const char *file_name);
void *createBitArray(long int n);
void initateBitArray(const char *file_name, bit_array_t *bit_array, const char *missing_chars);
void printBitArray(bit_array_t *bit_array, long int n);
void printFasta(const char *file_name, bit_array_t *bit_array, int wrap_length);
void freeBitArray(bit_array_t *bit_array);

#endif
