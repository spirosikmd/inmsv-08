/* 
 * File:   Colormap.cpp
 * Author: cm
 * 
 * Created on January 9, 2013, 7:53 PM
 */

#include "Colormap.h"
#include <iostream>

Colormap::Colormap() {
    hue = 0.0f;
    saturation = 1.0;
    numberOfColors = COL_256;
    limitation = CLAMPING;
    clampMin = 0.0;
    clampMax = 1.0;
    map.resize(COL_256);
}

void Colormap::putColor(HSV color, unsigned int position) {
    map[position] = color;
    for (unsigned i = 0; i < map.size(); i++) {
        std::cout << "(" << map[i].hue << "," << map[i].saturation << "," << map[i].value << "), ";
    }
    std::cout << '\n';
}

Colormap::Colormap(const Colormap& orig) {
}

Colormap::~Colormap() {
}

HSV Colormap::interpolate(int x, int x0, int x1) {
    HSV interpolated;

    HSV y0 = map[x0];
    HSV y1 = map[x1];

    interpolated.hue = y0.hue + (y1.hue - y0.hue) * ((x - x0) / (x1 - x0));
    interpolated.saturation = y0.saturation + (y1.saturation - y0.saturation) * ((x - x0) / (x1 - x0));
    interpolated.value = y0.value + (y1.value - y0.value) * ((x - x0) / (x1 - x0));

    return interpolated;
}

void Colormap::computeColors() {
    for (unsigned i = 0; i < map.size(); i++) {
        
    }
}