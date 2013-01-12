#ifndef VISUALIZATION_H_INCLUDED
#define	VISUALIZATION_H_INCLUDED

#include <rfftw.h>
#include <map>
#include "Colormap.h"              //the numerical simulation FFTW library

class Simulation;

class Visualization {
    friend class Application;

public:

    Visualization();

    //different types of color mapping: black-and-white, rainbow, banded

    enum ColorMode {
        GRADIENT,
        RAINBOW,
        ZEBRA,
        ColorModeCount // (automatically assigned)
    };

    enum Mode {
        SCALING,
        CLAMPING
    };

    enum Option {
        DrawSmoke, // draw the smoke or not
        DrawVectorField, // draw the vector field or not (not used for now)
        UseDirectionColoring, // use direction color-coding or not (not used for now)
        DrawGlyphs, // draw the velocities or not 
        DrawForces, // draw forces or nor
        OptionsCount // (automatically assigned)
    };

    enum ScalarDrawMode {
        Density, VelocityMagnitude, ForceMagnitude
    };

    enum VectorDrawMode {
        Velocity, Force
    };

    void visualize(Simulation const &simulation, int winWidth, int winHeight);
    //void set_scalar_col(ColorMode colorMode);
    void toggle(Option option);
    void enable(Option option);
    void disable(Option option);
    bool is_enabled(Option option) const;
    void scale_hedgehogs(float scale);
    float hedgehog_scale() const;
    void set_hue(const float h);
    void set_saturation(const float s);
    void set_num_of_colors(const int n);
    //
    //    void rainbow(float value, float* R, float* G, float* B);
    //    void grayscale(float value, float* R, float* G, float* B);
    //    void custom(float value, float* R, float* G, float* B);
    Colormap* getColormap();
    Colormap* loadColormap(ColorMode);
    void setColor(float vy);
    void direction_to_color(float x, float y);
    void magnitude_to_color(float x, float y);
    void set_scalar_draw_mode(ScalarDrawMode sdm);
    void set_vector_draw_mode(VectorDrawMode vdm);
    void set_sample_x(int x);
    void set_sample_y(int y);

private:

    void initializeColormaps();
    std::map<ColorMode, Colormap*> colormaps;
    
    float vec_scale; // scaling of hedgehogs 
    int options[OptionsCount]; // options boolean array
    Colormap* colormap;
    float hue, saturation;
    ScalarDrawMode scalar_draw_mode;
    VectorDrawMode vector_draw_mode;
    int sample_x, sample_y;

    void draw_smoke(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn);
    void draw_velocities(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn);
    void draw_forces(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn);
    float pick_scalar_field_value(Simulation const &simulation, size_t idx);
    void pick_vector_field_value(Simulation const &simulation, size_t idx, float values[]);
    void draw_glyphs(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn, const fftw_real wn_sample, const fftw_real hn_sample);
    float pick_scaled_field(float v);
};

#endif