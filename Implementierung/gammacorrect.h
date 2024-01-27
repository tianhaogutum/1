#include <getopt.h>
#include <string.h>
#include <errno.h>
#include "V0.h"
#include "readppm.h"
#include "V1.h"
#include "V2.h"
#include <time.h>

#ifndef GAMMACORRECT_H
#define GAMMACORRECT_H

static const char *usage_msg =
    "Usage: %s [options] -o outputfile inputfile     Compute gamma correction for the inputfile and save the result into outputfile.\n"
    "   or: %s -h                                    Print help and exit.\n"
    "   or: %s --help                                Print help and exit.\n"
    "Attention: Each option is only allowed to set once.\n";

static const char *help_msg = // should be updated at V B coeffs
    "Usage: %s [options] inputfile...\n"
    "Options:\n"
    "  -V<int>                            Optional. Choose a software version. Default is 0.\n"
    "  -B<int>                            Optional. Choose how many times the function call will be repeated. Default is 100.\n"
    "  Inputfile                          Specify the name of the input file.\n"
    "  -o<string>                         Specify the name of the output file.\n"
    "  --coeffs<float>,<float>,<float>    Optional. Set the coefficients for the grey value conversion.\n"
    "  --gamma<float>                     Optional. Set gamma for gamma correction.\n"
    "  -h|--help                          Print help and exit.\n";

#endif