#include "Colorbar.h"
#include "Application.h"

using namespace std;

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
        colormap(i*step);
        glVertex3i(i*step, colorbarHeight, 0);              // Top Left
        glVertex3i((i*step)+step, colorbarHeight, 0);       // Top Right
        glVertex3i((i*step)+step, 0, 0);                    // Bottom Right
        glVertex3i(i*step, 0, 0);                           // Bottom Left
    }
    glEnd();
}

void Colorbar::colormap(float value)
{
    float R, G, B;
    switch(colorMode)
    {
        case Visualization::Grayscale:
        {
            R = G = B = value/256.0;
        }
        break;
        case Visualization::Rainbow:
        {
            rainbow(value, &R, &G, &B);
        }
        break;
        case Visualization::Custom:
        {
            R = 1;
            G = B = value/256.0;
        }
        break;
        default: {} break;
    }
    glColor3f(R, G, B);
}

void Colorbar::rainbow(float value, float* R, float* G, float* B)
{
    const float dx=0.8;
    value = value/256;
    if (value<0) value=0; if (value>1) value=1;
    value = (6-2*dx)*value+dx;	
    *R = max(0.0, (3-fabs(value-4)-fabs(value-5)) / 2.0);
    *G = max(0.0, (4-fabs(value-2)-fabs(value-4)) / 2.0);
    *B = max(0.0, (3-fabs(value-1)-fabs(value-2)) / 2.0);
}

void Colorbar::setColorMode(Visualization::ColorMode colormode)
{
    colorMode = colormode;
}

void Colorbar::setN(size_t n)
{
    N = n;
}