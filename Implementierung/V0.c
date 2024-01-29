#include "V0.h"

void gamma_correct(const uint8_t *img, size_t width, size_t height, float a, float b, float c, float gamma, uint8_t *result)
{
    float sum_coeffs = a + b + c;
    size_t num_pixel = width * height;
    float Q_x_y = 0;
    size_t i = 0;
    while (i < num_pixel)
    {
        Q_x_y = (a * img[3 * i] + b * img[3 * i + 1] + c * img[3 * i + 2]) / sum_coeffs;
        result[i] = pow(Q_x_y / 255, gamma) * 255; // should handle edge cases, errors etc.
        i++;
    }
}