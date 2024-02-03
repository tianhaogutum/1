#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "V0.h"
#include "readppm.h"
#include "V1.h"
#include "V2.h"

#ifndef GAMMACORRECT_H
#define GAMMACORRECT_H

static const char *usage_msg =
    "Usage: %s [options] -o outputfile inputfile     Compute gamma correction for the inputfile and save the result into output file.\n"
    "   or: %s -h                                    Print help and exit.\n"
    "   or: %s --help                                Print help and exit.\n"
    "Attention: Each option is only allowed to set once.\n";

static const char *help_msg = // should be updated at V B coeffs
    "Usage: %s [options] inputfile...\n"
    "Options:\n"
    "  -V<int>                            Optional. Choose a version.\n"
    "  -B<int>                            Optional. Choose how many times the function call will be repeated.\n"
    "  Inputfile                          Specify the name of the input file.\n"
    "  -o<string>                         Specify the name of the output file.\n"
    "  --coeffs<float>,<float>,<float>    Optional. Set the coefficients for the grey value conversion.\n"
    "  --gamma<float>                     Optional. Set gamma for gamma correction.\n"
    "  -h|--help                          Print help and exit.\n"
    "\n"
    "This program takes a 24bpp ppm file as input and then convert it after greyscale conversion and gamma correction to a pgm file. The defualt coefficients for greyscale conversion are 0.299 for R, 0.587 for G, 0.114 for B. The default gamma for gamma correction is 1. With option V you can choose a version number from 0, 1, and 2. 0 is the default version number. If you want to benchmark this program, set option B. The default benchmark number is 1000. You can replace this number with an integer no less than 1000.\n";

#endif