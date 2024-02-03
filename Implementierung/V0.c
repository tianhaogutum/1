#include "V0.h"

void gamma_correct(const uint8_t *img, size_t width, size_t height, float a, float b, float c, float gamma, uint8_t *result)
{
    float sum_coeffs = a + b + c;
    float a_div_sum_coeffs = a / sum_coeffs;
    float b_div_sum_coeffs = b / sum_coeffs;
    float c_div_sum_coeffs = c / sum_coeffs;
    // after the computation above, the sum of a_div_sum_coeffs, b_div_sum_coeffs, and c_div_sum_coeffs will be one, simplify the computation afterwards
    size_t num_pixel = width * height;
    float Q_x_y = 0;
    size_t i = 0;
    while (i < num_pixel)
    {
        Q_x_y = a_div_sum_coeffs * img[3 * i] + b_div_sum_coeffs * img[3 * i + 1] + c_div_sum_coeffs * img[3 * i + 2]; // make greyscale conversion
        result[i] = roundf(gamma_pow(Q_x_y, gamma));                                                                   // get the result after gamma correction, rounded to the nearest integer
        i++;
    }
}

float gamma_pow(float Q_x_y, float gamma) // make gamma correction on the greyscale values, this function is implemented in order to compare the result with reference implementation
{
    return pow(Q_x_y / 255, gamma) * 255; // Errors of pow(x, y) with the given domain of Q_x_y and gamma will not come up, underflow error will not affect the result, so we don't need error handling here
}