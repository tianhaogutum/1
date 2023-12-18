#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef V0_H
#define V0_H

void gamma_correct(const uint8_t* img, size_t width, size_t height, float a, float b, float c, float gamma, uint8_t* result);

#endif