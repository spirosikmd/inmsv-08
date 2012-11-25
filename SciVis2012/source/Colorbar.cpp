#include "Colorbar.h"
#include "Application.h"

Colorbar::Colorbar()
{
    colorMode = Visualization::Grayscale;
    texture = 0;
    title = "Colorbar";
}

Colorbar::~Colorbar()
{
    if (texture)
        glDeleteTextures(1, &texture);
}

void Colorbar::render()
{
    glTranslatef(500.0f, 100.0f, 0.0f);
    
    size_t colorbarLength = 256;
    size_t colorbarHeight = 20;
    float step = colorbarLength/N;
    
    glBegin(GL_QUADS);
    for (size_t i = 0; i != N; i++)
    {
        float R, G, B;
        colormap(i*step, &R, &G, &B);
        glColor3f(R, G, B);
        glVertex3i(i*step, colorbarHeight, 0);              // Top Left
        glVertex3i((i*step)+step, colorbarHeight, 0);       // Top Right
        glVertex3i((i*step)+step, 0, 0);                    // Bottom Right
        glVertex3i(i*step, 0, 0);                           // Bottom Left
    }
    glEnd();
}

void Colorbar::colormap(float value, float* R, float* G, float* B)
{
    switch(colorMode)
    {
        case Visualization::Grayscale:
        {
            *R = value/256.0;
            *G = value/256.0;
            *B = value/256.0;
        }
        break;
        case Visualization::Rainbow:
        {
            
        }
        break;
        case Visualization::Custom:
        {
            *R = 256;
            *G = value/256.0;
            *B = value/256.0;
        }
        break;
        default: {} break;
    }
}

void Colorbar::setColorMode(Visualization::ColorMode colormode)
{
    colorMode = colormode;
}

void Colorbar::setN(size_t n)
{
    N = n;
}