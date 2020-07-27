#include <stdio.h>
#include <iostream>
#include <string>
#include "gl.cpp"
using namespace std;

int main()
{
    int height = 500;
    int width = 800;
    gl myGl(height, width);
    myGl.glViewPort(0, 0, width, height);
    myGl.glClear();
    //myGl.glPoly(poly1);
    //myGl.glPoly(poly2);
    //myGl.glPoly(poly3);
    //myGl.glPoly(poly4);
    myGl.glObj("lego.obj");
    myGl.glFinish();
}