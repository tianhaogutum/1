#include "gammacorrect.h"

#define VERSION_NUMBER 3 // suppose we have three versions, should be updated

static _Bool v_set = false;           // is V set?
static int version = 0;               // version number, default is zero
static _Bool b_set = false;           // is B set?
static int benchmark_number = 100;    // benchmark number, should be updated to default value
static char *input_file_name = NULL;  // input file name
static _Bool o_set = false;           // is o set?
static char *output_file_name = NULL; // output file name
static _Bool coeffs_set = false;      // is coefficient set?
static float a = 0;                   // temporary setting, should be initialized as the default a
static float b = 0;                   // temporary setting, should be initialized as the default b
static float c = 0;                   // temporary setting, should be initialized as the default c
static _Bool gamma_set = false;       // is gamma set?
static float gamma = 1;               // temporary setting, should be initialized as the default gamma
static const char *program_path;      // stores the path of the program

static const struct option long_options[] = { // long options' table
    {"coeffs", required_argument, 0, 256},
    {"gamma", required_argument, 0, 257},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}};
// function signatures
static void parse_options(int argc, char **argv); // getopt_long()
static void found_option_V(void);                 // behaviour if found option '-V'
static void found_option_B(void);                 // behaviour if found option '-B'
static void found_option_o(void);                 // behaviour if found option '-o'
static void found_option_h(void);                 // behaviour if found option '-h' or '--help'
static void found_option_coeffs(void);            // behaviour if found option '--coeffs'
static void found_option_gamma(void);             // behaviour if found option '--gamma'
static void print_help(void);
static void print_usage(void);
static void exit_failure_with_errmessage(const char *); // note that error message must end with newline, this function will log the error to stderr and print usage, and then exit with failure

int main(int argc, char **argv)
{
    program_path = argv[0]; // save program path as global
    parse_options(argc, argv);
    printf("version is %d\nbenchmark_number is %d\ngamma is %f\n", version, benchmark_number, gamma);//for testing
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
    version = strtol(optarg, NULL, 10);
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
        benchmark_number = strtol(optarg, NULL, 10);
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

static void found_option_coeffs(void)
{
    if (coeffs_set)
    {
        exit_failure_with_errmessage("Option 'coeffs' is already set, please don't set it twice.\nProgram terminated.\n");
    }

    char* ptr = strtok(optarg, ",");

    if(ptr == NULL){
        exit_failure_with_errmessage("No Value for coeffs was given.\nProgram terminated.\n");
    }
    a = strtof(ptr, NULL);

    ptr = strtok(NULL, ",");
    if(ptr == NULL){
        exit_failure_with_errmessage("No Value for coeffs was given.\nProgram terminated.\n");
    }
    b = strtof(ptr, NULL);

    ptr = strtok(NULL, ",");
    if(ptr == NULL){
        exit_failure_with_errmessage("No Value for coeffs was given.\nProgram terminated.\n");
    }
    c = strtof(ptr, NULL);

    gamma_set = true;
}

static void found_option_gamma(void)
{
    if (gamma_set)
    {
        exit_failure_with_errmessage("Option 'gamma' is already set, please don't set it twice.\nProgram terminated.\n");
    }
    gamma = strtof(optarg, NULL);
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