/* 
 * File:   Colormap.cpp
 * Author: cm
 * 
 * Created on January 9, 2013, 7:53 PM
 */

#include "Colormap.h"
#include <iostream>
#include <cmath>
#include <string>

Colormap::Colormap() {
    hue = 0.0f;
    saturation = 1.0;
    numberOfColors = COL_256;
    map.resize(COL_256);
    map[0] = BLACK;
    map[255] = WHITE;
    texture = 0;
    computeColors();
}

Colormap::Colormap(const Colormap& orig) {
}

Colormap::~Colormap() {
}

void Colormap::setHue(float h) {
    hue = h;
    computeColors();
}

float Colormap::getHue() {
    return hue;
}

void Colormap::setSaturation(float s) {
    saturation = s;
    computeColors();
}

float Colormap::getSaturation() {
    return saturation;
}

void Colormap::setNumberOfColors(int n) {
    numberOfColors = n;
    computeColors();
}

int Colormap::getNumberOfColors() {
    return numberOfColors;
}

void Colormap::putColor(HSV color, unsigned int position) {
    map[position] = color;
    computeColors();
}

HSV Colormap::getColorAt(int index) {
    return colors[index];
}

void Colormap::loadColormapTexture() {
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glBindTexture(GL_TEXTURE_1D, texture);
}

void Colormap::printColors() {
    for (unsigned i = 0; i < 256; i++) {
        std::cout << "[" << i << "](" << colors[i].hue << "," << colors[i].saturation << "," << colors[i].value << "), ";
    }
    std::cout << '\n';
}

void Colormap::render(float min, float max, int minorTicks) {
    float R, G, B;
    int step = 1;
    int width = 50;


    glColor3f(1, 1, 1);
    glBegin(GL_LINES);
    glVertex2f(1, 1);
    glVertex2f(1, 258 * step);
    glVertex2f(1, 258 * step);
    glVertex2f(width, 258 * step);
    glVertex2f(width, 258 * step);
    glVertex2f(width, 1);
    glVertex2f(width, 1);
    glVertex2f(1, 1);
    glEnd();

    glBegin(GL_QUADS);
    for (size_t i = 0; i < 256; i++) {
        hsv2rgb(colors[i].hue, colors[i].saturation, colors[i].value, R, G, B);
        glColor3f(R, G, B);
        glVertex2f(1, 1+(i * step) + step); // Top Left
        glVertex2f(width - 1, 1+(i * step) + step); // Top Right
        glColor3f(R, G, B);
        glVertex2f(width - 1, 1+i * step); // Bottom Right
        glVertex2f(1, 1+i * step); // Bottom Left
    }
    glEnd();

 
    glColor3f(1, 1, 1);
    glBegin(GL_LINES);
    glVertex2f(width, 1);
    glVertex2f(width+5, 1);
    glEnd();
    printText(width+8,1-3.5, float2str(min));
    
    minorTicks=minorTicks+1;
    for (int tick = 1; tick < minorTicks; tick++) {
        glBegin(GL_LINES);
        glVertex2f(width, tick * ((256 * step) / minorTicks));
        glVertex2f(width+2, tick * ((256 * step) / minorTicks));
        glEnd();
        
        float val = min + tick *(fabs(min-max)/minorTicks);
        printText(width+5,tick * ((256 * step) / minorTicks)-3.5, float2str(val));
    }

    glBegin(GL_LINES);
    glVertex2f(width, 256 * step);
    glVertex2f(width+5, 256 * step);
    glEnd();
    printText(width+8,256 * step-3.5, float2str(max));
}

HSV Colormap::interpolate(float x, float x0, float x1) {
    HSV interpolated;

    HSV y0 = map[x0];
    HSV y1 = map[x1];

    interpolated.hue = y0.hue + (y1.hue - y0.hue) * ((x - x0) / (x1 - x0));
    interpolated.saturation = y0.saturation + (y1.saturation - y0.saturation) * ((x - x0) / (x1 - x0));
    interpolated.value = y0.value + (y1.value - y0.value) * ((x - x0) / (x1 - x0));

    return interpolated;
}

void Colormap::computeColors() {
    int left = 0;
    int right = -1;
    for (unsigned i = 1; i < map.size(); i++) {
        if (map[i] != NULLHSV) {
            right = i;
            colors[right] = map[right];
            for (int pos = left + 1; pos < right; pos++) {
                colors[pos] = interpolate(pos, left, right);
            }
            left = right;
            right = -1;
        }
    }
    colors[0] = map[0];
    colors[255] = map[255];

    for (unsigned i = 0; i < 256; i++) {
        colors[i].hue = fmod(colors[i].hue + hue, 1.0);
        colors[i].saturation = colors[i].saturation * saturation;

    }

    int step = 256 / numberOfColors;
    for (size_t i = 0; i < 256; i++) {

        int c = step * (int) floor(i / step);
        if (i >= 128) {
            c = c + step - 1;
        }
        colors[i] = colors[c];
    }

    computeTexture();
}

void Colormap::computeTexture() {
    GLfloat rgbTexture[256][3];
    GLfloat R, G, B;
    for (int i = 0; i < 256; i++) {
        hsv2rgb(colors[i].hue, colors[i].saturation, colors[i].value, R, G, B);
        rgbTexture[i][0] = R;
        rgbTexture[i][1] = G;
        rgbTexture[i][2] = B;
    }

    //delete old texture
    if (texture != 0) {
        glDeleteTextures(1, &texture);
    }

    // allocate a texture name
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_1D, texture);
    glTexImage1D(GL_TEXTURE_1D, 0, 3, 256, 0, GL_RGB, GL_FLOAT, rgbTexture); // array with color values    
}