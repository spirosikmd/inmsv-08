#ifndef APPLICATION_H_INCLUDED
#define	APPLICATION_H_INCLUDED

#include <GLUI/glui.h>
#include "Simulation.h"
#include "Visualization.h"
#include "Colorbar.h"

class Application
{
    private:

        // GLUI control IDs
        enum UiControlID
        {
            QuitButton,
            SelectColormap,
            SelectedNumOfColors,
            HueSpinner,
            SaturationSpinner,
            ApplicationMode,
            ScalarDrawMode,
            VectorDrawMode
        };

    public:

        Application();
        ~Application();				

        static void initialize(int *argc, char** argv);
        static void initUI();
        static void outputUsage();		
        static void display();
        static void update();
        static void reshape(int w, int h);						// Handle window resizing (reshaping) events
        static void keyboard(unsigned char key, int x, int y);	// Handle key presses
        static void drag(int mx, int my);						// Handle mouse drag
        static void visualize();
        static void drawColorbar();
        static void quit();		
        static void buttonHandler(int id);

    private:

        static Simulation simulation;			// the smoke simulation
        static Visualization visualization;		// visualization of the simulation

        static int winWidth, winHeight;			//size of the graphics window, in pixels

        static GLUI *glui;						// user interface
        static int main_window;

        static int selected_colormap;
        static Colorbar *colorbar;

        static int selected_num_of_colors;
        
        static float hue_value;
        static float saturation_value;
        
        static Visualization::ApplicationMode app_mode;
        static Visualization::ScalarDrawMode scalar_draw_mode;
        static Visualization::VectorDrawMode vector_draw_mode;
};

#endif

