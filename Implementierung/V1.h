#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#ifndef V1_H
#define V1_H
    void gamma_correct_V1(const uint8_t *img, size_t width, size_t height, float a, float b, float c, float gamma, uint8_t *result);
    float pow_with_taylor_expansion(float, float);
#endif