
#include "Colorbar.h"

#include "Application.h"
#include <iostream>
#include <GLUT/glut.h>
#include <GLUI/glui.h>
#include <OpenGL/gl.h>
#include <map>

using namespace std;

Simulation Application::simulation; // the smoke simulation
Visualization Application::visualization; // visualization of the simulation

//size of the graphics window, in pixels
int Application::winWidth;
int Application::winHeight;

GLUI* Application::glui; // user interface
int Application::main_window;

Visualization::ColorMode Application::selectedColormap;
std::map<Visualization::ColorMode, Colormap*> Application::colormaps;
Colormap* Application::colormap;
int Application::selectedNumOfColors;
float Application::hueValue;
float Application::saturationValue;
Visualization::ScalarDataset Application::scalarDataset;
Visualization::VectorDataset Application::vectorDataset;
int Application::sample_x;
int Application::sample_y;

Visualization::Mode Application::appMode;

void Application::update() {
    glutSetWindow(main_window);
    simulation.do_one_simulation_step();
    glutPostRedisplay();
}

void Application::initialize(int *argc, char** argv) {
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

    simulation.init_simulation(Simulation::DIM); //initialize the simulation data structures


    visualization.initializeColormaps();

    Colormap* colormap = visualization.loadColormap(Visualization::GRADIENT);

    hueValue = colormap->getHue();
    saturationValue = colormap->getSaturation();
    selectedNumOfColors = colormap->getNumberOfColors();


    initUI();
    glutMainLoop(); // enter main loop
}
// output usage instructions

void Application::outputUsage() {
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

void Application::display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_TEXTURE_1D);
    visualization.visualize(simulation, winWidth, winHeight);
    glDisable(GL_TEXTURE_1D);
    
    glTranslatef(10, 10, 0);
    visualization.getColormap()->render(0,1,5);;
    glFlush();
    glutSwapBuffers();
}

//reshape: Handle window resizing (reshaping) events

void Application::reshape(int w, int h) {
    glViewport(0.0f, 0.0f, (GLfloat) w, (GLfloat) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (GLdouble) w, 0.0, (GLdouble) h);
    winWidth = w;
    winHeight = h;
}

//keyboard: Handle key presses

void Application::keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 't':
        {
            simulation.change_timestep(-0.001);
        }
            break;
        case 'T':
        {
            simulation.change_timestep(0.001);
        }
            break;
        case 'c':
        {
            visualization.toggle(Visualization::UseDirectionColoring);
        }
            break;
        case 'S':
        {
            visualization.scale_hedgehogs(1.2);
        }
            break;
        case 's':
        {
            visualization.scale_hedgehogs(0.8);
        }
            break;
        case 'V':
        {
            simulation.scale_viscosity(5.0);
        }
            break;
        case 'v':
        {
            simulation.scale_viscosity(0.2);
        }
            break;

        case 'x':
        {
            visualization.toggle(Visualization::DrawSmoke);
            if (visualization.is_enabled(Visualization::DrawSmoke))
                visualization.enable(Visualization::DrawVectorField);
        }
            break;

        case 'y':
        {
            visualization.toggle(Visualization::DrawVectorField);
            if (visualization.is_enabled(Visualization::DrawVectorField))
                visualization.enable(Visualization::DrawSmoke);
        }
            break;

        case 'm':
        {
        }
            break;

        case 'a':
        {
            simulation.toggle_frozen();
        }
            break;

        case 'q':
        {
            quit();
        }
            break;

        default:
        {
        }
            break;
    }

    // sync live variables
    GLUI_Master.sync_live_all();
}

// drag: When the user drags with the mouse, add a force that corresponds to the direction of the mouse
//       cursor movement. Also inject some new matter into the field at the mouse location.

void Application::drag(int mx, int my) {
    int xi, yi, X, Y;
    double dx, dy, len;
    static int lmx = 0, lmy = 0; //remembers last mouse location

    // Compute the array index that corresponds to the cursor location
    xi = (int) clamp((double) (Simulation::DIM + 1) * ((double) mx / (double) winWidth));
    yi = (int) clamp((double) (Simulation::DIM + 1) * ((double) (winHeight - my) / (double) winHeight));

    X = xi;
    Y = yi;

    if (X > (Simulation::DIM - 1))
        X = Simulation::DIM - 1;

    if (Y > (Simulation::DIM - 1))
        Y = Simulation::DIM - 1;

    if (X < 0) X = 0;
    if (Y < 0) Y = 0;

    // Add force at the cursor location
    my = winHeight - my;
    dx = mx - lmx;
    dy = my - lmy;
    len = sqrt(dx * dx + dy * dy);
    if (len != 0.0) {
        dx *= 0.1 / len;
        dy *= 0.1 / len;
    }

    simulation.insert_force(X, Y, dx, dy);
    lmx = mx;
    lmy = my;
}

void Application::buttonHandler(int id) {
    Colormap* colormap = visualization.getColormap();
    switch (id) {
        case QuitButton:
        {
            quit();
        }
            break;
        case SelectColormap:
        {


            Colormap* colormap = visualization.loadColormap(selectedColormap);
            hueValue = colormap->getHue();
            saturationValue = colormap->getSaturation();
            selectedNumOfColors = colormap->getNumberOfColors();
            glui->sync_live();
        }
            break;
        case SelectedNumOfColors:
        {
            colormap->setNumberOfColors(selectedNumOfColors);
        }
            break;
        case HueSpinner:
        {
            visualization.set_hue(hueValue);
            colormap->setHue(hueValue);
        }
            break;
        case SaturationSpinner:
        {
            visualization.set_saturation(saturationValue);
            colormap->setSaturation(saturationValue);
        }
            break;
        case ApplicationMode:
        {

        }
            break;
        case ScalarDrawMode:
        {
            visualization.set_scalar_draw_mode(scalarDataset);
        }
            break;
        case VectorDrawMode:
        {
            visualization.set_vector_draw_mode(vectorDataset);
        }
            break;
        case XSample:
        {
            visualization.set_sample_x(sample_x);
        }
            break;
        case YSample:
        {
            visualization.set_sample_y(sample_y);
        }
            break;
        default:
        {
        }
            break;
    }
}

void Application::initUI() {
    // main window
    glui = GLUI_Master.create_glui_subwindow(main_window, GLUI_SUBWINDOW_RIGHT);
    glui->set_main_gfx_window(main_window);

    // colormap
    GLUI_Panel *colormap_options = new GLUI_Panel(glui, "Colormap");
    colormap_options->set_alignment(GLUI_ALIGN_LEFT);

    GLUI_Listbox *colormap_list = new GLUI_Listbox(colormap_options, "Colormap ", (int*) &selectedColormap, SelectColormap, buttonHandler);
    colormap_list->set_alignment(GLUI_ALIGN_RIGHT);
    colormap_list->add_item(Visualization::GRADIENT, "Gradient");
    colormap_list->add_item(Visualization::RAINBOW, "Rainbow");
    colormap_list->add_item(Visualization::ZEBRA, "Zebra");

    GLUI_Spinner *hue_spinner = new GLUI_Spinner(colormap_options, "Hue ", &hueValue, HueSpinner, buttonHandler);
    hue_spinner->set_float_limits(0.0, 1.0, GLUI_LIMIT_CLAMP);
    hue_spinner->set_alignment(GLUI_ALIGN_RIGHT);

    GLUI_Spinner *sat_spinner = new GLUI_Spinner(colormap_options, "Saturation ", &saturationValue, SaturationSpinner, buttonHandler);
    sat_spinner->set_float_limits(0.0, 1.0, GLUI_LIMIT_CLAMP);
    sat_spinner->set_alignment(GLUI_ALIGN_RIGHT);

    GLUI_Listbox *num_of_colors_list = new GLUI_Listbox(colormap_options, "Band ", &selectedNumOfColors, SelectedNumOfColors, buttonHandler);
    num_of_colors_list->set_alignment(GLUI_ALIGN_RIGHT);
    num_of_colors_list->add_item(Colormap::COL_2, "2");
    num_of_colors_list->add_item(Colormap::COL_4, "4");
    num_of_colors_list->add_item(Colormap::COL_8, "8");
    num_of_colors_list->add_item(Colormap::COL_16, "16");
    num_of_colors_list->add_item(Colormap::COL_32, "32");
    num_of_colors_list->add_item(Colormap::COL_64, "64");
    num_of_colors_list->add_item(Colormap::COL_128, "128");
    num_of_colors_list->add_item(Colormap::COL_256, "256");
    num_of_colors_list->do_selection(Colormap::COL_256);

    GLUI_Listbox *application_mode_list = new GLUI_Listbox(colormap_options, "Mode ", (int*) &appMode, ApplicationMode, buttonHandler);
    application_mode_list->add_item(Visualization::SCALING, "Scale");
    application_mode_list->add_item(Visualization::CLAMPING, "Clamp");
    application_mode_list->set_alignment(GLUI_ALIGN_RIGHT);

    // dataset
    GLUI_Panel *datasetOptions = new GLUI_Panel(glui, "Dataset");
    datasetOptions->set_alignment(GLUI_ALIGN_LEFT);
    glui->add_statictext_to_panel(datasetOptions, "Scalar                           ");
    //GLUI_Panel *scalarDatasets = new GLUI_Panel(datasetOptions, "Scalar");
    GLUI_RadioGroup *scalarDatasetsGroup = new GLUI_RadioGroup(datasetOptions, (int*) &scalarDataset, ScalarDataset, buttonHandler);
    new GLUI_RadioButton(scalarDatasetsGroup, "Density rho");
    new GLUI_RadioButton(scalarDatasetsGroup, "Velocity |v|");
    new GLUI_RadioButton(scalarDatasetsGroup, "Force |f|");
    scalarDatasetsGroup->set_alignment(GLUI_ALIGN_LEFT);
    glui->add_separator_to_panel(datasetOptions);
    glui->add_statictext_to_panel(datasetOptions, "Vector  ");
    GLUI_RadioGroup *vectorDatasetsGroup = new GLUI_RadioGroup(datasetOptions, (int*) &vectorDataset, VectorDataset, buttonHandler);
    new GLUI_RadioButton(vectorDatasetsGroup, "Velocity v");
    new GLUI_RadioButton(vectorDatasetsGroup, "Force f");
    vectorDatasetsGroup->set_alignment(GLUI_ALIGN_LEFT);

    // quit
    GLUI_Button *quit = new GLUI_Button(glui, "Quit", QuitButton, buttonHandler);
    quit->set_alignment(GLUI_ALIGN_LEFT);





    // options
    GLUI_Panel *options_panel = new GLUI_Panel(glui, "Options");
    options_panel->set_w(200);
    new GLUI_Checkbox(options_panel, "Draw Glyphs", &visualization.options[Visualization::DrawGlyphs]);
    new GLUI_Checkbox(options_panel, "Draw Forces", &visualization.options[Visualization::DrawForces]);
    new GLUI_Checkbox(options_panel, "Draw Smoke", &visualization.options[Visualization::DrawSmoke]);

//    GLUI_Listbox *scalar_field_mode = new GLUI_Listbox(glui, "Scalar Field", (int*) &scalar_draw_mode, ScalarDrawMode, buttonHandler);
//    scalar_field_mode->add_item(Visualization::Density, "Density rho");
//    scalar_field_mode->add_item(Visualization::VelocityMagnitude, "Velocity |v|");
//    scalar_field_mode->add_item(Visualization::ForceMagnitude, "Force |f|");
//
//    GLUI_Listbox *vector_field_mode = new GLUI_Listbox(glui, "Vector Field", (int*) &vector_draw_mode, VectorDrawMode, buttonHandler);
//    vector_field_mode->add_item(Visualization::Velocity, "Velocity v");
//    vector_field_mode->add_item(Visualization::Force, "Force f");

    GLUI_Spinner *sample_x_spinner = new GLUI_Spinner(glui, "X Sample", &sample_x, XSample, buttonHandler);
    sample_x_spinner->set_int_val(20);
    GLUI_Spinner *sample_y_spinner = new GLUI_Spinner(glui, "Y Sample", &sample_y, YSample, buttonHandler);
    sample_y_spinner->set_int_val(20);
}

void Application::quit() {
    cout << "Quit.\n";
    exit(0);
}
