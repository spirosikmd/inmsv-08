/* 
 * File:   Colormap.h
 * Author: cm
 *
 * Created on January 9, 2013, 7:53 PM
 */

#include <vector>

#ifndef COLORMAP_H
#define	COLORMAP_H

enum Limitation {
    SCALING,
    CLAMPING
};

enum NumberOfColors {
    COL_2 = 2,
    COL_4 = 4,
    COL_8 = 8,
    COL_16 = 16,
    COL_32 = 32,
    COL_64 = 64,
    COL_128 = 128,
    COL_256 = 256,
};

struct HSV {
public:
    float hue;
    float saturation;
    float value;
    HSV() : hue(0.0), saturation(0.0), value(0.0) {
    }
    HSV(float h, float s, float v) : hue(h), saturation(s), value(v) {
    }
};



static const struct HSV BLACK(0, 0, 0);
static const struct HSV WHITE(0, 1.0, 1.0);

class Colormap {
public:
    Colormap();
    Colormap(const Colormap& orig);
    virtual ~Colormap();
    void putColor(HSV color, unsigned int position);

private:
    float hue;
    float saturation;
    NumberOfColors numberOfColors;
    Limitation limitation;
    float clampMin, clampMax;
    HSV colors[256];
    std::vector<HSV> map;
    HSV interpolate(int at, int left, int right);
    void computeColors();
};

#endif	/* COLORMAP_H */

