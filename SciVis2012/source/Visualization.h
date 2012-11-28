#ifndef VISUALIZATION_H_INCLUDED
#define	VISUALIZATION_H_INCLUDED

#include <rfftw.h>              //the numerical simulation FFTW library

class Simulation;

class Visualization
{
    friend class Application;

public:

    Visualization();

    //different types of color mapping: black-and-white, rainbow, banded
    enum ColorMode
    {
        Grayscale,
        Rainbow,
        Custom,
        ColorModeCount		// (automatically assigned)
    };

    enum Option
    {
        DrawSmoke,                  // draw the smoke or not
        DrawVectorField,		// draw the vector field or not (not used for now)
        UseDirectionColoring,	// use direction color-coding or not (not used for now)
        DrawVelocities,             // draw the velocities or not 
        DrawForces,                 // draw forces or nor
        OptionsCount		// (automatically assigned)
    };

    enum ApplicationMode
    {
        Scale, Clamp
    };

    void visualize(Simulation const &simulation, int winWidth, int winHeight);
    void set_scalar_col(ColorMode colorMode);
    void toggle(Option option);
    void enable(Option option);
    void disable(Option option);
    bool is_enabled(Option option) const;		
    void scale_hedgehogs(float scale);
    float hedgehog_scale() const;
    void set_hue(const float h);
    void set_saturation(const float s);
    void set_num_of_colors(const int n);

    void rainbow(float value, float* R, float* G, float* B);
    void grayscale(float value, float* R, float* G, float* B);
    void custom(float value, float* R, float* G, float* B);
    void set_colormap(float vy);
    void direction_to_color(float x, float y);
    void magnitude_to_color(float x, float y);

private:

    float vec_scale;		// scaling of hedgehogs 
    int options[OptionsCount];	// options boolean array
    ColorMode scalar_col;		//method for scalar coloring
    float hue, saturation;
    int N;                          // number of colors

    void drawSmoke(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn);
    void drawVelocities(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn);
    void drawForces(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn);
};

#endif