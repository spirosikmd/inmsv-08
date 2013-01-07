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
    scalar_draw_mode = Density;
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

void Visualization::set_scalar_draw_mode(ScalarDrawMode sdm)
{
    scalar_draw_mode = sdm;
}

void Visualization::set_vector_draw_mode(VectorDrawMode vdm)
{
    vector_draw_mode = vdm;
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
    
    f = sqrt(pow(x, 2) + pow(y, 2));
    f = round(f*(N-1))/(N-1);
        
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
//        draw_velocities(simulation, DIM, wn, hn);
        draw_glyphs(simulation, DIM, wn, hn);
    }
    if (options[DrawForces])
    {
        draw_forces(simulation, DIM, wn, hn);
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
        
        set_colormap(pick_scalar_field_value(simulation, idx));
        glVertex2f(px,py);

        for (i = 0; i < DIM - 1; i++)
        {
            px = wn + (fftw_real)i * wn;
            py = hn + (fftw_real)(j + 1) * hn;
            idx = ((j + 1) * DIM) + i;
            set_colormap(pick_scalar_field_value(simulation, idx));
            glVertex2f(px, py);
            px = wn + (fftw_real)(i + 1) * wn;
            py = hn + (fftw_real)j * hn;
            idx = (j * DIM) + (i + 1);
            set_colormap(pick_scalar_field_value(simulation, idx));
            glVertex2f(px, py);
        }

        px = wn + (fftw_real)(DIM - 1) * wn;
        py = hn + (fftw_real)(j + 1) * hn;
        idx = ((j + 1) * DIM) + (DIM - 1);
        set_colormap(pick_scalar_field_value(simulation, idx));
        glVertex2f(px, py);
        glEnd();
    }
}

float Visualization::pick_scalar_field_value(Simulation const &simulation, size_t idx)
{
    float value = 0.0;
    
    switch(scalar_draw_mode)
    {
        case VelocityMagnitude:
        {
            value = magnitude(simulation.vx[idx], simulation.vy[idx]);
        }
        break;
        case ForceMagnitude:
        {
            value = magnitude(simulation.fx[idx], simulation.fy[idx]);
        }
        break;
        case Density: { value = simulation.rho[idx]; }
        default: break;
    }
    return value;
}

void Visualization::draw_velocities(Simulation const &simulation, const int DIM, fftw_real wn, fftw_real hn)
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

void Visualization::draw_forces(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn)
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

void Visualization::draw_glyphs(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn)
{
    int i, j, idx;
    float magn;
    float *values = new float[2];
    
    for (i = 0; i < DIM; i++)
        for (j = 0; j < DIM; j++)
        {
            idx = (j * DIM) + i;
            pick_vector_field_value(simulation, idx, values);
            magn = magnitude(values);
            magn = pick_scaled_field(magn);
            GLfloat x_start = wn + (fftw_real)i * wn;
            GLfloat y_start = hn + (fftw_real)j * hn;
            GLfloat x = values[0];
            GLfloat y = values[1];
            GLfloat angle = 0.0;
            angle = atan2(y, x) * 180 / M_PI;
            glPushMatrix();
            glTranslatef(x_start, y_start, 0.0);
            glRotatef(angle, 0.0, 0.0, 1.0f);
            glTranslatef(-x_start, -y_start, 0.0);
            glBegin(GL_POLYGON);
            set_colormap(pick_scalar_field_value(simulation, idx));
            glVertex2f(x_start, y_start + 1);
            glVertex2f(x_start + magn, y_start + 1);
            glVertex2f(x_start + magn, y_start + 2);
            glVertex2f(x_start + magn + 3, y_start);
            glVertex2f(x_start + magn, y_start - 2);
            glVertex2f(x_start + magn, y_start - 1);
            glVertex2f(x_start, y_start - 1);
            glEnd();
            glPopMatrix();
        }
}

void Visualization::pick_vector_field_value(Simulation const &simulation, size_t idx, float values[])
{
    switch(vector_draw_mode)
    {
        case Force:
        {
            values[0] = simulation.fx[idx];
            values[1] = simulation.fy[idx];
        }
        break;
        case Velocity:
        {
            values[0] = simulation.vx[idx];
            values[1] = simulation.vy[idx];
        }
        default: break;
    }
}

float Visualization::pick_scaled_field(float v)
{
    float value = 0.0;
    
    switch (vector_draw_mode)
    {
        case Force:
        {
            value = scale(v, 0, 1, 0, vec_scale);
        }
        break;
        case Velocity:
        {
            value = scale(v, 0, 1, 0, vec_scale);
        }
        default: {} break;
    }
    return value;
}