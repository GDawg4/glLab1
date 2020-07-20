#include <stdio.h>
#include <iostream>
#include <string>
#include "gl.cpp"
using namespace std;

int main()
{
    int height = 25;
    int width = 25;
    gl myGl(height, width);
    myGl.glViewPort(0, 0, height, width);
    myGl.glClear();
    //myGl.glLine(-1, -1, 1, 1);
    //myGl.glLine(-1, 1, 1, -1);
    //myGl.glLine(0, -1, 0, 1);
    //myGl.glLine(-1, 0, 1, 0);
    myGl.glColor(0.5, 0.5,0.5);
    //myGl.glLine(-1, -1, 1,0);
    //myGl.glLine(-1, -1, 0,1);
    myGl.glLine(1, -1, -1, 1);
    //myGl.glVertexCoord(19, 19);
    myGl.glFinish();
}