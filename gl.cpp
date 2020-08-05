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
#include <iomanip>
#include <type_traits>
#include <typeinfo>
#include <string>
#include <cstdlib>
#include <limits>
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
    int *zBuffer;

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
        zBuffer = new int[height*width]();
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
                zBuffer[i*width+j] = -std::numeric_limits<int>::max();
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

    void glColor(vector<double> newColor){
        paintColor[0] = newColor[0] * 255;
        paintColor[1] = newColor[1] * 255;
        paintColor[2] = newColor[2] * 255;
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
            glLineVertical(pixelXStart, pixelYStart, pixelYFinish);
        } else if (pixelYFinish == pixelYStart){
            glLineHorizontal(pixelYStart, pixelXStart, pixelXFinish);
        } else {
            glLineCoordDiagonal(pixelXStart, pixelYStart, pixelXFinish, pixelYFinish);
        }
    }

    void glLineVertical(int xCoord, int yStart, int yFinish){
        int pixelXStart = xCoord;
        int pixelYStart = yStart;
        int pixelYFinish = yFinish;

        for(int i=pixelYStart; i <= pixelYFinish; i++){
            glVertexCoord(pixelXStart, i);
        }
    }

    void glLineHorizontal(int yCoord, int xStart, int xFinish){
        int pixelYStart = yCoord;
        int pixelXStart = xStart;
        int pixelXFinish = xFinish;

        for(int i=pixelXStart; i <= pixelXFinish; i++){
            glVertexCoord(i, pixelYStart);
        }
    }

    void glLineCoordDiagonal(int pixelXStart, int pixelYStart, int pixelXFinish, int pixelYFinish){
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

    void glObj(string fileName, bool isWireframe = false) {
        string line;
        string v;
        vector<vector<double>> points;
        vector<vector<double>> lines;
        vector<vector<double>> faces;

        vector<int> light = {0, 0, 1};

        ifstream myFile (R"(C:\\Users\\Rodrigo Garoz\\CLionProjects\\test\\model.obj)");
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

        double scaleX = 300;
        double scaleY = 300;
        double scaleZ = 300;
        double translateX = 500;
        double translateY = 500;
        double translateZ = 0;

        /*for (auto & element : points){
            glVertexCoord((int)(scaleX*element[0] + translateX), (int)(scaleY*element[1] + translateY));
        }*/
        int n = 0;
        for (auto & element : faces){
            vector<vector<int>> polyPoints;
            vector<int> onePoint;
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
                auto zStart = (int)round(start[2]*scaleZ + translateZ);
                auto xFinish = (int)round(finish[0]*scaleX + translateX);
                auto yFinish = (int)round(finish[1]*scaleY + translateY);

                //glLineCoord(xStart, yStart, xFinish, yFinish);
                if (!isWireframe){
                    polyPoints.push_back({xStart, yStart, zStart});
                }
            }
            if (!isWireframe){
                auto v0 = glVectorIntToDouble(polyPoints[0]);
                auto v1 = glVectorIntToDouble(polyPoints[1]);
                auto v2 = glVectorIntToDouble(polyPoints[2]);
                auto newLight = glVectorIntToDouble(light);
                //std::cout << v0[0] << ' ' << v0[1] << ' ' << v0[2] <<'\n';
                //std::cout << v1[0] << ' ' << v1[1] << ' ' << v1[2] <<'\n';
                //std::cout << v2[0] << ' ' << v2[2] << ' ' << v2[2] <<'\n';

                vector<double> v0v1 = glSubstractVectors(v0, v1);
                vector<double> v0v2 = glSubstractVectors(v0, v2);
                //std::cout << v0v1[0] << ' ' << v0v1[1] << ' ' << v0v1[2] <<'\n';
                //std::cout << v0v2[0] << ' ' << v0v2[1] << ' ' << v0v2[2] <<'\n';

                vector<double> normal = glCrossProduct(v0v1, v0v2);
                //std::cout << normal[0] << ' ' << normal[1] << ' ' << normal[2] <<'\n';
                //std::cout << std::setprecision(5) << glGetNorm(normal) << '\n';

                normal = glMultiplyVectorWithConstant(normal, 1/glGetNorm(normal));
                double intensity = glPointProduct(normal, newLight);
                //std::cout << std::setprecision(5) << intensity << '\n';
                glColor({intensity, intensity, intensity});
                glPoly(polyPoints, intensity);
                polyPoints.clear();
            }
        }
        glPaintBuffer();
    }

    vector<double> glVectorIntToDouble(vector<int> vector){
        std::vector<double> test;
        for (auto & element:vector) {
            test.push_back((double)element);
        }
        return test;
    }

    vector<int> glVectorDoubleToInt(vector<double> vector){
        std::vector<int> test;
        for (auto & element:vector) {
            test.push_back((int)element);
        }
        return test;
    }

    vector<double> glTransform(vector<double> point, vector<double> scale = {1, 1, 1}, vector<double> translate = {0, 0, 0}){
        return {point[0]*scale[0]+translate[0], point[1]*scale[1]+translate[1], point[2]*scale[2]+translate[2]};
    }

    vector<double> glSubstractVectors(vector<double> start, vector<double> end){
        return {end[0] - start[0], end[1] - start[1], end[2] - start[2]};
    }

    vector<double> glCrossProduct(vector<double> firstPoint, vector<double> secondPoint){
        return {firstPoint[1]*secondPoint[2]-firstPoint[2]*secondPoint[1],
                firstPoint[2]*secondPoint[0]-firstPoint[0]*secondPoint[2],
                firstPoint[0]*secondPoint[1]-firstPoint[1]*secondPoint[0]};
    }

    vector<double> glMultiplyVectorWithConstant(vector<double> vector, double constant){
        auto v0 = (vector[0]*constant);
        auto v1 = (vector[1]*constant);
        auto v2 = (vector[2]*constant);
        return {v0, v1, v2};
    }

    double glPointProduct(vector<double> vector1, vector<double> vector2){
        return vector1[0]*vector2[0]+vector1[1]*vector2[1]+vector1[2]*vector2[2];
    }

    double glGetNorm(vector<double> vector){
        return sqrt(pow(vector[0], 2)+pow(vector[1], 2)+pow(vector[2], 2));
    }

    void glPoly(vector<vector<int>> points, double intensity = 1){
        if (points.size() < 3){
            auto startPoints = points;
            int size = points.size();
            vector<vector<int>> convexPoints;
            vector<vector<int>> concavePoints;

            for (int i = 0; i < size; i++) {
                auto start = startPoints[i];
                auto finish = startPoints[(i+1)%size];
                auto xStart = start[0];
                auto yStart = start[1];
                auto xFinish = finish[0];
                auto yFinish = finish[1];
                //glLineCoord(xStart, yStart, xFinish, yFinish);
            }
            //glPaintTriangle(startPoints[0], startPoints[1], startPoints[2]);
        } else{
            auto startPoints = points;
            int size = points.size();
            vector<vector<int>> convexPoints;
            vector<vector<int>> concavePoints;
            int convexSize = convexPoints.size();

            for (int i = 0; i < size; i++) {
                auto start = startPoints[i];
                auto finish = startPoints[(i+1)%size];
                auto xStart = start[0];
                auto yStart = start[1];
                auto xFinish = finish[0];
                auto yFinish = finish[1];
                //glLineCoord(xStart, yStart, xFinish, yFinish);
            }

            for (int i = 0; i < size; i++) {
                auto prior = startPoints[i];
                auto center = startPoints[(i+1)%size];
                auto next = startPoints[(i+2)%size];

                if (isConvex(center, next, prior)){
                    glLineCoord(next[0], next[1], prior[0], prior[1]);
                    startPoints.erase(startPoints.begin()+(i+1)%size);
                    size = startPoints.size();
                    glPaintTriangleBC(prior, center, next, intensity);
                    //std::cout << size << '\n';
                }
            }
            if (convexSize > 0){
                glPoly(startPoints);
            }
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

    void glPaintTriangle(vector<int> firstCoord, vector<int> secondCoord, vector<int> thirdCoord, double intensity){
        if (firstCoord[1] == secondCoord[1] && thirdCoord[1] > firstCoord[1]){
            glPaintFlatBottomTriangle(thirdCoord, firstCoord, secondCoord);
        } else if (firstCoord[1] == thirdCoord[1] && secondCoord[1] > firstCoord[1]){
            glPaintFlatBottomTriangle(secondCoord, firstCoord, thirdCoord);
        } else if (thirdCoord[1] == secondCoord[1] && firstCoord[1] > thirdCoord[1]){
            glPaintFlatBottomTriangle(firstCoord, thirdCoord, secondCoord);
        } else if (firstCoord[1] == secondCoord[1] && thirdCoord[1] < firstCoord[1]){
            glPaintFlatTopTriangle(thirdCoord, firstCoord, secondCoord);
        } else if (firstCoord[1] == thirdCoord[1] && secondCoord[1] < firstCoord[1]){
            glPaintFlatTopTriangle(secondCoord, firstCoord, thirdCoord);
        } else if (thirdCoord[1] == secondCoord[1] && firstCoord[1] < thirdCoord[1]){
            glPaintFlatTopTriangle(firstCoord, thirdCoord, secondCoord);
        }else{
            glSplitTriangle(firstCoord, secondCoord, thirdCoord);
        }
    }

    bool glSplitTriangle(vector<int> firstPoint, vector<int> secondPoint, vector<int> thirdPoint){

        vector<int> highestPoint;
        vector<int> middlePoint;
        vector<int> lowestPoint;

        if (firstPoint[1] > secondPoint[1] && secondPoint[1] > thirdPoint[1]){
            highestPoint = firstPoint;
            middlePoint = secondPoint;
            lowestPoint = thirdPoint;
        } else if (firstPoint[1] > thirdPoint[1] && thirdPoint[1] > secondPoint[1]){
            highestPoint = firstPoint;
            middlePoint = thirdPoint;
            lowestPoint = secondPoint;
        } else if (secondPoint[1] > firstPoint[1] && firstPoint[1] > thirdPoint[1]){
            highestPoint = secondPoint;
            middlePoint = firstPoint;
            lowestPoint = thirdPoint;
        } else if (secondPoint[1] > thirdPoint[1] && thirdPoint[1] > firstPoint[1]){
            highestPoint = secondPoint;
            middlePoint = thirdPoint;
            lowestPoint = firstPoint;
        } else if (thirdPoint[1] > firstPoint[1] && firstPoint[1] > secondPoint[1]){
            highestPoint = thirdPoint;
            middlePoint = firstPoint;
            lowestPoint = secondPoint;
        } else if (thirdPoint[1] > secondPoint[1] && secondPoint[1] > firstPoint[1]){
            highestPoint = thirdPoint;
            middlePoint = secondPoint;
            lowestPoint = firstPoint;
        }

        /*std::cout << highestPoint[0] << ' ' << highestPoint[1] << '\n';
        std::cout << middlePoint[0] << ' ' << middlePoint[1] << '\n';
        std::cout << lowestPoint[0] << ' ' << lowestPoint[1] << '\n';*/

        double otherMiddlePointX = ((double)(highestPoint[0]-lowestPoint[0])/(highestPoint[1]-lowestPoint[1]))*(middlePoint[1]-highestPoint[1]);
        vector<int> otherMiddlePoint = {(int)round(otherMiddlePointX)+highestPoint[0], middlePoint[1]};

        glLineHorizontal(middlePoint[1], otherMiddlePoint[0], middlePoint[0]);
        glPaintFlatBottomTriangle(highestPoint, otherMiddlePoint, middlePoint);
        glPaintFlatTopTriangle(lowestPoint, otherMiddlePoint, middlePoint);
    }

    void glPaintFlatTopTriangle(vector<int> bottomPoint, vector<int> firstPoint, vector<int> secondPoint){
        if (firstPoint[0] > secondPoint[0]){
            std::swap(firstPoint, secondPoint);
        }

        double inverseSlope1 = (double)(firstPoint[0] - bottomPoint[0])/(double)(firstPoint[1] - bottomPoint[1]);
        double inverseSlope2 = (double)(secondPoint[0] - bottomPoint[0])/(double)(secondPoint[1] - bottomPoint[1]);

        double currentX1 = bottomPoint[0];
        double currentX2 = bottomPoint[0];

        for (int scanLineY = bottomPoint[1]; scanLineY <= firstPoint[1] ; ++scanLineY) {
            glLineHorizontal(scanLineY, (int)currentX1, (int)currentX2);
            currentX1 += inverseSlope1;
            currentX2 += inverseSlope2;
        }
    }

    void glPaintFlatBottomTriangle(vector<int> topPoint, vector<int> firstPoint, vector<int> secondPoint){
        if (firstPoint[0] > secondPoint[0]){
            std::swap(firstPoint, secondPoint);
        }

        double inverseSlope1 = (double)(firstPoint[0] - topPoint[0])/(double)(firstPoint[1] - topPoint[1]);
        double inverseSlope2 = (double)(secondPoint[0] - topPoint[0])/(double)(secondPoint[1] - topPoint[1]);

        double currentX1 = topPoint[0];
        double currentX2 = topPoint[0];

        for (int scanLineY = topPoint[1]; scanLineY >= firstPoint[1] ; --scanLineY) {
            glLineHorizontal(scanLineY, (int)currentX1, (int)currentX2);
            currentX1 -= inverseSlope1;
            currentX2 -= inverseSlope2;
        }
    }

    vector<double> glBarycentricCoordinates(vector<int> A, vector<int> B, vector<int> C, vector<int> P){
        auto testA = A[0];
        auto testA1 = A[1];
        auto testA2 = A[2];

        auto testB = B[0];
        auto testB1 = B[1];
        auto testB2 = B[2];

        auto testC = C[0];
        auto testC1 = C[1];
        auto testC2 = C[2];

        auto testP = P[0];
        auto testP1 = P[1];
        auto testP2 = P[2];

        double u0 = ((double)(B[1]-C[1])*(P[0]-C[0])+(C[0]-B[0])*(P[1]-C[1]));

        double u1 = ((double)(B[1]-C[1])*(A[0]-C[0])+(C[0]-B[0])*(A[1]-C[1]));

        double u = u0/u1;

        auto v = (
                (double)(((C[1]-A[1])*(P[0]-C[0]) + (A[0]-C[0])*(P[1]-C[1])))
                /
                (((B[1]-C[1])*(A[0]-C[0]) + (C[0]-B[0])*(A[1]-C[1])))
        );

        auto w = 1 - u - v;

        return {u, v, w};
    }

    void glPaintBuffer(){
        int minZ = -std::numeric_limits<int>::max();
        int maxZ = std::numeric_limits<int>::max();

        for(int i=0; i<height; i++){
            for(int j=0; j<width; j++){
                if(zBuffer[i*width+j] != -std::numeric_limits<int>::max()){
                    if (zBuffer[i*width+j] < minZ){
                        minZ = zBuffer[i*width+j];
                    }
                    if (zBuffer[i*width+j] > maxZ){
                        maxZ = zBuffer[i*width+j];
                    }
                }
            }
        }

        double depth;
        for(int i=0; i<height; i++){
            for(int j=0; j<width; j++){
                depth = zBuffer[i*width+j];
                if (depth == -std::numeric_limits<int>::max()){
                    depth = minZ;
                }
                depth = (depth-minZ)/(maxZ - minZ);
                glColor({depth, depth, depth});
                glVertexCoord(i, j);
            }
        }
    }

    void glPaintTriangleBC(vector<int> A, vector<int> B, vector<int> C, double intensity = 1){
        std::cout << 'h' << '\n';
        auto minX = std::min(std::min(A[0], B[0]), C[0]);
        auto minY = std::min(std::min(A[1], B[1]), C[1]);
        auto maxX = std::max(std::max(A[0], B[0]), C[0]);
        auto maxY = std::max(std::max(A[1], B[1]), C[1]);

        for (int i = minX; i < maxX+1 ; ++i) {
            for (int j = minY; j < maxY+1; ++j) {
                if (i>width || i<0 || j>=height || j<0){
                    continue;
                }
                auto bary = glBarycentricCoordinates(A, B, C, {i, j});
                auto u = bary[0];
                auto v = bary[1];
                auto w = bary[2];
                if (u >= 0 && v>= 0 && w>=0){
                    auto z = A[2]*u + B[2]*v + C[2]*w;
                    if (z > zBuffer[i*width+j]){
                        double r = 1*intensity;
                        double g = 1*intensity;
                        double b = 1*intensity;
                        glColor({r, g, b});
                        glVertexCoord(i, j);
                        zBuffer[i*width+j] = z;
                    }
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
