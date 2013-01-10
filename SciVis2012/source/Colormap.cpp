/* 
 * File:   Colormap.cpp
 * Author: cm
 * 
 * Created on January 9, 2013, 7:53 PM
 */

#include "Colormap.h"
#include <iostream>
#include <OpenGL/gl.h>
#include <cmath>

Colormap::Colormap() {
    hue = 0.0f;
    saturation = 1.0;
    numberOfColors = COL_256;
    limitation = CLAMPING;
    clampMin = 0.0;
    clampMax = 1.0;
    map.resize(COL_256);
    map[0] = BLACK;
    map[255] = WHITE;
    computeColors();
}

void Colormap::putColor(HSV color, unsigned int position) {
    map[position] = color;
    computeColors();
    //    for (unsigned i = 0; i < map.size(); i++) {
    //        if (map[i] != NULLHSV) {
    //            std::cout << "[" << i << "](" << map[i].hue << "," << map[i].saturation << "," << map[i].value << "), ";
    //        }
    //    }
    //    std::cout << '\n';
}

Colormap::Colormap(const Colormap& orig) {
}

Colormap::~Colormap() {
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
}

void Colormap::printColors() {
    for (unsigned i = 0; i < 256; i++) {
        std::cout << "[" << i << "](" << colors[i].hue << "," << colors[i].saturation << "," << colors[i].value << "), ";
    }
    std::cout << '\n';
}

void Colormap::setHue(float h) {
    hue = h;
}

float Colormap::getHue() {return hue;}

void Colormap::setSaturation(float s) {
    saturation = s;
}

float Colormap::getSaturation() {return saturation;}

void Colormap::setNumberOfColors(int n ) {
    numberOfColors  = n;
}

void Colormap::render() {
    float R, G, B;
    int step = 256/numberOfColors;
    int width = 50;
    
    glBegin(GL_QUADS);
    for (size_t i = 0,j=0; i != 256; i= i+step,j++) {
        int c = i;
        
        if (c>=127) {
            c=c+step-1;
        }
        
        float h = fmod(colors[c].hue + hue , 1.0);
        float s = colors[c].saturation * saturation;
        hsv2rgb(h, s, colors[c].value, R, G, B);
        glColor3f(R, G, B);

        
        glVertex3i(0, (j * step) + step, 0); // Top Left
        glVertex3i(width, (j * step) + step, 0); // Top Right
        glColor3f(R, G, B);
        glVertex3i(width, j* step, 0); // Bottom Right
        glVertex3i(0, j * step, 0); // Bottom Left
        
    }
    glEnd();
}

Colormap* Colormap::Rainbow() {
    static Colormap* colormap = new Colormap();

    colormap->putColor(HSV(0, 1, 1), 255);
    colormap->putColor(HSV((1.0 / 360.0)  * 120.0, 1, 1), 127);
    colormap->putColor(BLUE, 0);

    return colormap;
}

Colormap* Colormap::Grayscale() {
    static Colormap* colormap = new Colormap();

    colormap->putColor(WHITE, 256);
    colormap->putColor(BLACK, 0);
    colormap->setSaturation(0);
    return colormap;
}

Colormap* Colormap::Zebra() {
    static Colormap* colormap = new Colormap();

    for (int i = 0; i < 256; i = i + 2) {
        colormap->putColor(WHITE, i);
    }
    for (int i = 1; i < 256; i = i + 2) {
        colormap->putColor(BLACK, i);
    }

    return colormap;
}