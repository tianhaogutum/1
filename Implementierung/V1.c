#include "V1.h"
#include <stdio.h>//for testing and debugging, should be removed

static float pow_with_tylor_expansion(float, float);

void gamma_correct_V1(const uint8_t *img, size_t width, size_t height, float a, float b, float c, float gamma, uint8_t *result)
{
    float sum_coeffs = a + b + c;
    size_t num_pixel = width * height;
    float Q_x_y = 0;
    size_t i = 0;
    while (i < num_pixel)
    {
        Q_x_y = (a * img[3 * i] + b * img[3 * i + 1] + c * img[3 * i + 2]) / sum_coeffs;
        result[i] = pow_with_tylor_expansion(Q_x_y / 255, gamma) * 255;
        printf("%u\n", result[i]);
        i++;
    }
}

static float pow_with_tylor_expansion(float base, float gamma) // comput base ** gamma
{                                                              // we choose a = 1
    float ret = 1;
    float base_minus_a = base - 1;
    uint32_t counter = 1; // counter of terms
    float current_pow = gamma;
    float term = current_pow * base_minus_a;
    while (term > 0.00000000001 || term < -0.00000000001)
    {
        ret += term;
        counter += 1;
        current_pow = current_pow - 1;
        term = ((term * current_pow) / counter) * base_minus_a;
    }
    return ret;
}