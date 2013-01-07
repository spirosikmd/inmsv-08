#include <cmath>

#include "Utilities.h"

int clamp(float x)
{ 
    return ((x)>=0.0?((int)(x)):(-((int)(1-(x))))); 
}

void hsv2rgb(float h, float s, float v, float& r, float& g, float& b)
{
    int hueCase = (int)(h*6);
    float frac = 6*h-hueCase;
    float lx = v*(1-s);
    float ly = v*(1-s*frac);
    float lz = v*(1-s*(1-frac));
    switch (hueCase)
    {
        case 0:
        case 6: r=v;    g=lz;   b=lx;   break;
        case 1: r=ly;   g=v;    b=lx;   break;
        case 2: r=lx;   g=v;    b=lz;   break;
        case 3: r=lx;   g=ly;   b=v;    break;
        case 4: r=lz;   g=lx;   b=v;    break;
        case 5: r=v;    g=lx;   b=ly;   break;
    }
}

double round(double r)
{
    return (r >= 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}

float scale(float v, float f_min, float f_max, float min, float max)
{
    return ((v-f_min)*(max-min)/(f_max-f_min)+min);
}

float magnitude(float x, float y)
{
    return sqrt(pow(x, 2) + pow(y, 2));
}

float magnitude(float v[2])
{
    return sqrt(pow(v[0], 2) + pow(v[1], 2));
}