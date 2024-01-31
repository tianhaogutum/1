#include "gammacorrect.h"

#define VERSION_NUMBER 3 // suppose we have three versions, should be updated

static _Bool v_set = false;           // is V set?
static int version = 0;               // version number, default is zero, value checked in found_option_V
static _Bool b_set = false;           // is B set?
static int benchmark_number = 100;    // benchmark number, should be updated to default value, value checked in found_option_B
static char *input_file_name = NULL;  // input file name, value checked in parse_options
static _Bool o_set = false;           // is o set?
static char *output_file_name = NULL; // output file name, value checked in check_value
static _Bool coeffs_set = false;      // is coefficient set?
static float a = 0.2126;              // temporary setting, should be initialized as the default a
static float b = 0.7152;              // temporary setting, should be initialized as the default b
static float c = 0.0722;              // temporary setting, should be initialized as the default c, value checked in check_value
static _Bool gamma_set = false;       // is gamma set?
static float _gamma = 1;              // temporary setting, should be initialized as the default gamma, value checked in check_value
static const char *program_path;      // stores the path of the program
// long options' table
static const struct option long_options[] = {
    {"coeffs", required_argument, 0, 256},
    {"gamma", required_argument, 0, 257},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}};
// function signatures
static void parse_options(int argc, char **argv);                                                                                                          // getopt_long()
static void found_option_V(void);                                                                                                                          // behaviour if found option '-V'
static void found_option_B(void);                                                                                                                          // behaviour if found option '-B'
static void found_option_o(void);                                                                                                                          // behaviour if found option '-o'
static void found_option_h(void);                                                                                                                          // behaviour if found option '-h' or '--help'
static void found_option_coeffs(void);                                                                                                                     // behaviour if found option '--coeffs'
static void found_option_gamma(void);                                                                                                                      // behaviour if found option '--gamma'
static void print_help(void);                                                                                                                              // print usage
static void print_usage(void);                                                                                                                             // print help
static void exit_failure_with_errmessage(const char *);                                                                                                    // note that error message must end with newline, this function will log the error to stderr and print usage, and then exit with failure
static void check_values(void);                                                                                                                            // check if all input values are legal
static float parseFloatFromStr(char *str, const char *errmessage);                                                                                         // parse a String into float, handle errors
static int parseIntFromStr(char *str, const char *errmessage);                                                                                             // parse a String into int, handle errors
static void allocate_for_ppm_pgm_seq(size_t *width, size_t *height, uint8_t **img, uint8_t **result);                                                      // allocate space for input file and output file, which used for sequential implementation, V0 & V1
static void gamma_correct_seq(_Bool isDefault);                              // this function takes a pointer to a gamma correction function as parameter, which can be a pointer to gamma_correct or a pointer to gamma_correct_V1                                                                           // use taylor series to compute gamma correction sequentially, greyscale conversion will also be sequential
static void gamma_correct_simd(void); // this function takes a pointer to a gamma correction function as parameter, which can be a pointer to gamma_correct_V2
static _Bool save_output_to_outputfile(size_t width, size_t height, uint8_t *output, FILE * fd);                                                                       // save the output into the given output file
int main(int argc, char **argv)
{
    program_path = argv[0]; // save program path as global
    parse_options(argc, argv);
    printf("version is %d\nbenchmark_number is %d\ngamma is %f\ninput file name is %s\na is %f\nb is %f\nc is %f\n", version, benchmark_number, _gamma, input_file_name, a, b, c); // for testing
    check_values();
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
            exit_failure_with_errmessage("You give a wrong option or you forget to give argument to an option.\nProgram terminated.\n");
        }
    }
    if (optind >= argc) // no input file
    {
        exit_failure_with_errmessage("No input file specified.\nProgram terminated.\n");
    }
    else if (optind == argc - 1) // just one input file
    {
        input_file_name = argv[optind];
    }
    else // to many input files
    {
        exit_failure_with_errmessage("More than one input file is given.\nProgram terminated.\n");
    }
}

static void found_option_V(void)
{
    if (v_set)
    {
        exit_failure_with_errmessage("Option 'V' is already set, please don't set it twice.\nProgram terminated.\n");
    }
    version = parseIntFromStr(optarg, "Option 'V' conversion fails.\nProgram terminated.\n");
    if (version < 0 || version > VERSION_NUMBER - 1)
    {
        exit_failure_with_errmessage("The given version number is not provided, provided versions are 0, 1, 2\nProgram terminated.\n"); // should be updated
    }
    v_set = true;
}

static void found_option_B(void)
{
    if (b_set)
    {
        exit_failure_with_errmessage("Option 'B' is already set, please don't set it twice.\nProgram terminated.\n");
    }
    if (optarg)
    {
        benchmark_number = parseIntFromStr(optarg, "Option 'B' conversion fails.\nProgram terminated.\n");
        if (benchmark_number < 100)
        {                                                                                                              // should be updated to guarantee a sufficient workload
            exit_failure_with_errmessage("The number of repetitions cannot be less than 100.\nProgram terminated.\n"); // should be updated accordingly
        }
    }
    b_set = true;
}

static void found_option_o(void)
{
    if (o_set)
    {
        exit_failure_with_errmessage("Option 'o' is already set, please don't set it twice.\nProgram terminated.\n");
    }
    output_file_name = optarg;
    o_set = true;
}

static void found_option_h(void)
{ // help text will be printed, and then exit normally
    print_help();
    exit(EXIT_SUCCESS);
}

static void found_option_coeffs(void) // move value check into check values
{
    if (coeffs_set)
    {
        exit_failure_with_errmessage("Option 'coeffs' is already set, please don't set it twice.\nProgram terminated.\n");
    }

    char *ptr = strtok(optarg, ",");

    if (ptr != NULL) // can we rewrite the following repeated code?
    {
        a = parseFloatFromStr(ptr, "Option 'coeffs' conversion fails.\nProgram terminated.\n");
        ptr = strtok(NULL, ",");
    }
    else
    {
        exit_failure_with_errmessage("Option 'coeffs' requires at least three argument.\nProgram terminated.\n");
    }

    if (ptr != NULL)
    {
        b = parseFloatFromStr(ptr, "Option 'coeffs' conversion fails.\nProgram terminated.\n");
        ptr = strtok(NULL, ",");
    }
    else
    {
        exit_failure_with_errmessage("Option 'coeffs' requires at least three argument.\nProgram terminated.\n");
    }

    if (ptr != NULL)
    {
        c = parseFloatFromStr(ptr, "Option 'coeffs' conversion fails.\nProgram terminated.\n");
        ptr = strtok(NULL, ",");
    }
    else
    {
        exit_failure_with_errmessage("Option 'coeffs' requires at least three arguments.\nProgram terminated.\n");
    }

    // more than three arguments
    if (ptr != NULL)
    {
        exit_failure_with_errmessage("Option 'coeffs' requires exactly three arguments.\nProgram terminated.\n");
    }

    // a, b, c cannot be negative
    if (a < 0 || b < 0 || c < 0)
    {
        exit_failure_with_errmessage("Coefficients a, b, c cannot be negative.\nProgram terminated.\n");
    }

    coeffs_set = true;
}

static void found_option_gamma(void)
{
    if (gamma_set)
    {
        exit_failure_with_errmessage("Option 'gamma' is already set, please don't set it twice.\nProgram terminated.\n");
    }
    _gamma = parseFloatFromStr(optarg, "Option 'gamma' conversion fails.\nProgram terminated.\n");
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
    exit(EXIT_FAILURE);
}

static float parseFloatFromStr(char *str, const char *errmessage)
{
    char *endptr;
    errno = 0;
    float value = strtof(str, &endptr);
    if (endptr == str || errno == ERANGE)
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
    char *endptr;
    errno = 0;
    int value = strtol(str, &endptr, 10);
    if (endptr == str || errno == ERANGE)
    {
        exit_failure_with_errmessage(errmessage);
    }
    return value;
}

static void check_values(void)
{
    if (!o_set)
    {
        exit_failure_with_errmessage("Option o is mandatory.\nProgram terminated.\n");
    }
    if (a == 0 && b == 0 && c == 0)
    {
        exit_failure_with_errmessage("Coefficients can not be all zeros.\nProgram terminated.\n");
    }
    if (_gamma < 0)
    {
        exit_failure_with_errmessage("Only positive gamma accepted.\nProgram terminated.\n");
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
    readppm_for_simd(input_file_name, width, height, red_in_pixels, green_in_pixels, blue_in_pixels);
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
    allocate_for_ppm_pgm_seq(&width, &height, &input, &output);
    if(isDefault)
    {
        gamma_correct(input, width, height, a, b, c, _gamma, output);
    }
    else
    {
        gamma_correct_V1(input, width, height, a, b, c, _gamma, output);
    }
    FILE * fd = fopen(output_file_name, "w");
    if(!fd){
        free(input);
        free(output);
        fprintf(stderr, "Cannot open output file. Program terminated.\n");
        exit(EXIT_FAILURE);
    }
    if(!save_output_to_outputfile(width, height, output, fd)){
        free(input);
        free(output);
        fclose(fd);
        fprintf(stderr, "Failed to write into output file. Program terminated.\n");
        exit(EXIT_FAILURE);
    }
    if (b_set)
    {
        struct timespec start;
        clock_gettime(CLOCK_MONOTONIC, &start);
        for (int i = 0; i < benchmark_number; ++i)
        {
            gamma_correct(input, width, height, a, b, c, _gamma, output);
        }
        struct timespec end;
        clock_gettime(CLOCK_MONOTONIC, &end);
        double time = end.tv_sec - start.tv_sec + 1e-9 *(end.tv_nsec - start.tv_nsec);
        printf("This execution takes %lf.\n", time);
    }
    free(input);
    free(output);//free output buffer here
    fclose(fd);
}

static void gamma_correct_simd(void)
{
    size_t width, height;
    float * red_in_pixels = NULL;
    float * green_in_pixels = NULL;
    float * blue_in_pixels = NULL;
    uint8_t *output = NULL;
    allocate_for_ppm_pgm_simd(&width, &height, &red_in_pixels, &green_in_pixels, &blue_in_pixels, &output);
    gamma_correct_V2(red_in_pixels, green_in_pixels, blue_in_pixels, width, height, a, b, c, _gamma, output);
    FILE * fd = fopen(output_file_name, "w");
    if(!fd){
        free(red_in_pixels);
        free(green_in_pixels);
        free(blue_in_pixels);
        free(output);
        fprintf(stderr, "Cannot open output file. Program terminated.\n");
        exit(EXIT_FAILURE);
    }
    if(!save_output_to_outputfile(width, height, output, fd)){
        free(red_in_pixels);
        free(green_in_pixels);
        free(blue_in_pixels);
        free(output);
        fclose(fd);
        fprintf(stderr, "Failed to write into output file. Program terminated.\n");
        exit(EXIT_FAILURE);
    }
    if(b_set){
        struct timespec start;
        clock_gettime(CLOCK_MONOTONIC, &start);
        for(int i = 0; i < benchmark_number; ++i){
            gamma_correct(red_in_pixels, green_in_pixels, blue_in_pixels, width, height, a, b, c, _gamma, output);
        }
        struct timespec end;
        clock_gettime(CLOCK_MONOTONIC, &end);
        double time = end.tv_sec - start.tv_sec + 1e-9 *(end.tv_nsec - start.tv_nsec);
        printf("This execution takes %lf.\n", time);
    }
    free(red_in_pixels);
    free(green_in_pixels);
    free(blue_in_pixels);
    free(output);//free output buffer here
    fclose(fd);
}

static _Bool save_output_to_outputfile(size_t width, size_t height, uint8_t *output, FILE * fd){//fd has already been checked in the caller function
    fprintf(fd, "P5\n%lu\n%lu\n255\n", width, height);
    if(fwrite(output, width * height, 1, fd) != 1){//write result into output file and check if succeeded
        return false;
    }
    return true;
}