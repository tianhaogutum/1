#include "V1.h"

static float pow_with_tylor_expansion(float, float);

void gamma_correct_V1(const uint8_t *img, size_t width, size_t height, float a, float b, float c, float gamma, uint8_t *result){
    float sum_coeffs = a + b + c;
    size_t num_pixel = width * height;
    float Q_x_y = 0;
    size_t i = 0;
    while (i < num_pixel)
    {
        Q_x_y = (a * img[3 * i] + b * img[3 * i + 1] + c * img[3 * i + 2]) / sum_coeffs;
        result[i] = pow_with_tylor_expansion(Q_x_y, gamma);
        printf((uint8_t) Q_x_y == result[i] ? "" : "pixel %lu is different.\n", i);
        i++;
    }
}

static float pow_with_tylor_expansion(float Q_x_y, float gamma){//we choose a = 1
    float ret = 1;
    float x_minus_a = Q_x_y - 1;
    uint32_t counter = 1;
    float current_pow = gamma;
    float term = (current_pow / counter) * x_minus_a;
    while(term != 0){
        ret += term;
        counter += 1;
        current_pow = gamma - 1;
        term = ((term * current_pow)/counter) * x_minus_a;
    }
    return ret;
}