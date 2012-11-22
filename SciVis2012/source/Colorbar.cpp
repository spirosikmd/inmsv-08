#include "Colorbar.h"
#include <OpenGL/gl.h>

Colorbar::Colorbar()
{
    colorMode = Visualization::Rainbow;
}

Colorbar::~Colorbar()
{
    if (texture)
        glDeleteTextures(1, texture);
}

void Colorbar::setup()
{
    
}

void Colorbar::render()
{
    
}