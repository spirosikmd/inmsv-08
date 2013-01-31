#ifndef VISUALIZATION_H_INCLUDED
#define	VISUALIZATION_H_INCLUDED

#include <rfftw.h>
#include <map>
#include "Colormap.h"
#include "DataBuffer.h"              //the numerical simulation FFTW library
#include <map>
#include <vector>

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
        DRAW_SMOKE, // draw the smoke or not
        DRAW_ISOLINES,
        DrawVectorField, // draw the vector field or not (not used for now)
        UseDirectionColoring, // use direction color-coding or not (not used for now)
        DRAW_GLYPHS, // draw the velocities or not
        GRADIENT,
        COLORIZE,
        DRAW_HEIGHTPLOT,
        DRAW_NORMALS,
        DRAW_STREAMTUBES,
        OptionsCount // (automatically assigned)
    };

    enum DatasetType {
        DENSITY,
        VELOCITY_MAGN,
        FORCE_MAGN,
        VELOCITY,
        FORCE,
        DENSITY_GRADIENT,
        VELOCITY_MAGN_GRADIENT,
        VELOCITY_DIV,
        NONE
    };

    enum GlyphType {
        HEDGEHOGS,
        SIMPLE_ARROWS,
        CONES_3D,
        ARROWS_3D
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
    void visualize3D(Simulation const &simulation, int winWidth, int winHeight);
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
    void setHeightplotDataset(DatasetType sdm);
    void setSampleX(int x);
    int getSampleX();
    void setSampleY(int y);
    int getSampleY();

    void setScalarMin(float);
    void setScalarMax(float);
    void setScalarMode(Mode);
    float getScalarMin();
    float getScalarMax();
    float getScalarScaleMin();
    float getScalarScaleMax();
    Mode getScalarMode();

    void setDensityIsoline(float);
    float getDensityIsoline();
    void setDensityRHO1Isoline(float);
    void setDensityRHO2Isoline(float);
    void setNumIsolines(int);
    void setGlyphType(GlyphType);

private:

    void initializeColormaps();
    std::map<ColorMode, Colormap*> colormaps;
    std::map<DatasetType, Dataset> datasets;

    float vec_scale; // scaling of hedgehogs 
    int options[OptionsCount]; // options boolean array
    Colormap* colormap;
    float hue, saturation;
    float densityIsoline;
    float densityRHO1Isoline;
    float densityRHO2Isoline;
    int numIsolines;
    DatasetType scalarDataset;
    DatasetType vectorDataset;
    DatasetType heightplotDataset;
    GlyphType glyphType;
    int sample_x, sample_y;

    float getHeight(float vy, float maxheight);
    void draw_streamtubes(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn);
    void draw_streamtube(std::vector<std::vector<float > > points);
    std::vector<std::vector<float > > calculateStreamtubePoints(float x, float y, float z, float dt, int maxLength, int DIM, float wn, float hn, float zn);
    void draw_heightplot(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn);
    void draw_smoke(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn);
    void draw_isoline(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn, float);
    void draw_velocities(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn);
    void draw_forces(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn);
    float pick_scalar_field_value(Simulation const &simulation, size_t idx);
    float pick_scalar_field_value(DatasetType ds, Simulation const &simulation, size_t idx);
    void pick_vector_field_value(Simulation const &simulation, size_t idx, float values[], int i = 0, int j = 0, float wn = 0.0, float hn = 0.0, int DIM = 0);
    float pick_timescalar_field_value(size_t idx, int t);
    void pick_timevector_field_value(size_t idx, float values[], int t);
    void draw_glyphs(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn, const fftw_real wn_sample, const fftw_real hn_sample);
    void draw_hedgehogs(GLfloat x_start, GLfloat y_start, float value, float values[]);
    void draw_simple_arrow(GLfloat magn, GLfloat x_start, GLfloat y_start, GLfloat angle, float value);
    void draw_3d_cone(GLfloat magn, GLfloat x_start, GLfloat y_start, GLfloat angle, float value);
    void draw_3d_arrow(GLfloat magn, GLfloat x_start, GLfloat y_start, GLfloat angle, float value);
    void draw_glyphs_on_comp_grid(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn, const fftw_real wn_sample, const fftw_real hn_sample);
    void draw_glyphs_on_sampled_grid(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn, const fftw_real wn_sample, const fftw_real hn_sample);
    void draw_streamlines(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn);
    GLfloat pick_scaled_field(float v);
    float divergence(Simulation const &simulation, int idx);
    int modIndex(int x, int y, int DIM);
};

#endif