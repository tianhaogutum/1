#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>

#ifndef READPPM_H
#define READPPM_H
uint8_t *readppm_for_seq(const char *input_file, size_t * width, size_t * height);
void readppm_for_simd(const char * input_file, size_t *width, size_t *height, float ** red_in_pixels, float ** green_in_pixels, float ** blue_in_pixels);
#endif