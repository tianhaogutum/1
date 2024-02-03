#include "gammacorrect.h"

#define VERSION_NUMBER 3 // we have three versions

static _Bool v_set = false;           // is V set?
static int version = 0;               // version number, default is zero, value checked in found_option_V
static _Bool b_set = false;           // is B set?
static int benchmark_number = 1000;   // benchmark number, default is 1000, value checked in found_option_B
static char *input_file_name = NULL;  // input file name, value checked in parse_options
static _Bool o_set = false;           // is o set?
static char *output_file_name = NULL; // output file name, value checked in check_value
static _Bool coeffs_set = false;      // is coefficient set?
static float a = 0.299;               // default a is 0.299
static float b = 0.587;               // default b is 0.587
static float c = 0.114;               // default c is 0.114
static _Bool gamma_set = false;       // is gamma set?
static float _gamma = 1;              // default gamma is 1
static const char *program_path;      // stores the path of the program
// long options' table
static const struct option long_options[] = {
    {"coeffs", required_argument, 0, 256},
    {"gamma", required_argument, 0, 257},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}};
// function signatures
static void parse_options(int, char **);                                                             // getopt_long() to parse options
static void found_option_V(void);                                                                    // behaviour if found option '-V'
static void found_option_B(void);                                                                    // behaviour if found option '-B'
static void found_option_o(void);                                                                    // behaviour if found option '-o'
static void found_option_h(void);                                                                    // behaviour if found option '-h' or '--help'
static void found_option_coeffs(void);                                                               // behaviour if found option '--coeffs'
static void found_option_gamma(void);                                                                // behaviour if found option '--gamma'
static void print_help(void);                                                                        // print help
static void print_usage(void);                                                                       // print usage
static void exit_failure_with_errmessage(const char *);                                              // note that error message must end with newline, this function will log the error to stderr and print usage, and then exit with failure
static void check_values(void);                                                                      // check if all input values are legal
static float parseFloatFromStr(char *, const char *);                                                // parse a String into float, handle errors
static int parseIntFromStr(char *, const char *);                                                    // parse a String into int, handle errors
static void allocate_for_ppm_pgm_seq(size_t *, size_t *, uint8_t **, uint8_t **);                    // allocate space for input file and output file, which used for sequential implementation, V0 & V1
static void allocate_for_ppm_pgm_simd(size_t *, size_t *, float **, float **, float **, uint8_t **); // allocate space for input file and output file, which used for SIMD implementation, V2
static void gamma_correct_seq(_Bool);                                                                // this function takes _Bool, if set true, then gamma_correct will be used, if set false, gamma_correct_V1 will be used
static void gamma_correct_simd(void);                                                                // this function takes no parameter, and gamma_correct_V2 will be used
static _Bool save_output_to_outputfile(size_t, size_t, uint8_t *, FILE *);                           // save the output into the given output file
static void free_for_seq(uint8_t *, uint8_t *);                                                      // if gamma_correct_seq ends or an error occured in function body, then release memory for input and output
static void free_for_simd(float *, float *, float *, uint8_t *);                                     // if gamma_correct_simd ends or an error occured in function body, then release memory for output and input of every color

int main(int argc, char **argv)
{
    program_path = argv[0];                                                                                                                                                        // save program path as global, will be used in print_help and print_usage
    parse_options(argc, argv);                                                                                                                                                     // getopt_long
    check_values();                                                                                                                                                                // check if all values are acceptable
    printf("version is %d\nbenchmark_number is %d\ngamma is %f\ninput file name is %s\na is %f\nb is %f\nc is %f\n", version, benchmark_number, _gamma, input_file_name, a, b, c); // for testing
    switch (version)
    {
    case 0:
        gamma_correct_seq(true);
        break;
    case 1:
        gamma_correct_seq(false);
        break;
    case 2:
        gamma_correct_simd();
        break;
    default:
        fprintf(stderr, "There is a bug with version number, please contact developer.\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}

static void parse_options(int argc, char **argv)
{
    int opt = 0;
    while ((opt = getopt_long(argc, argv, "V:B::o:h", long_options, NULL)) != -1)
    {
        switch (opt)
        {
        case 'V':
            found_option_V();
            break;
        case 'B':
            found_option_B();
            break;
        case 'o':
            found_option_o();
            break;
        case 'h':
            found_option_h();
            break;
        case 256: //--coeffs
            found_option_coeffs();
            break;
        case 257: //--gamma
            found_option_gamma();
            break;
        default: // option argument missing or unknown option
            exit_failure_with_errmessage("You give a wrong option or you forget to give argument to an option.\n");
        }
    }
    if (optind >= argc) // no input file
    {
        exit_failure_with_errmessage("No input file specified.\n");
    }
    else if (optind == argc - 1) // just one input file
    {
        input_file_name = argv[optind];
    }
    else // to many input files
    {
        exit_failure_with_errmessage("More than one input file is given.\n");
    }
}

static void found_option_V(void)
{
    if (v_set)
    {
        exit_failure_with_errmessage("Option 'V' is already set, please don't set it twice.\n");
    }
    version = parseIntFromStr(optarg, "Argument of option 'V' parsing fails.\n"); // parse int from a string, if failed, report the error message
    if (version < 0 || version > VERSION_NUMBER - 1)                              // version number not allowed
    {
        exit_failure_with_errmessage("The given version number is not provided, provided versions are 0, 1, 2\n");
    }
    v_set = true;
}

static void found_option_B(void)
{
    if (b_set)
    {
        exit_failure_with_errmessage("Option 'B' is already set, please don't set it twice.\n");
    }
    if (optarg)
    {
        benchmark_number = parseIntFromStr(optarg, "Argument of option 'B' parsing fails.\n"); // parse int from string, if failed, report the error message
        if (benchmark_number < 1000)
        {                                                                                          // default benchmark number is 1000, to guarantee a sufficient workload
            exit_failure_with_errmessage("The number of repetitions cannot be less than 1000.\n"); // the benchmark number set by the user should be no less than the default value
        }
    }
    b_set = true;
}

static void found_option_o(void)
{
    if (o_set)
    {
        exit_failure_with_errmessage("Option 'o' is already set, please don't set it twice.\n");
    }
    output_file_name = optarg; // save output file name
    o_set = true;
}

static void found_option_h(void)
{ // help text will be printed, and then exit normally
    print_help();
    exit(EXIT_SUCCESS);
}

static void found_option_coeffs(void)
{
    if (coeffs_set)
    {
        exit_failure_with_errmessage("Option 'coeffs' is already set, please don't set it twice.\n");
    }
    char *ptr = strtok(optarg, ","); // cut off a from optarg
    if (ptr)
    {
        a = parseFloatFromStr(ptr, "Argument a of option 'coeffs' parsing fails.\n");
        ptr = strtok(NULL, ","); // cut off b from optarg
    }
    else
    {
        exit_failure_with_errmessage("Option 'coeffs' requires at least three arguments.\n");
    }
    if (ptr)
    {
        b = parseFloatFromStr(ptr, "Argument b of option 'coeffs' parsing fails.\n");
        ptr = strtok(NULL, "\x00"); // take the remaining string as c
    }
    else
    {
        exit_failure_with_errmessage("Option 'coeffs' requires at least three argument.\n");
    }
    if (ptr)
    {
        c = parseFloatFromStr(ptr, "Argument c of option 'coeffs' parsing fails.\n");
    }
    else
    {
        exit_failure_with_errmessage("Option 'coeffs' requires at least three arguments.\n");
    }
    coeffs_set = true;
}

static void found_option_gamma(void)
{
    if (gamma_set)
    {
        exit_failure_with_errmessage("Option 'gamma' is already set, please don't set it twice.\n");
    }
    _gamma = parseFloatFromStr(optarg, "Argument of option 'gamma' parsing fails.\n");
    gamma_set = true;
}

static void print_help(void)
{
    printf(help_msg, program_path);
}

static void print_usage(void)
{
    printf(usage_msg, program_path, program_path, program_path);
}

static void exit_failure_with_errmessage(const char *errmessage)
{
    fprintf(stderr, "%s", errmessage);
    print_usage();
    printf("Program terminated.\n");
    exit(EXIT_FAILURE);
}

static float parseFloatFromStr(char *str, const char *errmessage)
{
    char *endptr; // store the position of the first character which is not used for conversion
    errno = 0;    // clear the previous errno
    float value = strtof(str, &endptr);
    if (endptr == str || errno == ERANGE || (*endptr) != 0) // no conversion happend || given number causes overflow or underflow || unused characters at the end of str, which means there could be a poisonous input
    {
        exit_failure_with_errmessage(errmessage);
    }
    if (__builtin_isnan(value) || __builtin_isinf(value))
    {
        exit_failure_with_errmessage("The given argument in float can not be NaN or inf.\n");
    }
    return value;
}

static int parseIntFromStr(char *str, const char *errmessage)
{
    char *endptr; // stores the position of the first character which is not used for conversion
    errno = 0;    // clear previous errno
    int value = strtol(str, &endptr, 10);
    if (endptr == str || errno == ERANGE || (*endptr) != 0) // no conversion happend || given number causes overflow or underflow || unused characters at the end of str, which means there could be a poisonous input
    {
        exit_failure_with_errmessage(errmessage);
    }
    return value;
}

static void check_values(void)
{
    if (!o_set) // output file has to be set
    {
        exit_failure_with_errmessage("Option o is mandatory.\n");
    }
    // a, b, c cannot be negative
    if (a < 0 || b < 0 || c < 0)
    {
        exit_failure_with_errmessage("Coefficients a, b, c cannot be negative.\n");
    }
    // a, b, c cannot be all zeros
    if (a == 0 && b == 0 && c == 0)
    {
        exit_failure_with_errmessage("Coefficients can not be all zeros.\n");
    }
    // sum of a, b, c is too big
    if (__builtin_isinf(a + b + c))
    {
        exit_failure_with_errmessage("The total amount of coefficients exeeds the max limit of float.\n");
    }
    if (_gamma < 0)
    {
        exit_failure_with_errmessage("Only non negative gamma accepted.\n");
    }
}

static void allocate_for_ppm_pgm_seq(size_t *width, size_t *height, uint8_t **img, uint8_t **result)
{
    *img = readppm_for_seq(input_file_name, width, height); // we don't need to check the return value here, because if allocation error occured, the program will terminate in readppm_for_seq. And until now there is no ram/fd to release.
    *result = malloc((*width) * (*height));
    if (!(*result)) // if memory allocation failed, then release all resources
    {
        free(*img);
        fprintf(stderr, "%s", "memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
}

static void allocate_for_ppm_pgm_simd(size_t *width, size_t *height, float **red_in_pixels, float **green_in_pixels, float **blue_in_pixels, uint8_t **result)
{
    readppm_for_simd(input_file_name, width, height, red_in_pixels, green_in_pixels, blue_in_pixels); // we don't need to check the return value here, because if allocation error occured, the program will terminate in readppm_for_seq. And until now there is no ram/fd to release.
    *result = malloc((*width) * (*height));
    if (!(*result)) // if memory allocation failed, then release all resources
    {
        free(*(red_in_pixels));
        free(*(green_in_pixels));
        free(*(blue_in_pixels));
        fprintf(stderr, "%s", "memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
}

static void gamma_correct_seq(_Bool isDefault)
{
    size_t width, height;
    uint8_t *input = NULL;
    uint8_t *output = NULL;
    allocate_for_ppm_pgm_seq(&width, &height, &input, &output); // read ppm file and allocate space for input and output data, read metadata
    if (isDefault)                                              // the default version should be used?
    {
        gamma_correct(input, width, height, a, b, c, _gamma, output);
    }
    else
    {
        gamma_correct_V1(input, width, height, a, b, c, _gamma, output);
    }
    FILE *fd = fopen(output_file_name, "w"); // open output file
    if (!fd)                                 // check if fopen succeeded
    {
        free_for_seq(input, output);
        fprintf(stderr, "Cannot open output file. Program terminated.\n");
        exit(EXIT_FAILURE);
    }
    if (!save_output_to_outputfile(width, height, output, fd)) // check if write into output file succeeded
    {
        free_for_seq(input, output);
        fclose(fd);
        fprintf(stderr, "Failed to write into output file. Program terminated.\n");
        exit(EXIT_FAILURE);
    }
    if (b_set) // user sets option B for benchmarking?
    {
        struct timespec start;
        clock_gettime(CLOCK_MONOTONIC, &start);
        for (int i = 0; i < benchmark_number; ++i)
        {
            if (isDefault) // choose the version from V0 and V1
            {
                gamma_correct(input, width, height, a, b, c, _gamma, output);
            }
            else
            {
                gamma_correct_V1(input, width, height, a, b, c, _gamma, output);
            }
        }
        struct timespec end;
        clock_gettime(CLOCK_MONOTONIC, &end);
        double time = end.tv_sec - start.tv_sec + 1e-9 * (end.tv_nsec - start.tv_nsec);
        printf("This execution takes %lfs.\n", time);
    }
    free_for_seq(input, output);
    fclose(fd); // free all
}

static void gamma_correct_simd(void)
{
    size_t width, height;
    float *red_in_pixels = NULL;
    float *green_in_pixels = NULL;
    float *blue_in_pixels = NULL;
    uint8_t *output = NULL;
    allocate_for_ppm_pgm_simd(&width, &height, &red_in_pixels, &green_in_pixels, &blue_in_pixels, &output); // allocate space for R, G and B and output, read metadata from input ppm
    gamma_correct_V2(red_in_pixels, green_in_pixels, blue_in_pixels, width, height, a, b, c, _gamma, output);
    FILE *fd = fopen(output_file_name, "w"); // open output file
    if (!fd)                                 // check if fopen succeeded
    {
        free_for_simd(red_in_pixels, green_in_pixels, blue_in_pixels, output);
        fprintf(stderr, "Cannot open output file. Program terminated.\n");
        exit(EXIT_FAILURE);
    }
    if (!save_output_to_outputfile(width, height, output, fd)) // check if writing into output file succeeded
    {
        free_for_simd(red_in_pixels, green_in_pixels, blue_in_pixels, output);
        fclose(fd);
        fprintf(stderr, "Failed to write into output file. Program terminated.\n");
        exit(EXIT_FAILURE);
    }
    if (b_set) // user sets option B for benchmarking?
    {
        struct timespec start;
        clock_gettime(CLOCK_MONOTONIC, &start);
        for (int i = 0; i < benchmark_number; ++i)
        {
            gamma_correct_V2(red_in_pixels, green_in_pixels, blue_in_pixels, width, height, a, b, c, _gamma, output);
        }
        struct timespec end;
        clock_gettime(CLOCK_MONOTONIC, &end);
        double time = end.tv_sec - start.tv_sec + 1e-9 * (end.tv_nsec - start.tv_nsec);
        printf("This execution takes %lfs.\n", time);
    }
    free_for_simd(red_in_pixels, green_in_pixels, blue_in_pixels, output);
    fclose(fd); // free all
}

static _Bool save_output_to_outputfile(size_t width, size_t height, uint8_t *output, FILE *fd)
{ // fd has already been checked in the caller function, so it couldn't be NULL
    if (fprintf(fd, "P5\n%lu\n%lu\n255\n", width, height) < 0)
    { // if fprintf failed
        return false;
    }
    if (fwrite(output, width * height, 1, fd) != 1)
    { // write result into output file and check if succeeded
        return false;
    }
    return true;
}

static void free_for_seq(uint8_t *input, uint8_t *output)
{
    free(input);
    free(output);
}

static void free_for_simd(float *red_in_pixels, float *green_in_pixels, float *blue_in_pixels, uint8_t *output)
{
    free(red_in_pixels);
    free(green_in_pixels);
    free(blue_in_pixels);
    free(output);
}