#include "Application.h"
#include <iostream>
#include <GLUT/glut.h>
#include "Utilities.h"
#include <GLUI/glui.h>
#include <OpenGL/gl.h>

using namespace std;

Simulation Application::simulation;			// the smoke simulation
Visualization Application::visualization;	// visualization of the simulation

//size of the graphics window, in pixels
int Application::winWidth;
int Application::winHeight;		

GLUI* Application::glui;						// user interface
int Application::main_window;

int Application::selectedColormap;
Colorbar* Application::colorbar;

void Application::update()
{
    glutSetWindow(main_window);
    simulation.do_one_simulation_step();
    glutPostRedisplay();
}

void Application::initialize(int *argc, char** argv)
{	
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(1000, 800);
    main_window = glutCreateWindow("Real-time smoke simulation and visualization");

    // pass static functions as callback to GLUT
    glutDisplayFunc(display);
    glutMotionFunc(drag);
    glutKeyboardFunc(keyboard);

    // pass static functions as callback to GLUI
    GLUI_Master.set_glutReshapeFunc(reshape);  
    GLUI_Master.set_glutIdleFunc(update);

    simulation.init_simulation(Simulation::DIM);	//initialize the simulation data structures
    initUI();
    
    colorbar = new Colorbar();

    glutMainLoop();                                 // enter main loop
}

// output usage instructions
void Application::outputUsage()
{
    cout << "Fluid Flow Simulation and Visualization\n";
    cout << "=======================================\n";
    cout << "Click and drag the mouse to steer the flow!\n";
    cout << "T/t:   increase/decrease simulation timestep\n";
    cout << "S/s:   increase/decrease hedgehog scaling\n";
    cout << "c:     toggle direction coloring on/off\n";
    cout << "V/v:   increase decrease fluid viscosity\n";
    cout << "x:     toggle drawing matter on/off\n";
    cout << "y:     toggle drawing hedgehogs on/off\n";
    cout << "m:     toggle thru scalar coloring\n";
    cout << "a:     toggle the animation on/off\n";
    cout << "q:     quit\n\n";
}

//------ INTERACTION CODE STARTS HERE -----------------------------------------------------------------

//display: Handle window redrawing events. Simply delegates to visualize().
void Application::display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    visualization.visualize(simulation, winWidth, winHeight);
    drawColorbar();
    glFlush();
    glutSwapBuffers();
}

//reshape: Handle window resizing (reshaping) events
void Application::reshape(int w, int h)
{
    glViewport(0.0f, 0.0f, (GLfloat)w, (GLfloat)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
    winWidth = w; 
    winHeight = h;
}

//keyboard: Handle key presses
void Application::keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 't': { simulation.change_timestep(-0.001); } break;
        case 'T': { simulation.change_timestep(0.001); } break;		
        case 'c': { visualization.toggle(Visualization::UseDirectionColoring); } break;
        case 'S': { visualization.scale_hedgehogs(1.2); } break;
        case 's': { visualization.scale_hedgehogs(0.8); } break;
        case 'V': { simulation.scale_viscosity(5.0); } break;
        case 'v': { simulation.scale_viscosity(0.2); } break;

        case 'x': 
        {
                visualization.toggle(Visualization::DrawSmoke);		
                if(visualization.is_enabled(Visualization::DrawSmoke))
                        visualization.enable(Visualization::DrawVectorField);				  		  
        } break;

        case 'y': 
        {				
                visualization.toggle(Visualization::DrawVectorField);		
                if(visualization.is_enabled(Visualization::DrawVectorField))
                        visualization.enable(Visualization::DrawSmoke);		
        } break;

        case 'm': { } break;

        case 'a': { simulation.toggle_frozen(); } break;

        case 'q': { quit(); } break;

        default: {} break;
    }

    // sync live variables
    GLUI_Master.sync_live_all();
}

// drag: When the user drags with the mouse, add a force that corresponds to the direction of the mouse
//       cursor movement. Also inject some new matter into the field at the mouse location.
void Application::drag(int mx, int my)
{
    int xi, yi, X, Y; 
    double  dx, dy, len;
    static int lmx=0,lmy=0;				//remembers last mouse location

    // Compute the array index that corresponds to the cursor location
    xi = (int)clamp((double)(Simulation::DIM + 1) * ((double)mx / (double)winWidth));
    yi = (int)clamp((double)(Simulation::DIM + 1) * ((double)(winHeight - my) / (double)winHeight));

    X = xi; Y = yi;

    if (X > (Simulation::DIM - 1))  
            X = Simulation::DIM - 1; 

    if (Y > (Simulation::DIM - 1))  
            Y = Simulation::DIM - 1;

    if (X < 0) X = 0; if (Y < 0) Y = 0;

    // Add force at the cursor location
    my = winHeight - my;
    dx = mx - lmx; dy = my - lmy;
    len = sqrt(dx * dx + dy * dy);
    if (len != 0.0) {  dx *= 0.1 / len; dy *= 0.1 / len; }

    simulation.insert_force(X, Y, dx, dy);	
    lmx = mx; 
    lmy = my;
}

void Application::buttonHandler(int id)
{
    switch(id)
    {
        case QuitButton : { quit(); } break;
        case SelectColormap:
        {
            switch(selectedColormap)
            {
                case Visualization::Rainbow:
                {
                    visualization.set_scalar_col(Visualization::Rainbow);
                }
                break;
                case Visualization::Grayscale:
                {
                    visualization.set_scalar_col(Visualization::Grayscale);
                }
                break;
                case Visualization::Custom:
                {
                    visualization.set_scalar_col(Visualization::Custom); 
                }
                break;
                default: {} break;
            }
        }
        break;
        default: {} break;
    }
}

void Application::initUI()
{
    glui = GLUI_Master.create_glui_subwindow(main_window, GLUI_SUBWINDOW_RIGHT);
    glui->set_main_gfx_window(main_window);

    // options
    GLUI_Panel *options_panel = new GLUI_Panel(glui, "Options");
    options_panel->set_w(200);
    new GLUI_Checkbox( options_panel, "Draw Vector Field", &visualization.options[Visualization::DrawVectorField]);
    new GLUI_Checkbox( options_panel, "Draw Smoke", &visualization.options[Visualization::DrawSmoke]);

    new GLUI_Button(glui, "Quit", QuitButton, buttonHandler);

    GLUI_Listbox *colormap_list = glui->add_listbox("Select Colormap", &selectedColormap, SelectColormap, buttonHandler);
    colormap_list->add_item(Visualization::Rainbow, "Rainbow");
    colormap_list->add_item(Visualization::Grayscale, "Grayscale");
    colormap_list->add_item(Visualization::Custom, "Custom");

}

void Application::drawColorbar()
{
    colorbar->setup();
    colorbar->render();
}

void Application::quit()
{
    cout << "Quit.\n";
    exit(0);
}