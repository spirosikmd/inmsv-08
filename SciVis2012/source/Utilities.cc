#include <cmath>
#include <sstream>
#include "Utilities.h"
#include <iostream>



using namespace std;

std::string float2str(float f) {
    std::stringstream ss(std::stringstream::in | std::stringstream::out);
    ss << f;
    return ss.str();
}

void printText(GLfloat x, GLfloat y, std::string text) {
    //    glMatrixMode(GL_PROJECTION);
    //    glPushMatrix();
    //    glLoadIdentity();
    //    glOrtho(0, 1000, 0, 800, -1.0f, 1.0f);
    //    glMatrixMode(GL_MODELVIEW);
    //    glPushMatrix();
    //    glLoadIdentity();
    //    glPushAttrib(GL_DEPTH_TEST);
    //    glDisable(GL_DEPTH_TEST);
    glRasterPos2f(x, y);
    for (size_t i = 0; i != text.size(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, text[i]);
    }
    //    glPopAttrib();
    //    glMatrixMode(GL_PROJECTION);
    //    glPopMatrix();
    //    glMatrixMode(GL_MODELVIEW);
    //    glPopMatrix();
}

int clamp(float x) {
    return ((x) >= 0.0 ? ((int) (x)) : (-((int) (1 - (x)))));
}

void hsv2rgb(float h, float s, float v, float& r, float& g, float& b) {
    int hueCase = (int) (h * 6);
    float frac = 6 * h - hueCase;
    float lx = v * (1 - s);
    float ly = v * (1 - s * frac);
    float lz = v * (1 - s * (1 - frac));
    switch (hueCase) {
        case 0:
        case 6: r = v;
            g = lz;
            b = lx;
            break;
        case 1: r = ly;
            g = v;
            b = lx;
            break;
        case 2: r = lx;
            g = v;
            b = lz;
            break;
        case 3: r = lx;
            g = ly;
            b = v;
            break;
        case 4: r = lz;
            g = lx;
            b = v;
            break;
        case 5: r = v;
            g = lx;
            b = ly;
            break;
    }
}

void rgb2hsv(float r, float g, float b, float& h, float& s, float& v) {
    float M = fmax(r, fmax(g, b));
    float m = fmin(r, fmin(g, b));
    float d = M - m;
    v = M;
    s = (M > 0.00001) ? d / M : 0;
    if (s == 0) h = 0;
    else {
        if (r == M) h = (g - b) / d;
        else if (g == M) h = 2 + (b - r) / d;
        else h = 4 + (r - g) / d;
        h /= 6;
        if (h < 0) h += 1;
    }
}

double round(double r) {
    return (r >= 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}

float scale(float v, float f_min, float f_max, float min, float max) {
    return ((v - f_min)*(max - min) / (f_max - f_min) + min);
}

float magnitude(float x, float y) {
    return sqrt(pow(x, 2) + pow(y, 2));
}

GLfloat magnitude(float v[2]) {
    return sqrt(pow(v[0], 2) + pow(v[1], 2));
}

GLfloat magnitude3(float v[3]) {
    return sqrt(pow(v[0], 2) + pow(v[1], 2)+ pow(v[2], 2));
}

float dotproduct3(float u[3],float v[3]) {
    return u[0]*v[0]+u[1]*v[1]+u[2]*v[2];
}

void normalize2(float *R) {
    float m = sqrt(pow(R[0], 2) + pow(R[1], 2));
    R[0] = R[0] / m;
    R[1] = R[1] / m;
}

void normalize3(float *R) {
    float m = sqrt(pow(R[0], 2) + pow(R[1], 2) + pow(R[2], 2));
    R[0] = R[0] / m;
    R[1] = R[1] / m;
    R[2] = R[2] / m;
}

void crossproduct(float *U, float *V, float *R) {
    R[0] = (U[1] * V[2])-(V[1] * U[2]);
    R[1] = -(U[0] * V[2])+(V[0] * U[2]);
    R[2] = (U[0] * V[1])-(U[1] * V[0]);
}
void printPoint(float *xyz_new) {
    cout << "(" << xyz_new[0] << "|" << xyz_new[1] << "|" << xyz_new[2] << ")\n";
}


float angleVec(float v[3], float u[3]) {
        float m_u = magnitude3(u);
        float m_v = magnitude3(v);
        float dp = dotproduct3(u,v);
        return acos(dp/(m_u+m_v));
}