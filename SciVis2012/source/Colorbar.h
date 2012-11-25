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
    
    Colorbar();
    virtual ~Colorbar();
    
    void setup();
    void render();
    
    void setTitle(std::string title);
    
    void setColorMode(Visualization::ColorMode colorMode);
    Visualization::ColorMode *getColorMode();
    
    void fillData(GLubyte (&data)[256][3]);
    
protected:
    Visualization::ColorMode colorMode;
    GLuint texture;
    std::string title;
};

#endif	/* COLORBAR_H */

