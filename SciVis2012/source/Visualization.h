#ifndef VISUALIZATION_H_INCLUDED
#define	VISUALIZATION_H_INCLUDED

#include <rfftw.h>
#include <map>
#include "Colormap.h"              //the numerical simulation FFTW library
#include <map>

class Simulation;

class Visualization {
    friend class Application;

public:

    Visualization();

    //different types of color mapping: black-and-white, rainbow, banded

    enum ColorMode {
        GRAYSCALE,
        BGRADIENT,
        WGRADIENT,
        BLUEYELLOWGREEN,
        RAINBOW,
        HEATMAP,
        ZEBRA,
        ColorModeCount // (automatically assigned)
    };

    enum Mode {
        SCALING,
        CLAMPING
    };

    enum ColorType {
        TEXTURE,
        SIMPLE // with this I mean that we assign color with glColor3f()
    };

    enum Option {
        DrawSmoke, // draw the smoke or not
        DrawVectorField, // draw the vector field or not (not used for now)
        UseDirectionColoring, // use direction color-coding or not (not used for now)
        DrawGlyphs, // draw the velocities or not 
        OptionsCount // (automatically assigned)
    };

    enum DatasetType {
        DENSITY,
        VELOCITY_MAGN,
        FORCE_MAGN,
        VELOCITY,
        FORCE,
        NONE
    };

    enum GlyphType {
        SIMPLE_ARROWS,
        CONES
    };

    struct Dataset {
        float max;
        float min;
        Mode mode;
        float scaleMin;
        float scaleMax;

        Dataset() {
            max = 0;
            min = 0;
            mode = SCALING;
        }

        Dataset(float mn, float mx, Mode md) {
            max = mx;
            min = mn;
            mode = md;
            scaleMin = INFINITY;
            scaleMax = -INFINITY;
        }
    };

    void visualize(Simulation const &simulation, int winWidth, int winHeight);
    void toggle(Option option);
    void enable(Option option);
    void disable(Option option);
    bool is_enabled(Option option) const;
    void scale_hedgehogs(float scale);
    float hedgehog_scale() const;
    void set_hue(const float h);
    void set_saturation(const float s);
    void set_num_of_colors(const int n);

    Colormap* getColormap();
    Colormap* loadColormap(ColorMode);

    void setColor(float vy, ColorType t);
    void direction_to_color(float x, float y);
    void magnitude_to_color(float x, float y);
    void setScalarDataset(DatasetType sdm);
    void setVectorDataset(DatasetType vdm);
    void set_sample_x(int x);
    void set_sample_y(int y);

    void setScalarMin(float);
    void setScalarMax(float);
    void setScalarMode(Mode);
    float getScalarMin();
    float getScalarMax();
    Mode getScalarMode();

    void setGlyphType(GlyphType);

private:

    void initializeColormaps();
    std::map<ColorMode, Colormap*> colormaps;
    std::map<DatasetType, Dataset> datasets;

    float vec_scale; // scaling of hedgehogs 
    int options[OptionsCount]; // options boolean array
    Colormap* colormap;
    float hue, saturation;
    DatasetType scalarDataset;
    DatasetType vectorDataset;
    GlyphType glyphType;
    int sample_x, sample_y;

    void draw_smoke(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn);
    void draw_velocities(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn);
    void draw_forces(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn);
    float pick_scalar_field_value(Simulation const &simulation, size_t idx);
    void pick_vector_field_value(Simulation const &simulation, size_t idx, float values[]);
    void draw_glyphs(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn, const fftw_real wn_sample, const fftw_real hn_sample);
    void draw_3d_cones(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn);
    void draw_simple_arrows(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn);
    GLfloat pick_scaled_field(float v);
};

#endif