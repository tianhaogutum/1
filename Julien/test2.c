#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

bool v_set = false;
bool b_set = false; 
bool o_set = false; 
bool h_set = false; 

int version;
int optimal;
char* outputFile;
char* inputFile;

void parser(int argc, char** argv, struct option * array){
  int index;
  int tmp;
  int version;
  opterr = 0;
  int testing;


  //Flags
  int Vflag =0;
  int Bflag =0;
  int oflag =0;
  int hflag =0;

  while ((tmp = getopt (argc, argv, "V:B:o:h:")) != -1)
    switch (tmp)
      {
      case 'V':
        if(optarg != NULL){
            v_set = true;
            version = (int) optarg;
        } else {
            //error neeed option argument
        }
        break;
      case 'B':
        if(optarg != NULL){
            b_set = true;
            optimal = (int) optarg;
        } else {
            //error neeed option argument
        }
        break;
      case 'o':
        if(optarg != NULL){
            o_set = true;
            outputFile = optarg;
        } else {
            //error neeed option argument
        }
        break;
      case 'h':
        //explain the stuff
        break;
      case '?':
        if (optopt == 'V'){
            //default
        }
        else if (optopt == 'B'){
            //default
        }
        else if (optopt == 'o') {
            fprintf (stderr, "Dateiname of output must be given\n");
        }
      default:
        abort ();
      }

    //Dateiname must be given!
    if(index==optind){
        fprintf (stderr, "Dateiname of input must be given\n");
    }

  for (index = optind; index < argc; index++){
    printf ("Non-option argument %s\n", argv[index]);
    return 0;
  }

}

int main (int argc, char **argv) {


}
