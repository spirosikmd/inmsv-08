#include "Colorbar.h"
#include "Application.h"
#include "Utilities.h"

using namespace std;

Colorbar::Colorbar()
{
    colorMode = Visualization::Grayscale;
    title = "Value";
    N = 8;
}

Colorbar::~Colorbar() {}

void Colorbar::render()
{
    glTranslatef(730.0f, 20.0f, 0.0f);
    
    size_t colorbarLength = 20;
    size_t colorbarHeight = 256;
    float step = colorbarHeight/N;
    
    // draw colorbar
    glBegin(GL_QUADS);
    for (size_t i = 0; i != N; i++)
    {
        put_color(i*step);
        glVertex3i(0, (i*step)+step, 0);              // Top Left
        glVertex3i(colorbarLength, (i*step)+step, 0);       // Top Right
        glVertex3i(colorbarLength, i*step, 0);                    // Bottom Right
        glVertex3i(0, i*step, 0);                           // Bottom Left
    }
    glEnd();
    
    // draw border
    glColor3f(1, 1, 1);
    glBegin(GL_LINES);
    glVertex2f(0, 0);
    glVertex2f(0, 256);
    glVertex2f(0, 256);
    glVertex2f(20, 256);
    glVertex2f(20, 256);
    glVertex2f(20, 0);
    glVertex2f(20, 0);
    glVertex2f(0, 0);
    glEnd();
    
    // draw legend text
    glColor3f(1, 1, 1);
    print_text(755, 15, "-0.23");
    print_text(755, 84, "-0.11");
    print_text(755, 148, "0.00");
    print_text(755, 216, "0.11");
    print_text(755, 266, "0.23");
    
    // draw title
    glColor3f(1, 1, 1);
    print_text(730, 285, title);
}

void Colorbar::print_text(int x, int y, string text)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 1000, 0, 800, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glPushAttrib(GL_DEPTH_TEST);
    glDisable(GL_DEPTH_TEST);
    glRasterPos2i(x,y);
    for (size_t i=0; i!=text.size(); i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, text[i]);
    }
    glPopAttrib();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void Colorbar::put_color(float value)
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
            value = value/256.0;
            rainbow(value, &R, &G, &B);
        }
        break;
        case Visualization::Custom:
        {
            value = value/256.0;
            hsv2rgb(hue, saturation, value, R, G, B);
        }
        break;
        default: {} break;
    }
    glColor3f(R, G, B);
}

void Colorbar::rainbow(float value, float* R, float* G, float* B)
{
    const float dx=0.8;
    if (value<0) value=0; if (value>1) value=1;
    value = (6-2*dx)*value+dx;	
    *R = max(0.0, (3-fabs(value-4)-fabs(value-5)) / 2.0);
    *G = max(0.0, (4-fabs(value-2)-fabs(value-4)) / 2.0);
    *B = max(0.0, (3-fabs(value-1)-fabs(value-2)) / 2.0);
}

void Colorbar::set_color_mode(Visualization::ColorMode colormode)
{
    colorMode = colormode;
}

void Colorbar::set_N(size_t n)
{
    N = n;
}

void Colorbar::set_title(std::string t)
{
    title = t;
}

void Colorbar::set_hue(float h)
{
    hue = h;
}

void Colorbar::set_saturation(float s)
{
    saturation = s;
}