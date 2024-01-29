#include "V1.h"
#define MAX_gamma 67075968 // if base is not 1 and gamma exceeds this limit, then the result of power multiplied by 255 is always smaller then one, and after conversion to uint_8 it will be zero

static float pow_with_taylor_expansion(float, float);
static float int_power(float, uint32_t);
static float decimal_power_with_taylor_expansion(float, float);

void gamma_correct_V1(const uint8_t *img, size_t width, size_t height, float a, float b, float c, float gamma, uint8_t *result)
{
    float sum_coeffs = a + b + c;
    size_t num_pixel = width * height;
    float Q_x_y = 0;
    size_t i = 0;
    while (i < num_pixel)
    {
        Q_x_y = (a * img[3 * i] + b * img[3 * i + 1] + c * img[3 * i + 2]) / sum_coeffs;
        result[i] = pow_with_taylor_expansion(Q_x_y / 255, gamma) * 255;
        i++;
    }
}

static float pow_with_taylor_expansion(float base, float gamma) // comput base ** gamma
{
    
    if(base <= 0.5 && gamma >= 150) // if base is less than 0.5 and gamma is greater than 150, then the result is always 0
    {
        return 0;
    }

    if (base == 1.0) // if base is one, then the result is always 1
    {
        return 1;
    }
    
    ///todo:this step can move to the outer function
    
    if (gamma > MAX_gamma) // if base is less than 1, then if gamma is greater than MAX_gamma, return 0
    {
        return 0;
    }

    uint32_t index = gamma;
    gamma -= index;
    float ret = int_power(base, index);
    ret *= decimal_power_with_taylor_expansion(base, gamma);
    return ret;
}

static float decimal_power_with_taylor_expansion(float base, float decimal_gamma)
{ // we choose a = 1
    float ret = 1;
    float base_minus_a = base - 1;
    uint32_t counter = 1; // counter of terms
    float current_pow = decimal_gamma;
    float term = current_pow * base_minus_a;

    //todo:Magic number 0.00000001 is used to avoid floating point error?

    while (term > 0.00000001 || term < -0.00000001)
    {
        ret += term;
        counter += 1;
        current_pow = current_pow - 1;
        term = ((term * current_pow) / counter) * base_minus_a;
    }
    return ret;
}

static float int_power(float base, uint32_t int_pow)
{ // int_pow has 32 bits, i-th bit set means to add base ** (2 ** i) to the result, so use a loop to traverse int_pow
    float current_digit = base;
    float result = 1;
    uint32_t index_bit = 1;

    ///todo:if gamma smaller than 26, we would rather calculate it directly
    ///todo:can change 32 to 26 

    for (size_t i = 0; i < 32; ++i)
    {
        if (index_bit & int_pow)
        {
            result *= current_digit;
        }
        index_bit <<= 1;
        current_digit *= current_digit;
    }
    return result;
}