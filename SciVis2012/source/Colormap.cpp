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
    map[0] = BLACK;
    map[255] = WHITE;
    computeColors();
}

void Colormap::putColor(HSV color, unsigned int position) {
    map[position] = color;
    computeColors();
    for (unsigned i = 0; i < map.size(); i++) {
        if (map[i] != NULLHSV) {
            std::cout << "[" << i << "](" << map[i].hue << "," << map[i].saturation << "," << map[i].value << "), ";
        }
    }
    std::cout << '\n';
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