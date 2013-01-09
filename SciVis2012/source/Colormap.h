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

    HSV() : hue(-1), saturation(-1), value(-1) {
    }

    HSV(float h, float s, float v) : hue(h), saturation(s), value(v) {
    }

    bool operator==(const HSV & other) {
        return (hue == other.hue && saturation == other.saturation && value == other.value);
    }

    bool operator!=(const HSV & other) {
        return (hue != other.hue || saturation != other.saturation || value != other.value);
    }
};



static const struct HSV BLACK(0, 0, 0);
static const struct HSV WHITE(0, 1.0, 1.0);
static const struct HSV NULLHSV(-1, -1, -1);

class Colormap {
public:
    Colormap();
    Colormap(const Colormap& orig);
    virtual ~Colormap();
    void putColor(HSV color, unsigned int position);
    void printColors();
private:
    float hue;
    float saturation;
    NumberOfColors numberOfColors;
    Limitation limitation;
    float clampMin, clampMax;
    HSV colors[256];
    std::vector<HSV> map;
    HSV interpolate(float at, float left, float right);
    void computeColors();

};

#endif	/* COLORMAP_H */

