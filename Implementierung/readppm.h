#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>

#ifndef READPPM_H
#define READPPM_H
uint8_t *readppm(const char *input_file, size_t * width, size_t * height);
#endif