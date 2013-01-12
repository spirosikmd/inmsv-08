#include <cmath>
#include <sstream>
#include "Utilities.h"

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

float magnitude(float v[2]) {
    return sqrt(pow(v[0], 2) + pow(v[1], 2));
}