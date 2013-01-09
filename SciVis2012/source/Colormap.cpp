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
        if (map[i] != NULLHSV) {
            std::cout << "["<<i<<"](" << map[i].hue << "," << map[i].saturation << "," << map[i].value << "), ";
        }
    }
    std::cout << '\n';
}

Colormap::Colormap(const Colormap& orig) {
}

Colormap::~Colormap() {
}


