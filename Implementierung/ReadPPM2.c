#include<stdio.h>
#include<stdlib.h>
#include <stdint.h>
#include <math.h>


typedef struct {
     int width, height;
     uint8_t *data;
} PPMImage;

#define RGB_COMPONENT_COLOR 255

static PPMImage* readPPM(const char *filename)
{
         char buff[16];
         PPMImage *img;
         FILE *fp;
         int c, rgb_comp_color;

         //open PPM file for reading
         fp = fopen(filename, "rb");
         if (!fp) {
              fprintf(stderr, "Unable to open file '%s'\n", filename);
              exit(1);
         }

         //read image format
         if (!fgets(buff, sizeof(buff), fp)) {
              perror(filename);
              exit(1);
         }

    //check the image format
    if (buff[0] != 'P' || buff[1] != '6') {
         fprintf(stderr, "Invalid image format (must be 'P6')\n");
         exit(1);
    }

    //alloc memory from image
    img = (PPMImage *) malloc(sizeof(PPMImage));
    if (!img) {
         fprintf(stderr, "Unable to allocate memory\n");
         exit(1);
    }

    //check for comments
    c = getc(fp);
    while (c == '#') {
    while (getc(fp) != '\n') ;
         c = getc(fp);
    }
    ungetc(c, fp);


    //read image size information
    if (fscanf(fp, "%d %d", &img->width, &img->height) != 2) {
         fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
         exit(1);
    }

    //read rgb component
    if (fscanf(fp, "%d", &rgb_comp_color) != 1) {
         fprintf(stderr, "Invalid rgb component (error loading '%s')\n", filename);
         exit(1);
    }

    //check rgb component depth
    if (rgb_comp_color!= RGB_COMPONENT_COLOR) {
         fprintf(stderr, "'%s' does not have 8-bits components\n", filename);
         exit(1);
    }

    while (fgetc(fp) != '\n') ;

    //memory allocation for pixel data
    img->data = (uint8_t*) malloc(img->width * img->height * 3);

    if (!img) {
         fprintf(stderr, "Unable to allocate memory\n");
         exit(1);
    }

     
    //read pixel data from file
    if (fread(img->data, 1, 3 * img->width * img->height, fp) != 3 * img->width * img->height) {
         fprintf(stderr, "Error loading image '%s'\n", filename);
         exit(1);
    }

    fclose(fp);
    return img;
}


void createPGMfile(uint8_t* ptr, size_t width, size_t height, size_t maxval){
     //create new file

     FILE* outputFile = fopen("myfile.pgm","w");
     fprintf(outputFile,"P5");
     fprintf(outputFile,"\n");
     fprintf(outputFile,"%li %li",width,height);
     fprintf(outputFile,"\n");     
     fprintf(outputFile,"%li",maxval);
     fprintf(outputFile,"\n");  

     for(int y =0;y<height;y++){
          for(int x = 0; x<width;x++){
               fprintf(outputFile,"%i ",*ptr++);
          }
          fprintf(outputFile,"\n");  
     }
}


int main(){
    PPMImage *image;
    image = readPPM("sample_640×426.ppm");

    printf("The width is %i and the height %i \n",image->width,image->height);


    uint8_t *pixelptr = image->data;

    for(int i = 0; i<30;i++){
     printf("Color %i: %i \n",i,pixelptr[i]);
    }

    uint8_t* greyPtr = greyMaker(pixelptr,1.,1.,1.,0.5);
    printf("The first gray value is: %i", greyPtr[0]);

    uint8_t ptr[] = {1,2,3,4,5,5,13,5,5,4,6,1,2,5,4,5,2,4,3,1,6,7,8,3,5,6,9,8,3,5,7,4,7,5,2,7};
    createPGMfile(ptr,5,3,15);




}


