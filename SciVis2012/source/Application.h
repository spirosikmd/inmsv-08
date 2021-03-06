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
        TUBE_SPINNER,
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
        HEIGHTPLOT_DATASET_LIST,
        DIM_SPINNER,
        SEGMENT_SPINNER,
        SAMPLE_X_SPINNER,
        SAMPLE_Y_SPINNER,
        SEED_X,
        SEED_Y,
        SEED_Z,
        ADD_SEEDPOINT_BUTTON
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
    static void special(int key, int x, int y); // Handle key presses
    static void drag(int mx, int my); // Handle mouse drag
    static void visualize();
    static void quit();
    static void buttonHandler(int id);
    static void updateMenu();
    static void displayMenu();
    static void click(int,int,int,int);
    static DataBuffer timeslices;
private:

    static Simulation simulation; // the smoke simulation
    static Visualization visualization; // visualization of the simulation


    static int winWidth, winHeight; //size of the graphics window, in pixels

    static GLUI *glui; // user interface
    static int main_window;
    static int menu_window;

    static Visualization::ColorMode selectedColormap;

    static void initializeColormaps();
    static std::map<Visualization::ColorMode, Colormap*> colormaps;
    static Colormap *colormap;
    static int selectedNumOfColors;

    static Visualization::DatasetType scalarDataset;
    static float scalarMax, scalarMin;

    static Visualization::DatasetType vectorDataset;
    static Visualization::DatasetType heightplotDataset;

    static Visualization::GlyphType glyphType;

    
    static GLUI_Panel *streamtube_options;
    static float hueValue;
    static float saturationValue;
    static int dim;
    static int numberOfSegments;
    static int sample_x, sample_y;
    static float densityIsoline;
    static float densityRHO1Isoline;
    static float densityRHO2Isoline;
    static int numberIsolines;
    static int angle;
    static int translate_x, translate_z, translate_y, distance;
    static Visualization::Mode scalarMode;

    static int seed_x,seed_y,seed_z;
    
    static void renderColormap();

};

#endif

