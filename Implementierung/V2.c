#include "V2.h"

static float *packed_compute_greyscale(const float *, const float *, const float *, size_t, size_t, float, float, float);

void gamma_correct_V2(float *red, float *green, float *blue, size_t width, size_t height, float a, float b, float c, float gamma, uint8_t *result)
{
    float *greyscale_value_of_pixels = packed_compute_greyscale(red, green, blue, width, height, a, b, c);//this function uses simd to compute greyscale converison
    if(!greyscale_value_of_pixels){//check if the allocation in packed_compute_greyscale succeeded
        free(red);
        free(green);
        free(blue);
        free(result);
        fprintf(stderr, "Cannot allocate space for grey scale values. Program terminated.\n");
        exit(EXIT_FAILURE);
    }
    size_t number_of_pixels = width * height;
    for (size_t i = 0; i < number_of_pixels; ++i)//do gamma correction with pow function from pixel to pixel
    {
        result[i] = roundf(pow(greyscale_value_of_pixels[i], gamma) * 255);//round the result to the nearest integer
    }
    free(greyscale_value_of_pixels);
}

static float *packed_compute_greyscale(const float *red, const float *green, const float *blue, size_t width, size_t height, float a, float b, float c)
{
    float sum_coeffs = a + b + c;
    __m128 packed_a_div_sum_coeffs = _mm_set1_ps(a / sum_coeffs);//load a_div_sum_coeffs to xmm register
    __m128 packed_b_div_sum_coeffs = _mm_set1_ps(b / sum_coeffs);//load b_div_sum_coeffs to xmm register
    __m128 packed_c_div_sum_coeffs = _mm_set1_ps(c / sum_coeffs);//load c_div_sum_coeffs to xmm register
    __m128 packed_255 = _mm_set1_ps(255.0);//load 255.0 to all positions of xmm register
    size_t number_of_pixels = width * height;
    size_t color_buffer_size = (((number_of_pixels) << 2) & 0xfffffffffffffff0) + 16;
    float *greyscale_value_of_pixels_div_by_255 = aligned_alloc(16, color_buffer_size);//allocate space for result. As the input buffer shouldn't be modified due to the possible benchmarking afterwards, we should allocate new space.
    if(!greyscale_value_of_pixels_div_by_255){//check if allocation succeeded
        return NULL;
    }
    for (size_t i = 0; i < number_of_pixels; i += 4)
    {
        __m128 a_mul_red = _mm_mul_ps(_mm_load_ps(red + i), packed_a_div_sum_coeffs);//packed red mul packed a
        __m128 b_mul_green = _mm_mul_ps(_mm_load_ps(green + i), packed_b_div_sum_coeffs);//packed green mul packed b
        __m128 c_mul_blue = _mm_mul_ps(_mm_load_ps(blue + i), packed_c_div_sum_coeffs);//packed blue mul packed c
        __m128 sum_of_previous_three_val = _mm_add_ps(_mm_add_ps(a_mul_red, b_mul_green), c_mul_blue);//get (a*R + b*G + c*B)/(a + b + c)
        _mm_store_ps(greyscale_value_of_pixels_div_by_255 + i, _mm_div_ps(sum_of_previous_three_val, packed_255));//get Q_x_y and stores the Q_x_y to memory
    }
    return greyscale_value_of_pixels_div_by_255;//return greyscale values divided by 255.0 for all pixels
}