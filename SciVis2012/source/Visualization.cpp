#include <algorithm>
#include <cmath>
#include <GLUT/glut.h>
#include <iostream>

#include "Visualization.h"
#include "Simulation.h"
#include "Utilities.h"

using namespace std;

Visualization::Visualization()
{
    vec_scale = 1000;
    scalar_col = Grayscale;
    N = 256;
    draw_mode = Velocity;
    options[UseDirectionColoring] = false;              // not used for now
    options[DrawSmoke] = true;
    options[DrawForces] = false;
    options[DrawVelocities] = false;
    options[DrawVectorField] = true;                    // not used for now
}

void Visualization::set_scalar_col(ColorMode colorMode)
{
    scalar_col = colorMode;
}

void Visualization::toggle(Option option)
{
    options[option] = !options[option];
}

void Visualization::enable(Option option)
{
    options[option] = true;
}

void Visualization::disable(Option option)
{
    options[option] = false;
}

bool Visualization::is_enabled(Option option) const
{
    return options[option];
}

void Visualization::scale_hedgehogs(float scale)
{
    vec_scale *= scale;
}

float Visualization::hedgehog_scale() const
{
    return vec_scale;
}

void Visualization::set_hue(const float h)
{
    hue = h;
}

void Visualization::set_saturation(const float s)
{
    saturation = s;
}

void Visualization::set_num_of_colors(const int n)
{
    N = n;
}

void Visualization::set_draw_mode(DrawMode dm)
{
    draw_mode = dm;
}

//rainbow: Implements a color palette, mapping the scalar 'value' to a rainbow color RGB
void Visualization::rainbow(float value, float* R, float* G, float* B)
{
    const float dx=0.8;
    if (value<0) value=0; if (value>1) value=1;
    value = (6-2*dx)*value+dx;	
    *R = max(0.0, (3-fabs(value-4)-fabs(value-5)) / 2.0);
    *G = max(0.0, (4-fabs(value-2)-fabs(value-4)) / 2.0);
    *B = max(0.0, (3-fabs(value-1)-fabs(value-2)) / 2.0);
}

void Visualization::grayscale(float value, float* R, float* G, float* B)
{
    if (value<0) value=0; if (value>1) value=1;
    *R = *G = *B = value;
}

void Visualization::custom(float value, float* R, float* G, float* B)
{
    if (value<0) value=0; if (value>1) value=1;
    *R = 1;
    *G = *B = value;
}

//set_colormap: Sets three different types of colormaps
void Visualization::set_colormap(float vy)
{
    float R,G,B;
    
    vy = round(vy*(N-1))/(N-1);

    if (scalar_col == Grayscale)
        grayscale(vy, &R, &G,&B);
    else if (scalar_col == Rainbow)
        rainbow(vy, &R, &G,&B);
    else if (scalar_col == Custom)
        hsv2rgb(hue, saturation, vy, R, G, B);
 
    glColor3f(R,G,B);
}

//direction_to_color: Set the current color by mapping a direction vector (x,y), using
//                    the color mapping method 'method'. If method==1, map the vector direction
//                    using a rainbow colormap. If method==0, simply use the white color
void Visualization::direction_to_color(float x, float y)
{
    float r,g,b,f;
    switch(scalar_col)
    {
        case Grayscale:
        {
            f = atan2(y,x) / 3.1415927 + 1;
            r = g = b = f;
        }
        break;
        case Custom:
        { 
            hsv2rgb(hue, saturation, f, r, g, b);
        }
        break;
        case Rainbow:
        default:
        {
            f = atan2(y,x) / 3.1415927 + 1;
            r = f;
            if(r > 1) r = 2 - r;
            g = f + .66667;
            if(g > 2) g -= 2;
            if(g > 1) g = 2 - g;
            b = f + 2 * .66667;
            if(b > 2) b -= 2;
            if(b > 1) b = 2 - b;
        }
        break;
    }
    glColor3f(r,g,b);
}

void Visualization::magnitude_to_color(float x, float y)
{
    float r,g,b,f;
//    static float max_f = -1000.0;
    
    f = sqrt(pow(x, 2) + pow(y, 2));
    f = round(f*(N-1))/(N-1);
//    if (f > max_f) { cout << max_f << '\n'; max_f = f; }
        
    switch(scalar_col)
    {
        case Grayscale:
        {
            r = g = b = f;
        }
        break;
        case Custom:
        { 
            hsv2rgb(hue, saturation, f, r, g, b);
        }
        break;
        case Rainbow:
        default:
        {
            r = f;
            if(r > 1) r = 2 - r;
            g = f + .66667;
            if(g > 2) g -= 2;
            if(g > 1) g = 2 - g;
            b = f + 2 * .66667;
            if(b > 2) b -= 2;
            if(b > 1) b = 2 - b;
        }
        break;
    }
    
    glColor3f(r,g,b);
}
		
//visualize: This is the main visualization function
void Visualization::visualize(Simulation const &simulation, int winWidth, int winHeight)
{
    const int DIM = Simulation::DIM;
    fftw_real  wn = (fftw_real)winWidth / (fftw_real)(DIM + 1);   // Grid cell width
    fftw_real  hn = (fftw_real)winHeight / (fftw_real)(DIM + 1);  // Grid cell heigh

    if (options[DrawSmoke])
    {
        draw_smoke(simulation, DIM, wn, hn);
    }
    if (options[DrawVelocities])
    {
        drawVelocities(simulation, DIM, wn, hn);
    }
    if (options[DrawForces])
    {
        drawForces(simulation, DIM, wn, hn);
    }
}

void Visualization::draw_smoke(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn)
{
    int i, j, idx;
    double px,py;
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    for (j = 0; j < DIM - 1; j++)			//draw smoke
    {
        glBegin(GL_TRIANGLE_STRIP);

        i = 0;
        px = wn + (fftw_real)i * wn;
        py = hn + (fftw_real)j * hn;
        idx = (j * DIM) + i;

        glColor3f(0.0, 0.0, 0.0);
        glVertex2f(px,py);

        for (i = 0; i < DIM - 1; i++)
        {
            px = wn + (fftw_real)i * wn;
            py = hn + (fftw_real)(j + 1) * hn;
            idx = ((j + 1) * DIM) + i;
            set_colormap(pick_value(simulation, idx));
            glVertex2f(px, py);
            px = wn + (fftw_real)(i + 1) * wn;
            py = hn + (fftw_real)j * hn;
            idx = (j * DIM) + (i + 1);
            set_colormap(pick_value(simulation, idx));
            glVertex2f(px, py);
        }

        px = wn + (fftw_real)(DIM - 1) * wn;
        py = hn + (fftw_real)(j + 1) * hn;
        idx = ((j + 1) * DIM) + (DIM - 1);
        set_colormap(pick_value(simulation, idx));
        glVertex2f(px, py);
        glEnd();
    }
}

float Visualization::pick_value(Simulation const &simulation, size_t idx)
{
    float value = 0.0;
    static float max_f = 0.0;
    switch(draw_mode)
    {
        case Velocity:
        {
            value = sqrt(pow(simulation.vx[idx], 2) + pow(simulation.vy[idx], 2));
            value = scale(value, 0, 0.02, 0, 1);
        }
        break;
        case Force:
        {
            value = sqrt(pow(simulation.fx[idx], 2) + pow(simulation.fy[idx], 2));
            value = scale(value, 0, 0.2, 0, 1);
        }
        break;
        case Density: { value = simulation.rho[idx]; }
        default: break;
    }
    if (value > max_f) { cout << max_f << '\n'; max_f = value; }
    return value;
}

void Visualization::drawVelocities(Simulation const &simulation, const int DIM, fftw_real wn, fftw_real hn)
{
    int i, j, idx;
    
    glBegin(GL_LINES);				//draw velocities
    for (i = 0; i < DIM; i++)
        for (j = 0; j < DIM; j++)
        {
            idx = (j * DIM) + i;
            magnitude_to_color(simulation.vx[idx], simulation.vy[idx]);
            glVertex2f(wn + (fftw_real)i * wn, hn + (fftw_real)j * hn);
            glVertex2f((wn + (fftw_real)i * wn) + vec_scale * simulation.vx[idx], (hn + (fftw_real)j * hn) + vec_scale * simulation.vy[idx]);
        }
    glEnd();
}

void Visualization::drawForces(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn)
{
    int i, j, idx;
    
    glBegin(GL_LINES);				//draw forces
    for (i = 0; i < DIM; i++)
        for (j = 0; j < DIM; j++)
        {
            idx = (j * DIM) + i;
            magnitude_to_color(simulation.fx[idx], simulation.fy[idx]);
            glVertex2f(wn + (fftw_real)i * wn, hn + (fftw_real)j * hn);
            glVertex2f((wn + (fftw_real)i * wn) + vec_scale * simulation.fx[idx], (hn + (fftw_real)j * hn) + vec_scale * simulation.fy[idx]);
        }
    glEnd();
}