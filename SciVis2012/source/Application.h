#ifndef APPLICATION_H_INCLUDED
#define	APPLICATION_H_INCLUDED

#include <GLUI/glui.h>
#include "Simulation.h"
#include "Visualization.h"
#include "Colormap.h"
#include <map>

class Application {
private:

    // GLUI control IDs

    enum UiControlID {
        QUIT_BUTTON,
        COLORMAP_LIST,
        NUMBER_OF_COLORS_LIST,
        HUE_SPINNER,
        SATURATION_SPINNER,
        SCALAR_DATASET_LIST,
        SCALAR_MODE_LIST,
        SCALAR_MAX_SPINNER,
        SCALAR_MIN_SPINNER,
        VECTOR_DATASET_LIST,
        GLYPH_TYPE_LIST,
        DENSITY_ISOLINE_SPINNER,
        DENSITY_RHO1_ISOLINE_SPINNER,
        DENSITY_RHO2_ISOLINE_SPINNER,
        NUMBER_ISOLINES_SPINNER,
        DIM_SPINNER,
        SAMPLE_X_SPINNER,
        SAMPLE_Y_SPINNER
    };

public:

    Application();
    ~Application();

    static void initialize(int *argc, char** argv);
    static void initUI();
    static void outputUsage();
    static void display();
    static void update();
    static void reshape(int w, int h); // Handle window resizing (reshaping) events
    static void keyboard(unsigned char key, int x, int y); // Handle key presses
    static void drag(int mx, int my); // Handle mouse drag
    static void visualize();
    static void quit();
    static void buttonHandler(int id);

private:

    static Simulation simulation; // the smoke simulation
    static Visualization visualization; // visualization of the simulation

    static int winWidth, winHeight; //size of the graphics window, in pixels

    static GLUI *glui; // user interface
    static int main_window;

    static Visualization::ColorMode selectedColormap;

    static void initializeColormaps();
    static std::map<Visualization::ColorMode, Colormap*> colormaps;
    static Colormap *colormap;
    static int selectedNumOfColors;

    static Visualization::DatasetType scalarDataset;
    static float scalarMax, scalarMin;

    static Visualization::DatasetType vectorDataset;

    static Visualization::GlyphType glyphType;

    static float hueValue;
    static float saturationValue;
    static int dim;
    static int sample_x, sample_y;
    static float densityIsoline;
    static float densityRHO1Isoline;
    static float densityRHO2Isoline;
    static int numberIsolines;

    static Visualization::Mode scalarMode;
    
    static void renderColormap();

};

#endif

