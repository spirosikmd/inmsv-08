/* 
 * File:   Colormap.h
 * Author: cm
 *
 * Created on January 9, 2013, 7:53 PM
 */

#include <vector>
#include "Utilities.h"
#include <OpenGL/gl.h>

#ifndef COLORMAP_H
#define	COLORMAP_H

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

struct RGB {
public:
    float red;
    float green;
    float blue;

    RGB() : red(-1), green(-1), blue(-1) {
    }

    RGB(float r, float g, float b) : red(r), green(g), blue(b) {
    }

    bool operator==(const RGB & other) {
        return (red == other.red && green == other.green && blue == other.blue);
    }

    bool operator!=(const RGB & other) {
        return (red != other.red || green != other.green || blue != other.blue);
    }
};

static const struct RGB NULLRGB;
static const struct HSV NULLHSV;
static const struct RGB BLACK(0, 0, 0);
static const struct RGB WHITE(1, 1, 1);
static const struct RGB RED(1, 0, 0);
static const struct RGB GREEN(0, 1, 0);
static const struct RGB BLUE(0, 0, 1);
static const struct RGB YELLOW(1, 1, 0);
static const struct RGB MAGENTA(1, 0, 1);
static const struct RGB AQUA(0, 1, 1);

//static const struct RGB MAGENTA((1.0 / 360.0) * 330.0, 1, 1);
//static const struct HSV PINK((1.0 / 360.0) * 300.0, 1, 1);
//static const struct HSV PURPLE((1.0 / 360.0) * 270.0, 1, 1);
//static const struct HSV BLUE((1.0 / 360.0) * 240.0, 1, 1);
//static const struct HSV CYAN((1.0 / 360.0) * 180.0, 1, 1);
//static const struct HSV SEAFOAM((1.0 / 360.0) * 150.0, 1, 1);
//static const struct HSV GREEN((1.0 / 360.0) * 120.0, 1, 1);
//static const struct HSV YELLOW((1.0 / 360.0) * 60.0, 1, 1);
//static const struct HSV ORANGE((1.0 / 360.0) * 30.0, 1, 1);
//static const struct HSV RED(0, 1, 1);

class Colormap {
public:

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

    Colormap();
    Colormap(const Colormap& orig);
    virtual ~Colormap();
    void putColor(RGB color, unsigned int position);
    void printColors();
    void render(float min, float max, int ticks);
    void setHue(float);
    float getHue();
    void setSaturation(float);
    float getSaturation();
    void setNumberOfColors(int);
    int getNumberOfColors();
    void loadColormapTexture();
    RGB getColorAt(int);

private:

    float hue;
    float saturation;
    int numberOfColors;
    RGB colors[256];
    std::vector<RGB> map;
    GLuint texture;

    RGB interpolate(float at, float left, float right);
    void computeTexture();
    void computeColors();
};

#endif	/* COLORMAP_H */

