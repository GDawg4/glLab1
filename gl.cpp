//
// Created by Rodrigo Garoz on 8/07/2020.
//

#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <math.h>
using namespace std;

class gl {
private:
    static const int bytesPerPixel = 3;
    const int fileHeaderSize = 14;
    const int infoHeaderSize = 40;
    int height;
    int width;
    int viewPortHeight;
    int viewPortWidth;
    int vPortX;
    int vPortY;
    char* imageFileName = "result.bmp";
    double defaultColor[3] = {120, 190, 200};
    double paintColor[3] = {255, 255, 255};
    unsigned char *list;

public:
    gl(int h, int w){
        glCreateWindow(h, w);
    };

    void glCreateWindow(int h, int w){
        height = h;
        width = w;
        viewPortHeight = 10;
        viewPortWidth = 10;
        vPortX = 0;
        vPortY = 0;
        list = new unsigned char[height*width*bytesPerPixel]();
    }

    bool glViewPort(int x, int y, int newWidth, int newHeight){
        if (newWidth >= width && newHeight >= height){
            viewPortWidth = newWidth;
            viewPortHeight = newHeight;
            vPortX = x;
            vPortY = y;
            return true;
        }
        return false;
    }

    void glClear(){
        for(int i=0; i<height; i++){
            for(int j=0; j<width; j++){
                for (int k = 0; k < bytesPerPixel; k++) {
                    list[i*width*bytesPerPixel+j*bytesPerPixel+k] = (unsigned char)defaultColor[2-k];
                }
            }
        }
    }

    void glClearColor(double r, double g, double b){
        defaultColor[0] = r * 255;
        defaultColor[1] = g * 255;
        defaultColor[2] = b * 255;
    };

    bool glVertex(int xToTransform, int yToTransform){
        if ((xToTransform >= -1 && xToTransform<=1) || (yToTransform >= -1 && yToTransform<=1)){
            auto nx=round((xToTransform+1)*(viewPortWidth/2)+vPortX);
            auto ny=round((yToTransform+1)*(viewPortHeight/2)+vPortY);
            glVertexCoord(nx, ny);
            return true;
        }
        return false;
    }

    void glVertexCoord(int xToPaint, int yToPaint){
        for (int k = 0; k < bytesPerPixel; k++){
            list[yToPaint*width*bytesPerPixel+xToPaint*bytesPerPixel+k] = (unsigned char)paintColor[2-k];
        }
    }

    void glColor(double r, double g, double b){
        paintColor[0] = r * 255;
        paintColor[1] = g * 255;
        paintColor[2] = b * 255;
    };

    void glLine(double xStart, double yStart, double xFinish, double yFinish){
        bool isBackwards = xStart > xFinish;

        auto trueXStart = (isBackwards) ? xFinish : xStart;
        auto trueXFinish = (isBackwards) ? xStart : xFinish;
        auto trueYStart = (isBackwards) ? yFinish : yStart;
        auto trueYFinish = (isBackwards) ? yStart : yFinish;

        auto pixelXStart=round((trueXStart+1)*(viewPortWidth/2)+vPortX) < width ? round((trueXStart+1)*(viewPortWidth/2)+vPortX) : round((trueXStart+1)*(viewPortWidth/2)+vPortX) - 1;
        auto pixelYStart=round((trueYStart+1)*(viewPortHeight/2)+vPortY) < height ? round((trueYStart+1)*(viewPortHeight/2)+vPortY) : round((trueYStart+1)*(viewPortHeight/2)+vPortY) - 1;
        auto pixelXFinish=round((trueXFinish+1)*(viewPortWidth/2)+vPortX) < width ? round((trueXFinish+1)*(viewPortWidth/2)+vPortX) : round((trueXFinish+1)*(viewPortWidth/2)+vPortX) - 1;
        auto pixelYFinish=round((trueYFinish+1)*(viewPortHeight/2)+vPortY) < height ? round((trueYFinish+1)*(viewPortHeight/2)+vPortY) : round((trueYFinish+1)*(viewPortHeight/2)+vPortY) - 1;

        std::cout << pixelXStart << '\n';
        std::cout << pixelYStart << '\n';

        std::cout << pixelXFinish << '\n';
        std::cout << pixelYFinish << '\n';

        if (pixelXFinish == pixelXStart){
            for(int i=pixelYStart; i <= pixelYFinish; i++){
                glVertexCoord(pixelXStart, i);
                //std::cout << pixelXFinish << ' ' << i <<'\n';
            }
        } else if (pixelYFinish == pixelYStart){
            for(int i=pixelXStart; i <= pixelXFinish; i++){
                glVertexCoord(i, pixelYStart);
                //std::cout << i << ' ' << pixelYStart <<'\n';
            }
        } else{
            auto dy = (double)(pixelYFinish-pixelYStart);
            auto dx = (double)(pixelXFinish-pixelXStart);
            auto slope = dy/dx;

            std::cout << '\n' << slope <<'\n';

            bool isSteep = (dy > dx);

            double offset = 0;
            double limit = 0.5;

            int currentY = !isSteep ? pixelYStart : pixelXStart;
            int start = !isSteep ? pixelXStart : pixelYStart;
            int finish = !isSteep ? pixelXFinish : pixelYFinish;
            slope = !isSteep ? dy / dx : dx / dy;

            std::cout << '\n' << isSteep << 't' << '\n';
            std::cout << start << 's' << '\n';
            std::cout << finish << 'f' << '\n' << '\n';

            for(int i=start; i <= finish ; i++){
                if (isSteep){
                    glVertexCoord((int)(currentY), i);
                } else{
                    glVertexCoord(i, (int)(currentY));
                }
                std::cout << i << ' ' << (int)(currentY) <<'\n';
                offset += abs(slope);
                if (abs(offset) >= limit){
                    trueYFinish>trueYStart? currentY++ : currentY--;
                    limit += 1;
                }
            }
        }
    }

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
