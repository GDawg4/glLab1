#include <stdio.h>
#include <iostream>
#include <string>
#include "gl.cpp"
using namespace std;

int main()
{
    gl myGl(5, 5);
    myGl.glViewPort(0, 0, 5, 5);
    myGl.glClearColor(0.8, 0.4, 0);
    myGl.glClear();
    myGl.glVertex(0, 0);
    myGl.glColor(1, 0, 0.5);
    myGl.glVertex(1,1);
    myGl.glFinish();
}