//
// Created by Rodrigo Garoz on 8/07/2020.
//

#include <stdio.h>
#include <iostream>
#include <string>
#include "gl.h"
#include <vector>
using namespace std;

class gl {
public:
    static const int bytesPerPixel = 3;
    const int fileHeaderSize = 14;
    const int infoHeaderSize = 40;
    int height;
    int width;
    int viewPortHeight;
    int viewPortWidth;
    int vPortX;
    int vPortY;
    char* imageFileName = "bitmapImage.bmp";
    double defaultColor[3] = {120, 190, 200};
    double paintColor[3] = {255, 255, 255};
    unsigned char *list;

    gl(int h, int w){
        height = h;
        width = w;
        glClear();
    };

    void glViewPort(int x, int y, int newWidth, int newHeight){
        viewPortWidth = newWidth;
        viewPortHeight = newHeight;
        vPortX = x;
        vPortY = y;
    }

    void glVertex(int xToTransform, int yToTransform){
        auto nx=int((xToTransform+1)*(viewPortWidth/2)+vPortX);
        auto ny=int((yToTransform+1)*(viewPortHeight/2)+vPortY);
        for (int k = 0; k < 3; k++){
            *(list + nx*width*3 + ny*3 + k) = (unsigned char)paintColor[2-k];
        }
    }

    void glClear(){
        for (int i = 0; i < height; i++){
            for (int j = 0; j < width; j++){
                for (int k = 0; k < 3; k++){
                    *(list + i*width*3 + j*3 + k) = (unsigned char)defaultColor[2-k];
                }
            }
        }
    }

    void glClearColor(double r, double g, double b){
        defaultColor[0] = r * 255;
        defaultColor[1] = g * 255;
        defaultColor[2] = b * 255;
    };

    void glColor(double r, double g, double b){
        paintColor[0] = r * 255;
        paintColor[1] = g * 255;
        paintColor[2] = b * 255;
    };

    void glFinish(){
        generateBitmapImage((unsigned char *)list, height, width, imageFileName);
    }

    void generateBitmapImage(unsigned char *image, int height, int width, char* imageFileName){

        unsigned char padding[3] = {0, 0, 0};
        int paddingSize = (4 - (width*bytesPerPixel) % 4) % 4;

        unsigned char* fileHeader = createBitmapFileHeader(height, width, paddingSize);
        unsigned char* infoHeader = createBitmapInfoHeader(height, width);

        FILE* imageFile = fopen(imageFileName, "wb");

        fwrite(fileHeader, 1, fileHeaderSize, imageFile);
        fwrite(infoHeader, 1, infoHeaderSize, imageFile);

        int i;
        for(i=0; i<height; i++){
            fwrite(image+(i*width*bytesPerPixel), bytesPerPixel, width, imageFile);
            fwrite(padding, 1, paddingSize, imageFile);
        }

        fclose(imageFile);
    }

    unsigned char* createBitmapFileHeader(int height, int width, int paddingSize){
        int fileSize = fileHeaderSize + infoHeaderSize + (bytesPerPixel*width+paddingSize) * height;

        static unsigned char fileHeader[] = {
                0,0, /// signature
                0,0,0,0, /// image file size in bytes
                0,0,0,0, /// reserved
                0,0,0,0, /// start of pixel array
        };

        fileHeader[ 0] = (unsigned char)('B');
        fileHeader[ 1] = (unsigned char)('M');
        fileHeader[ 2] = (unsigned char)(fileSize    );
        fileHeader[ 3] = (unsigned char)(fileSize>> 8);
        fileHeader[ 4] = (unsigned char)(fileSize>>16);
        fileHeader[ 5] = (unsigned char)(fileSize>>24);
        fileHeader[10] = (unsigned char)(fileHeaderSize + infoHeaderSize);

        return fileHeader;
    }

    unsigned char* createBitmapInfoHeader(int height, int width){
        static unsigned char infoHeader[] = {
                0,0,0,0, /// header size
                0,0,0,0, /// image width
                0,0,0,0, /// image height
                0,0, /// number of defaultColor planes
                0,0, /// bits per pixel
                0,0,0,0, /// compression
                0,0,0,0, /// image size
                0,0,0,0, /// horizontal resolution
                0,0,0,0, /// vertical resolution
                0,0,0,0, /// colors in defaultColor table
                0,0,0,0, /// important defaultColor count
        };

        infoHeader[ 0] = (unsigned char)(infoHeaderSize);
        infoHeader[ 4] = (unsigned char)(width    );
        infoHeader[ 5] = (unsigned char)(width>> 8);
        infoHeader[ 6] = (unsigned char)(width>>16);
        infoHeader[ 7] = (unsigned char)(width>>24);
        infoHeader[ 8] = (unsigned char)(height    );
        infoHeader[ 9] = (unsigned char)(height>> 8);
        infoHeader[10] = (unsigned char)(height>>16);
        infoHeader[11] = (unsigned char)(height>>24);
        infoHeader[12] = (unsigned char)(1);
        infoHeader[14] = (unsigned char)(bytesPerPixel*8);

        return infoHeader;
    }

};
