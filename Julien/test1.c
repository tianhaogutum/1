#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char **argv) {

  int tmp;
  while ((tmp = getopt (argc, argv, "VBoh")) != -1){
    switch (tmp) 
     {
      case 'V':
        printf("Yay V \n");
        break;
      case 'B':
        printf("Yay B\n");
        break;
      case 'o':
        printf("Yay o\n");
        break;
      case 'h':
        printf("Yay H\n");
        break;
      case '?':

      default:
        0==0;
      }
  }


    for (int index = optind; index < argc; index++){
        printf ("Non-option argument %s\n", argv[index]);
        return 0;
    }

}