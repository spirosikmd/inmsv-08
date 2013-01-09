#include "Colorbar.h"
#include "Application.h"

using namespace std;

Colorbar::Colorbar(int x, int y, int z, size_t w, size_t h)
{
    colorMode = Visualization::Grayscale;
    title = "Value";
    N = 256;
    pos_x = x;
    pos_y = y;
    pos_z = z;
    width = w;
    height = h;
}

Colorbar::~Colorbar() {}

void Colorbar::render()
{
    glTranslatef(pos_x, pos_y, pos_z);
    
    float step = height/N;
    
    // draw colorbar
    glBegin(GL_QUADS);
    for (size_t i = 0; i != N; i++)
    {
        put_color(i*step+step);
        glVertex3i(0, (i*step)+step, 0);              // Top Left
        glVertex3i(width, (i*step)+step, 0);       // Top Right
        put_color(i*step);
        glVertex3i(width, i*step, 0);                    // Bottom Right
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
    print_text(pos_x+width+5, 15, "-0.23");
    print_text(pos_x+width+5, 84, "-0.11");
    print_text(pos_x+width+5, 148, "0.00");
    print_text(pos_x+width+5, 216, "0.11");
    print_text(pos_x+width+5, 266, "0.23");
    
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

void Colorbar::set_title(string t)
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