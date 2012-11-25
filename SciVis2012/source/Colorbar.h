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
    
    void setTitle(std::string title);
    
    void setColorMode(Visualization::ColorMode colorMode);
    Visualization::ColorMode *getColorMode();
    
    void fillData(GLubyte (&data)[8][3]);
    
    void setN(size_t n);
    
    void colormap(float value);
    
    void rainbow(float value, float* R, float* G, float* B);
    
    void printtext(int x, int y, std::string text);
    
protected:
    Visualization::ColorMode colorMode;
    std::string title;
    size_t N;
};

#endif	/* COLORBAR_H */

