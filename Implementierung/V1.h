#include <stdio.h>
#include <stdint.h>

#ifndef V1_H
#define V1_H
    void gamma_correct_V1(const uint8_t *img, size_t width, size_t height, float a, float b, float c, float gamma, uint8_t *result);
#endif