#include <algorithm>
#include <cmath>
#include <rfftw.h>              //the numerical simulation FFTW library
#include <GLUT/glut.h>

#include "Visualization.h"
#include "Simulation.h"
#include "Utilities.h"

using namespace std;

Visualization::Visualization()
{
	vec_scale = 1000;
	scalar_col = Rainbow;
	options[UseDirectionColoring] = false;
	options[DrawSmoke] = false;
	options[DrawVectorField] = true;	
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

//set_colormap: Sets three different types of colormaps
void Visualization::set_colormap(float vy)
{
    float R,G,B;

    if (scalar_col == Grayscale)
        R = G = B = vy;
    else if (scalar_col == Rainbow)
        rainbow(vy, &R, &G,&B);
    else if (scalar_col == Custom)
    {
       const int NLEVELS = 7;
       vy *= NLEVELS; vy = (int)(vy); vy/= NLEVELS;
       rainbow(vy,&R,&G,&B);
    }
    
    glColor3f(R,G,B);
}

//direction_to_color: Set the current color by mapping a direction vector (x,y), using
//                    the color mapping method 'method'. If method==1, map the vector direction
//                    using a rainbow colormap. If method==0, simply use the white color
void Visualization::direction_to_color(float x, float y, int method)
{
	float r,g,b,f;
	if (method)
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
	else
	{ r = g = b = 1; }
	glColor3f(r,g,b);
}

		
//visualize: This is the main visualization function
void Visualization::visualize(Simulation const &simulation, int winWidth, int winHeight)
{
	int        i, j, idx; double px,py;
	const int DIM = Simulation::DIM;
	fftw_real  wn = (fftw_real)winWidth / (fftw_real)(DIM + 1);   // Grid cell width
	fftw_real  hn = (fftw_real)winHeight / (fftw_real)(DIM + 1);  // Grid cell heigh
		
	if (options[DrawSmoke])
	{
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
			set_colormap(simulation.rho[idx]);
			glVertex2f(px, py);
			px = wn + (fftw_real)(i + 1) * wn;
			py = hn + (fftw_real)j * hn;
			idx = (j * DIM) + (i + 1);
			set_colormap(simulation.rho[idx]);
			glVertex2f(px, py);
		}

		px = wn + (fftw_real)(DIM - 1) * wn;
		py = hn + (fftw_real)(j + 1) * hn;
		idx = ((j + 1) * DIM) + (DIM - 1);
		set_colormap(simulation.rho[idx]);
		glVertex2f(px, py);
		glEnd();
	}
	}

	if (options[DrawVectorField])
	{
	  glBegin(GL_LINES);				//draw velocities
	  for (i = 0; i < DIM; i++)
	    for (j = 0; j < DIM; j++)
	    {
		  idx = (j * DIM) + i;
		  direction_to_color(simulation.vx[idx], simulation.vy[idx], options[UseDirectionColoring]);
		  glVertex2f(wn + (fftw_real)i * wn, hn + (fftw_real)j * hn);
		  glVertex2f((wn + (fftw_real)i * wn) + vec_scale * simulation.vx[idx], (hn + (fftw_real)j * hn) + vec_scale * simulation.vy[idx]);
	    }
	  glEnd();
	}
}


