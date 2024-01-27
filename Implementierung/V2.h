#include <stdint.h>
#include <stdlib.h>
#include <xmmintrin.h>
#include <immintrin.h>
#include <math.h>
#include <stdio.h>

#ifndef V2_H
#define V2_H
void gamma_correct_V2(float *red, float *green, float *blue, size_t width, size_t height, float a, float b, float c, float gamma, uint8_t *result);
#endif