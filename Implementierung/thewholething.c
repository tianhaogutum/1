#include<stdio.h>
#include<stdlib.h>
#include <stdint.h>
#include <math.h>
#include <nmmintrin.h>

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
         fprintf(stderr, "Invalid image format (must be 'P3')\n");
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

uint8_t* convertPPMptrToPGMptr(int numberPixels, uint8_t* ppmPtr ){
     uint8_t* pgmptr = malloc(numberPixels);

    for(int i = 0;i<numberPixels;i++){
          int acc = 0;

          acc = acc + *ppmPtr++;
          acc = acc + *ppmPtr++;
          acc = acc + *ppmPtr++;

          acc = (int) acc/3;

          pgmptr[i] = (uint8_t) acc;
    }
    return pgmptr;
}


PPMImage* convertPPMtoPGMsimple(PPMImage* inputImg){

      int width = inputImg->width;
      int height = inputImg->height;
      int numberPixels = height*width;

      uint8_t *pixelptr = inputImg->data;
      printf("The width is %i and the height %i \n",inputImg->width,inputImg->height);

    uint8_t* greyptr;

    greyptr= convertPPMptrToPGMptr(numberPixels,pixelptr);

    PPMImage outputImg = { width, height, greyptr};

    PPMImage* tmp = &outputImg;

    return  tmp;
}

float* rearangeData(int n, uint8_t *inputPtr){

    float* floatPtr = malloc(4*n);
    int rest = n % 12;
    int zyklen = (n - n % 12) / 12;

    for (int i = 0; i < zyklen; i++)
    {
        int offset = i * 12;
        floatPtr[offset + 0] = inputPtr[offset + 0];
        floatPtr[offset + 1] = inputPtr[offset + 3];
        floatPtr[offset + 2] = inputPtr[offset + 6];
        floatPtr[offset + 3] = inputPtr[offset + 9];
        floatPtr[offset + 4] = inputPtr[offset + 1];
        floatPtr[offset + 5] = inputPtr[offset + 4];
        floatPtr[offset + 6] = inputPtr[offset + 7];
        floatPtr[offset + 7] = inputPtr[offset + 10];
        floatPtr[offset + 8] = inputPtr[offset + 2];
        floatPtr[offset + 9] = inputPtr[offset + 5];
        floatPtr[offset + 10] = inputPtr[offset + 8];
        floatPtr[offset + 11] = inputPtr[offset + 11];
    }

    return floatPtr;
}

float* scalarOfFloats(size_t n, float* sortedPixels)
{

//Alignment!!!!

     float* baseptr = malloc(4*n);

     float* greyPixels = baseptr;

    float a = 1;
    float b = 1;
    float c = 1;
    float d = 1 / (a + b + c);

    for (size_t i = 0; i < n; i += 4)
    {
        // lade constanten in register
        __m128 va = _mm_setr_ps(a, a, a, a);
        __m128 vb = _mm_setr_ps(b, b, b, b);
        __m128 vc = _mm_setr_ps(c, c, c, c);
        __m128 vd = _mm_setr_ps(d, d, d, d);

        // lade variablen in register
        __m128 vred = _mm_loadu_ps(sortedPixels);
        sortedPixels+=4;
        __m128 vgreen = _mm_loadu_ps(sortedPixels);
        sortedPixels+=4;
        __m128 vblue = _mm_loadu_ps(sortedPixels);
        sortedPixels+=4;

        // multipliziere constanten zu variablen
        vred = _mm_mul_ps(vred, va);
        vgreen = _mm_mul_ps(vgreen, vb);
        vblue = _mm_mul_ps(vblue, vc);

        // adiere register aufeinander
        vgreen = _mm_add_ps(vred, vgreen);
        vblue = _mm_add_ps(vgreen, vblue);

        // Multipliziere (Dividiere) das xmmm register
        vblue = _mm_mul_ps(vblue, vd);

        // lade das ergebnis in einen Pointer
        _mm_store_ps(greyPixels, vblue);
        greyPixels += 4;
    }

    return baseptr;
}

PPMImage*  convertPPMtoPGM_SIMD(PPMImage* image){
      int width = image->width;
      int height = image->height;
      int numberPixels = height*width;

      uint8_t *pixelptr = image->data;

      float* tmpptr;

      tmpptr = rearangeData(3*numberPixels,pixelptr);

      float* greyptr;

      greyptr = scalarOfFloats(3*numberPixels,tmpptr);

     PPMImage imggrey= {width, height, greyptr};

     PPMImage* imgptr = &imggrey;

     //createPGMfile(imgptr, )
}



PPMImage* convertToGrey(int version, PPMImage* image ){
    PPMImage* outputimage;
    if(version ==0) {outputimage = convertPPMtoPGMsimple(image);}
    if(version ==1) {outputimage = convertPPMtoPGM_SIMD(image);}
    //if(version ==2) {outputimage = convertPPMtoPGMSuperSIMD(image);}

    return outputimage;
}

void createPGMfile(  PPMImage* image,  char* outputFileChar){

     uint8_t* ptr = image->data;
      int width = image->width;
      int height = image->height;
      int numberPixels = height*width;
     FILE* outputFile = fopen(outputFileChar,"w");


     fprintf(outputFile,"P2");
     fprintf(outputFile,"\n");
     fprintf(outputFile,"%li %li",image->width,image->height);
     fprintf(outputFile,"\n");     
     fprintf(outputFile,"%li",255);
     fprintf(outputFile,"\n");   

     for(int y =0;y<image->height;y++){
          for(int x = 0; x<image->width;x++){
               fprintf(outputFile,"%i ",*ptr++);
             
          }
          fprintf(outputFile,"\n");  
     }
}

int checkImages(){
    return 1;
}

int checkFiles(){
    return 1;
}
void convertMainFramework( char *inputfile, char *outputfile, int version){
    PPMImage* image;
    PPMImage* imagegrey;

    image = readPPM(inputfile);

    imagegrey = convertToGrey(version,image);

   // if (checkImages());


    createPGMfile(imagegrey, outputfile);

   // if(checkFiles());

    printf("Conversion was a succsess!!!!");
}


void main(){
    convertMainFramework("tree_1.ppm","bigTest2.pgm",0);
}