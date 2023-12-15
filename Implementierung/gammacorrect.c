#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define VERSION_NUMBER 3//suppose we have three versions, should be updated

static _Bool v_set = false;//is V set?
static int version_number = 0;//version number, default is zero
static _Bool b_set = false;//is B set?
static int benchmark_number = -1;//benchmark number
static char * input_file_name = NULL;//input file name
static _Bool o_set = false;//is o set?
static char * output_file_name = NULL;//output file name
static _Bool coeffs_set = false;//is coefficient set?
static float a = 0;//temporary setting, should be initialized as the default a
static float b = 0;//temporary setting, should be initialized as the default b
static float c = 0;//temporary setting, should be initialized as the default c
static _Bool gamma_set = false;//is gamma set?
static _Bool h_set = false;//is h set?

static const struct option long_options[] = {//long options' table
    {"coeffs", required_argument, 0, 256},
    {"gamma", required_argument, 0, 257},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}
};
//function signatures
void parse_options(int argc, char ** argv);//getopt_long()
void found_option_V(void);//behaviour if found option '-V'
void found_option_B(void);//behaviour if found option '-B'
void found_option_o(void);//behaviour if found option '-o'
void found_option_h(void);//behaviour if found option '-h' or '--help'
void found_option_coeffs(void);//behaviour if found option '--coeffs'
void found_option_gamma(void);//behaviour if found option '--gamma'

int main(int argc, char ** argv){
    parse_options(argc, argv);

    return 0;
}

void parse_options(int argc, char ** argv){
    int opt = 0;
    while((opt = getopt_long(argc, argv, "V:B::o:h", long_options, NULL)) != -1){
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
        case 256:
            found_option_coeffs();
            break;
        case 257:
            found_option_gamma();
            break;    
        default:
            break;
        }
    }
    if(optind >= argc){
        fprintf(stderr, "No input file specified.\nProgram terminated.\n");
        exit(EXIT_FAILURE);
    }
    else if(optind == argc - 1){
        input_file_name = argv[optind];
    }
    else{
        fprintf(stderr, "More than one input file is given.\nProgram terminated.\n");
        exit(EXIT_FAILURE);
    }
}

void found_option_V(void){
    if(v_set){
        fprintf(stderr, "Option 'V' is already set, please don't set it twice.\nProgram terminated.\n");
        exit(EXIT_FAILURE);
    }
    int tmp = atoi(optarg);
    if(tmp < 0 || tmp > VERSION_NUMBER - 1){
        fprintf(stderr, "The given version number is not provided, provided versions are 0, 1, 2\nProgram terminated.\n");
        exit(EXIT_FAILURE);
    }
    version_number = tmp;
}

void found_option_B(void){

}

void found_option_o(void){

}

void found_option_h(void){

}

void found_option_coeffs(void){

}

void found_option_gamma(void){

}