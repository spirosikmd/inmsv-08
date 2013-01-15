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

void Colormap::putColor(RGB color, unsigned int position) {
    map[position] = color;
    computeColors();
}

RGB Colormap::getColorAt(int index) {
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
        std::cout << "[" << i << "](" << colors[i].red << "," << colors[i].green << "," << colors[i].blue << "), ";
    }
    std::cout << '\n';
}

void Colormap::render(float min, float max, int minorTicks) {
    int step = 1;
    int width = 50;
    
    glTranslatef(10, 10, 0);
    glColor3f(1, 1, 1);
    glBegin(GL_LINES);
    glVertex2f(1, 1);
    glVertex2f(1, 256 * step +2);
    glVertex2f(1, 256 * step +2);
    glVertex2f(width, 256 * step+2);
    glVertex2f(width, 256 * step+2);
    glVertex2f(width, 1);
    glVertex2f(width, 1);
    glVertex2f(1, 1);
    glEnd();

    glBegin(GL_QUADS);
    for (size_t i = 0; i < 256; i++) {
        glColor3f(colors[i].red, colors[i].green, colors[i].blue);
        glVertex2f(1, 1+(i * step) + step); // Top Left
        glVertex2f(width - 1, 1+(i * step) + step); // Top Right
        glColor3f(colors[i].red, colors[i].green, colors[i].blue);
        glVertex2f(width - 1, 1+i * step); // Bottom Right
        glVertex2f(1, 1+i * step); // Bottom Left
    }
    glEnd();

 
    glColor3f(1, 1, 1);
    glBegin(GL_LINES);
    glVertex2f(width, 2);
    glVertex2f(width+5, 2);
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
    glVertex2f(width, 256 * step+1);
    glVertex2f(width+5, 256 * step+1);
    glEnd();
    printText(width+8,256 * step-3.5, float2str(max));
}

RGB Colormap::interpolate(float x, float x0, float x1) {
    RGB interpolated;

    RGB y0 = map[x0];
    RGB y1 = map[x1];
        
     interpolated.red= y0.red + (y1.red - y0.red) * ((x - x0) / (x1 - x0));
     interpolated.green= y0.green + (y1.green - y0.green) * ((x - x0) / (x1 - x0));
     interpolated.blue= y0.blue + (y1.blue - y0.blue) * ((x - x0) / (x1 - x0));
   
    return interpolated;
}

void Colormap::computeColors() {
    int left = 0;
    int right = -1;
    for (unsigned i = 1; i < map.size(); i++) {
        if (map[i] != NULLRGB) {
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
        float H,S,V;
        rgb2hsv(colors[i].red,colors[i].green,colors[i].blue,H,S,V);
        H = fmod(H + hue, 1.0);
        S = S * saturation;
        hsv2rgb(H,S,V,colors[i].red,colors[i].green,colors[i].blue);
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
    
    for (int i = 0; i < 256; i++) {
        rgbTexture[i][0] = colors[i].red;
        rgbTexture[i][1] = colors[i].green;
        rgbTexture[i][2] = colors[i].blue;
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