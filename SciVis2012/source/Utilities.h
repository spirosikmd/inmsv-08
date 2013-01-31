#ifndef UTILITIES_H_INCLUDED
#define	UTILITIES_H_INCLUDED

#include <rfftw.h>
#include <string>
#include <GLUT/glut.h>
#include <OpenGL/gl.h>

std::string float2str(float f);
int clamp(float x);
void hsv2rgb(float h, float s, float v, float& r, float& g, float& b);
void rgb2hsv(float r, float g, float b, float& h, float& s, float& v);
//double round(double r);
float scale(float v, float f_min, float f_max, float min, float max);
float magnitude(float x, float y);
GLfloat magnitude(float v[]);

void printText(GLfloat x, GLfloat y, std::string text);

#endif

