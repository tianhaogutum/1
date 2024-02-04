#include "V1.h"
#define MAX_gamma 75464577 // if base is not 1 and gamma exceeds this limit, then the result of power multiplied by 255 is always smaller then 0.5, and after conversion to uint_8 it will be zero

static float int_power(float, uint32_t);
static float decimal_power_with_taylor_expansion(float, float);

void gamma_correct_V1(const uint8_t *img, size_t width, size_t height, float a, float b, float c, float gamma, uint8_t *result)
{
    float sum_coeffs = a + b + c;
    float a_div_sum_coeffs = a / sum_coeffs;
    float b_div_sum_coeffs = b / sum_coeffs;
    float c_div_sum_coeffs = c / sum_coeffs;
    // after the computation above, the sum of a_div_sum_coeffs, b_div_sum_coeffs, and c_div_sum_coeffs will be 1
    size_t num_pixel = width * height;
    float Q_x_y = 0;
    size_t i = 0;
    while (i < num_pixel)
    {
        Q_x_y = a_div_sum_coeffs * img[3 * i] + b_div_sum_coeffs * img[3 * i + 1] + c_div_sum_coeffs * img[3 * i + 2]; // make greyscale conversion
        result[i] = roundf(pow_with_taylor_expansion(Q_x_y / 255, gamma) * 255);                                       // make gamma correction with taylor expansion, round the result to the nearest integer
        i++;
    }
}

float pow_with_taylor_expansion(float base, float gamma) // comput base ** gamma
{

    if (base <= 0.5 && gamma >= 150) // if base is less than 0.5 and gamma is greater than 150, then the result is always 0
    {
        return 0;
    }
    else if (base == 1.0) // if base is one, then the result is always 1
    {
        return 1;
    }
    else if (gamma > MAX_gamma) // if base is less than 1, then if gamma is greater than MAX_gamma, return 0
    {
        return 0;
    }

    uint32_t index = gamma;                                  // round gamma to the nearest smaller integer
    gamma -= index;                                          // gamma is now its decimal part
    float ret = int_power(base, index);                      // compute integer power of the base
    ret *= decimal_power_with_taylor_expansion(base, gamma); // compute decimal power of the base
    return ret;
}
//f(x) = f(a) + f'(a)*(x-a) + (f'(a)*f''(a) / 2)*(x-a)^2 + ((f'(a)*f''(a)*f'''(a)) / (3*2))*(x-a)^3 + ........, f(x) = x^gamma, take a = 1
static float decimal_power_with_taylor_expansion(float base, float decimal_gamma)
{                           // we choose a = 1, as this will simplify the calculation
    if (decimal_gamma == 0) // when gamma is zero, then result will always be 1
    {
        return 1;
    }
    else if (base == 0) // when base is zero and gamma is not zero, then result will always be 0. As the black pixels (0,0,0) could be quite common in images, this will be handled as special case
    {
        return 0;
    }
    float ret = 1;                                                                // 1 is already the first element in taylor expansion
    float base_minus_a = base - 1;                                                // save base - 1, to be multiplied to n_pow_base_minus_a
    uint64_t counter = 1;                                                         // counter of terms
    float current_pow = decimal_gamma;                                            // will be updated to gamma - 1, gamma - 2, ......
    float n_pow_base_minus_a = base_minus_a;                                      // will be updated to (base - 1)**2, (base - 1)**3, ......
    float term = current_pow;                                                     // term will be the coeffients of (base - 1)**n in taylor expansion
    while (n_pow_base_minus_a > 0.000000001 || n_pow_base_minus_a < -0.000000001) // condition for loop end, according to Peano rest part of the taylor expansion, the delta after n terms will be o((base - 1)**n), so the value set will be a upper bound of delta, this upper bound is sufficient to ensure the delta is smaller than 1/255, so that the absolute error after multiplied by 255 and integer conversion will be less than 1
    {
        ret += term * n_pow_base_minus_a; // coefficients*(base - 1)**n
        counter += 1;                     // update counter
        current_pow = current_pow - 1;
        n_pow_base_minus_a *= base_minus_a;
        term *= current_pow / counter; // update the coefficient
        if (base_minus_a == -1 && current_pow == -16777216)
        { // if base is too small, the base - 1 will be -1(absorption), this will cause endless while loop and needs to be handled, in such case, the loop ends at the condition of current_pow = -16777216, as from this number, all following current_pow will remain this number.
            break;
        }
    }
    return ret;
}

static float int_power(float base, uint32_t int_pow) // int_pow has 32 bits, i-th bit set means to multiply base ** (2 ** i) to the result (i from 0 to 31), so use a loop to traverse int_pow
{
    float current_digit = base; // used to store base^1, base^2, base^4, base^8,.......,base^(2^31)
    float result = 1;
    uint32_t index_bit = 1; // with this bit we can use bitwise & to test if certain bit in int_pow is set
    if (int_pow < 27)       // if int_pow is smaller than 27, then use direct mul in loop
    {
        for (size_t i = 0; i < int_pow; ++i)
        {
            result *= base;
        }
    }
    else
    {
        for (size_t i = 0; i < 27; ++i) // int_pow cannot be greater than MAX_gamma, so no more than 27 bits in int_pow can be set
        {
            if (index_bit & int_pow)
            {
                result *= current_digit;
            }
            index_bit <<= 1;                // prepare for the next loop
            current_digit *= current_digit; // prepare for the next loop
        }
    }
    return result;
}