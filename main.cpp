#include <stdio.h>
#include <iostream>
#include <string>
#include "gl.cpp"
using namespace std;

int main()
{
    gl myGl(5, 5);
    myGl.glViewPort(0, 0, 5, 5);
    myGl.glVertex(0,0);
    myGl.glFinish();
}