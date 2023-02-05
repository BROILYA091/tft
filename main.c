// Tft Touch Shield V1.0 demo - Display BMP file

//#include <SD.h>
#include <wiringPi.h>
#include <stdio.h>
#include "tft.h"

typedef enum {false = 0,true} boolean;

void bmpdraw(FILE *f, int x, int y);
boolean bmpReadHeader(FILE *f);
unsigned int read16(FILE *f);
unsigned long read32(FILE *f);

char str[100]; // desperdicio de RAM

// In the SD card, place 24 bit color BMP files (be sure they are 24-bit!)
// the file itself
FILE *bmpFile;

// information we extract about the bitmap file
int bmpWidth, bmpHeight;
unsigned char bmpDepth, bmpImageoffset;

int main( int argc, const char* argv[] ) 
{

    ResetTFT();
    ILI9325_CPT28_Initial();  // Dentro de esta fx inicializar wiringPi
    setOrientation(0);

    
    sprintf(str,"/home/bananapi/Desktop/files/%s.bmp",argv[1]);
    
    //Image 1
    bmpFile = fopen(str, "r");
    if (! bmpFile) {
        printf("didnt find image\r\n");
        while (1);
    }

    if (! bmpReadHeader(bmpFile)) {
        printf("bad bmp\r\n");
        return -1;
    }

    printf("image data\r\n");
    
    printf("offset: %u\r\n",bmpImageoffset);
    printf("depth: %u\r\n",bmpDepth);
    printf("Height: %u\r\n",bmpHeight);
    printf("Width: %u\r\n",bmpWidth);
    

    bmpdraw(bmpFile, 0, 0);
    delay(3000);
    fclose(bmpFile);
    
    
    return 0;
}


/*********************************************/
// This procedure reads a bitmap and draws it to the screen
// its sped up by reading many pixels worth of data at a time
// instead of just one pixel at a time. increading the buffer takes
// more RAM but makes the drawing a little faster. 20 pixels' worth
// is probably a good place

#define BUFFPIXEL 20

void bmpdraw(FILE *f, int x, int y) {
    int whence=0;

    fseek(bmpFile,bmpImageoffset,whence);

    unsigned int p;
    unsigned char g, b;
    int i, j;

    unsigned char sdbuffer[3 * BUFFPIXEL];  // 3 * pixels to buffer
    unsigned char buffidx = 3*BUFFPIXEL;

 /*   for (i=0; i< bmpHeight; i++) {   // Vertical

        setXY(i, x);

        for (j=0; j<bmpWidth; j++) {*/

   
    for (i=0; i< bmpHeight; i++) {     // Horizontal

        setXY(x, y+bmpHeight-i);

        for (j=0; j<bmpWidth; j++) {
            // read more pixels
            if (buffidx >= 3*BUFFPIXEL) {
                fread(sdbuffer, 1, 3*BUFFPIXEL, bmpFile);
                buffidx = 0;
            }

            // convert pixel from 888 to 565
            b = sdbuffer[buffidx++];     // blue
            g = sdbuffer[buffidx++];     // green
            p = sdbuffer[buffidx++];     // red

            p >>= 3;
            p <<= 6;

            g >>= 2;
            p |= g;
            p <<= 5;

            b >>= 3;
            p |= b;
            
            // write out the 16 bits of color
            sendData(p);
        }
    }

}

boolean bmpReadHeader(FILE *f) {
    // read header
    //unsigned long tmp;

    if (read16(f) != 0x4D42) {
        // magic bytes missing
        return false;
    }

    // read file size
   /*tmp = */read32(f);

    // read and ignore creator bytes
    read32(f);

    bmpImageoffset = read32(f);

    // read DIB header
   /* tmp = */read32(f);

    bmpWidth = read32(f);
    bmpHeight = read32(f);


    if (read16(f) != 1)
    return false;

    bmpDepth = read16(f);

    if (read32(f) != 0) {
        // compression not supported!
        return false;
    }


    return true;
}

/*********************************************/
// These read data from the SD card file and convert them to big endian
// (the data is stored in little endian format!)

// LITTLE ENDIAN!
unsigned int read16(FILE *f) {
	
    unsigned int d;
    unsigned char b;
    unsigned char buf[3];
    
    fread(buf, 1, 2, bmpFile);
    
    b = buf[0];
    d = buf[1];
    d <<= 8;
    d |= b;
    return d;
	
}

// LITTLE ENDIAN!
unsigned long read32(FILE *f) {
	
    unsigned long d;
    unsigned int b;

    b = read16(f);
    d = read16(f);
    d <<= 16;
    d |= b;
    return d;
}



