/* 
 * File:   colorbar.h
 * Author: spyros
 *
 * Created on November 22, 2012, 9:38 AM
 */

#ifndef COLORBAR_H
#define	COLORBAR_H

#include <iostream>
#include <OpenGL/gl.h>
#include "Visualization.h"

class Colorbar
{       
public:
    
    enum NUMCOLORS
    {
        COL_2 = 2,
        COL_4 = 4,
        COL_8 = 8,
        COL_16 = 16,
        COL_32 = 32,
        COL_64 = 64,
        COL_128 = 128,
        COL_256 = 256,
    };
    
    Colorbar();
    virtual ~Colorbar();
    
    void render();
    
    void set_color_mode(Visualization::ColorMode colorMode);
    Visualization::ColorMode *get_color_mode();
    
    void fill_data(GLubyte (&data)[8][3]);
    void set_N(size_t n);
    void set_title(std::string title);
    void set_hue(float h);
    void set_saturation(float s);
    void put_color(float value);
    void rainbow(float value, float* R, float* G, float* B);
    
protected:
    Visualization::ColorMode colorMode;
    std::string title;
    size_t N;
    float hue, saturation;
    
private:
    
    void print_text(int x, int y, std::string text);
};

#endif	/* COLORBAR_H */

