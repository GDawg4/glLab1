#include <stdio.h>
#include <iostream>
#include <string>
#include "gl.cpp"
using namespace std;

int main()
{
    int height = 6000;
    int width = 6000;
    gl myGl(height, width);
    myGl.glViewPort(0, 0, width, height);
    myGl.glClear();
    /*vector<vector<int>> poly1 {{165, 380}, {185, 360}, {180, 330}, {207, 345}, {233, 330}, {230, 360}, {250, 380}, {220, 385}, {205, 410}, {193, 383}};
    vector<vector<int>> poly2 {{321, 335}, {288, 286}, {339, 251}, {374, 302}};
    vector<vector<int>> poly3 {{377, 249}, {411, 197}, {436, 249}};
    vector<vector<int>> poly4 {{413, 177}, {448, 159}, {502, 88} ,{553, 53} ,{535, 36} ,{676, 37} ,{660, 52}, {750, 145}, {761, 179}, {672, 192}, {659, 214}, {615, 214}, {632, 230}, {580, 230},{597, 215}, {552, 214}, {517, 144}, {466, 180}};
    vector<vector<int>> poly5 {{682, 175}, {708, 120}, {735, 148}, {739, 170}};*/

    /*myGl.glPoly(poly1);
    myGl.glPoly(poly2);
    myGl.glPoly(poly3);
    myGl.glPoly(poly4);
    myGl.glColor(0,0,0);
    myGl.glPoly(poly5);*/
    myGl.glObj("lego.obj");
    myGl.glFinish();
}