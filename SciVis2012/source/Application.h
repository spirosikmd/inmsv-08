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
        QuitButton,
        SelectColormap,
        SelectedNumOfColors,
        HueSpinner,
        SaturationSpinner,
        ApplicationMode,
        ScalarDrawMode,
        VectorDrawMode,
        ScalarDataset,
        VectorDataset,
        XSample,
        YSample
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
    static int scalarDataset;
    static int vectorDataset;

    static float hueValue;
    static float saturationValue;
    static int sample_x, sample_y;

    static Visualization::Mode appMode;
    static Visualization::ScalarDrawMode scalar_draw_mode;
    static Visualization::VectorDrawMode vector_draw_mode;


};

#endif

