#include "V2.h"

static float *packed_compute_greyscale(const float *, const float *, const float *, size_t, size_t, float, float, float);

void gamma_correct_V2(float *red, float *green, float *blue, size_t width, size_t height, float a, float b, float c, float gamma, uint8_t *result)
{
    float *greyscale_value_of_pixels = packed_compute_greyscale(red, green, blue, width, height, a, b, c);
    if(!greyscale_value_of_pixels){
        free(red);
        free(green);
        free(blue);
        free(result);
        fprintf(stderr, "Cannot allocate space for grey scale values. Program terminated.\n");
        exit(EXIT_FAILURE);
    }
    size_t number_of_pixels = width * height;
    for (size_t i = 0; i < number_of_pixels; ++i)
    {
        result[i] = pow(greyscale_value_of_pixels[i], gamma) * 255;
    }
    free(greyscale_value_of_pixels);
}

static float *packed_compute_greyscale(const float *red, const float *green, const float *blue, size_t width, size_t height, float a, float b, float c)
{
    __m128 packed_a = _mm_set1_ps(a);
    __m128 packed_b = _mm_set1_ps(b);
    __m128 packed_c = _mm_set1_ps(c);
    __m128 packed_sum_abc = _mm_set1_ps(a + b + c);
    __m128 packed_255 = _mm_set1_ps(255.0);
    size_t number_of_pixels = width * height;
    size_t color_buffer_size = (((number_of_pixels) << 2) & 0xfffffffffffffff0) + 16;
    float *greyscale_value_of_pixels_div_by_255 = aligned_alloc(16, color_buffer_size);
    if(!greyscale_value_of_pixels_div_by_255){//check if allocation succeeded
        return NULL;
    }
    for (size_t i = 0; i < number_of_pixels; i += 4)
    {
        __m128 a_mul_red = _mm_mul_ps(_mm_load_ps(red + i), packed_a);
        __m128 b_mul_green = _mm_mul_ps(_mm_load_ps(green + i), packed_b);
        __m128 c_mul_blue = _mm_mul_ps(_mm_load_ps(blue + i), packed_c);
        __m128 sum_of_previous_three_val = _mm_add_ps(_mm_add_ps(a_mul_red, b_mul_green), c_mul_blue);
        _mm_store_ps(greyscale_value_of_pixels_div_by_255 + i, _mm_div_ps(_mm_div_ps(sum_of_previous_three_val, packed_sum_abc), packed_255));
    }
    return greyscale_value_of_pixels_div_by_255;
}