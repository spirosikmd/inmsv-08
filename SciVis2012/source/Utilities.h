#ifndef UTILITIES_H_INCLUDED
#define	UTILITIES_H_INCLUDED

#include <rfftw.h>

int clamp(float x);
void hsv2rgb(float h, float s, float v, float& r, float& g, float& b);
double round(double r);
float scale(float v, float f_min, float f_max, float min, float max);
float magnitude(float x, float y);

#endif

