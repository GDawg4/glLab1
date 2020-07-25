//
// Created by Rodrigo Garoz on 8/07/2020.
//

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <math.h>
#include <sstream>
#include <vector>
#include <typeinfo>
#define PI 3.14159265
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
    double defaultColor[3] = {0, 0, 0};
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

        auto pixelXStart=round((trueXStart+1)*((double)viewPortWidth/2)+vPortX) < width ? round((trueXStart+1)*((double)viewPortWidth/2)+vPortX) : round((trueXStart+1)*((double)(double)viewPortWidth/2)+vPortX) - 1;
        auto pixelYStart=round((trueYStart+1)*((double)viewPortHeight/2)+vPortY) < height ? round((trueYStart+1)*((double)viewPortHeight/2)+vPortY) : round((trueYStart+1)*((double)viewPortHeight/2)+vPortY) - 1;
        auto pixelXFinish=round((trueXFinish+1)*((double)viewPortWidth/2)+vPortX) < width ? round((trueXFinish+1)*((double)viewPortWidth/2)+vPortX) : round((trueXFinish+1)*((double)viewPortWidth/2)+vPortX) - 1;
        auto pixelYFinish=round((trueYFinish+1)*((double)viewPortHeight/2)+vPortY) < height ? round((trueYFinish+1)*((double)viewPortHeight/2)+vPortY) : round((trueYFinish+1)*((double)viewPortHeight/2)+vPortY) - 1;

        glLineCoord(pixelXStart, pixelYStart, pixelXFinish, pixelYFinish);
    }

    void glLineCoord(int xStart, int yStart, int xFinish, int yFinish){
        bool isBackwards = xStart > xFinish;

        auto pixelXStart = (isBackwards) ? xFinish : xStart;
        auto pixelXFinish = (isBackwards) ? xStart : xFinish;
        auto pixelYStart = (isBackwards) ? yFinish : yStart;
        auto pixelYFinish = (isBackwards) ? yStart : yFinish;

        /*std:: cout << pixelXStart << 'x' << '\n';
        std:: cout << pixelYStart << 'y' << '\n';
        std:: cout << pixelXFinish << 'X' << '\n';
        std:: cout << pixelYFinish << 'Y' << '\n';*/

        if (pixelXFinish == pixelXStart){
            for(int i=pixelYStart; i <= pixelYFinish; i++){
                glVertexCoord(pixelXStart, i);
            }
        } else if (pixelYFinish == pixelYStart){
            for(int i=pixelXStart; i <= pixelXFinish; i++){
                glVertexCoord(i, pixelYStart);
            }
        } else{
            auto dy = (double)(pixelYFinish-pixelYStart);
            auto dx = (double)(pixelXFinish-pixelXStart);
            auto slope = dy/dx;

            /*std::cout << dy << ' ' << dx << '\n';
            std::cout << '\n' << slope <<'\n';*/

            bool isSteep = (abs(dy) > abs(dx));

            int testX = pixelXStart;
            int testY = pixelYStart;
            int testX2 = pixelXFinish;
            int testY2 = pixelYFinish;

            if (isSteep){
                std::swap(testX, testY);
                std::swap(testX2, testY2);
            }

            if (isSteep && dy/dx < 0){
                std::swap(testX, testX2);
                std::swap(testY, testY2);
            }

            double offset = 0;
            double limit = 0.5;
            int currentY = testY;
            int start = testX;
            int finish = testX2;

            slope = !isSteep ? dy / dx : dx / dy;
            /*std::cout << '\n' << isSteep << 't' << '\n';
            std::cout << start << 's' << '\n';
            std::cout << finish << 'f' << '\n' << '\n';*/

            for(int i=start; i <= finish ; i++){
                if (isSteep){
                    glVertexCoord((int)(currentY), i);
                } else{
                    glVertexCoord(i, (int)(currentY));
                }
                /*std::cout << i << ' ' << (int)(currentY) <<'\n';*/
                offset += abs(slope);
                if (abs(offset) >= limit){
                    (pixelYFinish>pixelYStart)? currentY++ : currentY--;
                    limit += 1;
                }
            }
        }

    }

    void glObj(string fileName) {
        string line;
        string v;
        vector<vector<double>> points;
        vector<vector<double>> lines;
        vector<vector<double>> faces;

        ifstream myFile (R"(C:\\Users\\Rodrigo Garoz\\CLionProjects\\test\\lego.obj)");
        while(!myFile.eof()){
            getline (myFile,line);
            if (line[0] =='v'){
                if (line[1] !='n' && line[1] != 't'){
                    vector<double> newPointCoordinates;
                    istringstream ss(line);
                    do {
                        string word;
                        ss >> word;
                        try {
                            //std::cout << word << '\n';
                            newPointCoordinates.push_back(std::stod(word));
                        } catch (...) {
                        }
                    } while (ss);
                    points.push_back(newPointCoordinates);
                }else{/*
                    vector<double> newLinePoints;
                    istringstream ss(line);
                    do {
                        string word;
                        ss >> word;
                        try {
                            newLinePoints.push_back(std::stod(word));
                        } catch (...) {
                        }
                    } while (ss);
                    lines.push_back(newLinePoints);*/
                }
            } else if(line[0] == 'f'){
                vector<double> newFacePoints;
                istringstream ss(line);
                do {
                    string word;
                    ss >> word;
                    try {
                        newFacePoints.push_back(std::stod(word));
                    } catch (...) {
                    }
                    //std::cout << word;
                } while (ss);
                faces.push_back(newFacePoints);
            }
        }

        double scaleX = 5;
        double scaleY = 5;
        double translateX = 500;
        double translateY = 500;

        /*for (auto & element : points){
            glVertexCoord((int)(scaleX*element[0] + translateX), (int)(scaleY*element[1] + translateY));
        }*/
        int n = 0;
        for (auto & element : faces){
            int size = element.size();
            int adjusted;
            int nextAdjusted;
            for (int i = 0; i < size; i++){
                adjusted = (int)element[i] - 1;
                nextAdjusted = (int)element[(i+1)%size] - 1;

                auto start = points[adjusted];
                auto finish = points[nextAdjusted];

                auto xStart = (int)round(start[0]*scaleX + translateX);
                auto yStart = (int)round(start[1]*scaleY + translateY);
                auto xFinish = (int)round(finish[0]*scaleX + translateX);
                auto yFinish = (int)round(finish[1]*scaleY + translateY);

                glLineCoord(xStart, yStart, xFinish, yFinish);
                /*std::cout << ' ' << adjusted << ' ' << nextAdjusted << ' ' << '\n';
                std::cout << ' ' << xStart << ' ' << yStart << ' ' << xFinish << ' ' << yFinish << '\n';*/
            }
            /*++n;
            std::cout << '\n';*/
            /*if (n == 1241){
                glLineCoord(0, 0, 200, 100);
                break;
            }*/
            //cout << points.size() << '\n';
        }
    }

    void glPoly(vector<vector<int>> points){
        if (points.size() <= 3){
            auto startPoints = points;
            int size = points.size();
            vector<vector<int>> convexPoints;
            vector<vector<int>> concavePoints;
            int j = 0;

            for (int i = 0; i < size; i++) {
                auto start = startPoints[i];
                auto finish = startPoints[(i+1)%size];
                auto xStart = start[0];
                auto yStart = start[1];
                auto xFinish = finish[0];
                auto yFinish = finish[1];
                glLineCoord(xStart, yStart, xFinish, yFinish);
            }
        } else{
            auto startPoints = points;
            int size = points.size();
            vector<vector<int>> convexPoints;
            vector<vector<int>> concavePoints;
            int j = 0;

            for (int i = 0; i < size; i++) {
                auto start = startPoints[i];
                auto finish = startPoints[(i+1)%size];
                auto xStart = start[0];
                auto yStart = start[1];
                auto xFinish = finish[0];
                auto yFinish = finish[1];
                glLineCoord(xStart, yStart, xFinish, yFinish);
            }

            for (int i = 0; i < size; i++) {auto prior = startPoints[i];
                auto center = startPoints[(i+1)%size];
                auto next = startPoints[(i+2)%size];

                if (isConvex(center, next, prior)){
                    glLineCoord(next[0], next[1], prior[0], prior[1]);
                    startPoints.erase(startPoints.begin()+(i+1)%size);
                    size = startPoints.size();
                }
            }
            glPoly(startPoints);
        }
    }

    bool isConvex(vector<int> center, vector<int> next, vector<int> prior){
        /*std::cout << prior[0] << ' ' << center[0] << ' ' << next [0] << '\n';
        std::cout << prior[1] << ' ' << center[1] << ' ' << next [1] << '\n';*/
        auto legOneX = next[0] - center[0];
        auto legOneY = next[1] - center[1];

        auto legTwoX = prior[0] - center[0];
        auto legTwoY = prior[1] - center[1];

        auto number = legOneX*legTwoY-legOneY*legTwoX;
        return (number > 0);
    };

    void glPaintTriangle(int firstCoord, int secondCoord, int thirdCoord){
        
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
