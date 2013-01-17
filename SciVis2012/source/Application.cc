
#include "Colorbar.h"

#include "Application.h"
#include <iostream>
#include <GLUT/glut.h>
#include <GLUI/glui.h>
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

Visualization::DatasetType Application::scalarDataset;
float Application::scalarMax;
float Application::scalarMin;

Visualization::DatasetType Application::vectorDataset;
Visualization::GlyphType Application::glyphType;
int Application::dim;
int Application::sample_x;
int Application::sample_y;

Visualization::Mode Application::scalarMode;

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

    simulation.init_simulation(); //initialize the simulation data structures

    visualization.initializeColormaps();
    Colormap* colormap = visualization.loadColormap(Visualization::GRAYSCALE);
    hueValue = colormap->getHue();
    saturationValue = colormap->getSaturation();
    selectedNumOfColors = colormap->getNumberOfColors();

    scalarMax = visualization.getScalarMax();
    scalarMin = visualization.getScalarMin();
    scalarMode = visualization.getScalarMode();
    
    dim = simulation.get_DIM();
    
    sample_x = visualization.getSampleX();
    sample_y = visualization.getSampleY();

    initUI();
    glui->sync_live();

    GLfloat black[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat cyan[] = {0.0, 1.0, 1.0, 1.0};
    GLfloat white[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat direction[] = {1.0, 1.0, 5.0, 0.0};

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, cyan);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glMaterialf(GL_FRONT, GL_SHININESS, 30);

    glLightfv(GL_LIGHT0, GL_AMBIENT, black);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);
    glLightfv(GL_LIGHT0, GL_POSITION, direction);

    glEnable(GL_LIGHTING); // so the renderer considers light
    glEnable(GL_LIGHT0); // turn LIGHT0 on
    glEnable(GL_DEPTH_TEST); // so the renderer considers depth
    glEnable(GL_COLOR_MATERIAL); // to be able to color objects when lighting is on

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

    glViewport(0.0f, 0.0f, (GLfloat) winWidth, (GLfloat) winHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (GLdouble) winWidth, 0.0, (GLdouble) winHeight, -10, 10);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    visualization.getColormap()->render(visualization.getScalarScaleMin(), visualization.getScalarScaleMax(), 5);
    glEnable(GL_LIGHTING);

    visualization.visualize(simulation, winWidth, winHeight);

    glFlush();
    glutSwapBuffers();
}

//reshape: Handle window resizing (reshaping) events

void Application::reshape(int w, int h) {
    //
    //    glViewport(0.0f, 0.0f, (GLfloat) w * (1.0 / 4.0), (GLfloat) h);
    //    glMatrixMode(GL_PROJECTION);
    //    glLoadIdentity();
    //    glOrtho(0.0, (GLdouble) w, 0.0, (GLdouble) h, -10, 10);
    //
    //    //
    //    glViewport((GLfloat) w * (1.0 / 4.0), 0.0f, (GLfloat) w, (GLfloat) h);
    //    glMatrixMode(GL_PROJECTION);
    //    glLoadIdentity();
    //    glOrtho(0.0, (GLdouble) w, 0.0, (GLdouble) h, -10, 10);

    //    if (w <= h) {
    //        // width is smaller, so stretch out the height
    //        glOrtho(-2.5, 2.5, -2.5 / aspect, 2.5 / aspect, -10.0, 10.0);
    //    } else {
    //        // height is smaller, so stretch out the width
    //        glOrtho(-2.5 * aspect, 2.5 * aspect, -2.5, 2.5, -10.0, 10.0);
    //    }

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
            visualization.toggle(Visualization::DRAW_SMOKE);
            if (visualization.is_enabled(Visualization::DRAW_SMOKE))
                visualization.enable(Visualization::DrawVectorField);
        }
            break;

        case 'y':
        {
            visualization.toggle(Visualization::DrawVectorField);
            if (visualization.is_enabled(Visualization::DrawVectorField))
                visualization.enable(Visualization::DRAW_SMOKE);
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

        case QUIT_BUTTON:
            quit();
            break;

        case COLORMAP_LIST:
            colormap = visualization.loadColormap(selectedColormap);
            hueValue = colormap->getHue();
            saturationValue = colormap->getSaturation();
            selectedNumOfColors = colormap->getNumberOfColors();
            break;

        case NUMBER_OF_COLORS_LIST:
            colormap->setNumberOfColors(selectedNumOfColors);
            break;

        case HUE_SPINNER:
            visualization.set_hue(hueValue);
            colormap->setHue(hueValue);
            break;

        case SATURATION_SPINNER:
            visualization.set_saturation(saturationValue);
            colormap->setSaturation(saturationValue);
            break;

        case SCALAR_DATASET_LIST:
            visualization.setScalarDataset(scalarDataset);
            scalarMax = visualization.getScalarMax();
            scalarMin = visualization.getScalarMin();
            scalarMode = visualization.getScalarMode();
            break;

        case SCALAR_MAX_SPINNER:
            if (scalarMax <= scalarMin) {
                scalarMax = scalarMin;
            }
            visualization.setScalarMax(scalarMax);
            break;

        case SCALAR_MIN_SPINNER:
            if (scalarMin >= scalarMax) {
                scalarMin = scalarMax;
            }
            visualization.setScalarMin(scalarMin);
            break;

        case SCALAR_MODE_LIST:
            visualization.setScalarMode(scalarMode);
            break;

        case VECTOR_DATASET_LIST:
            visualization.setVectorDataset(vectorDataset);
            break;

        case GLYPH_TYPE_LIST:
            visualization.setGlyphType(glyphType);
            break;
            
        case DIM_SPINNER:
            simulation.set_DIM(dim);
            simulation.init_simulation();
            break;

        case SAMPLE_X_SPINNER:
            visualization.setSampleX(sample_x);
            break;
            
        case SAMPLE_Y_SPINNER:
            visualization.setSampleY(sample_y);
            break;
            
        default:
            break;
    }
    glui->sync_live();
}

void Application::initUI() {
    // main window
    glui = GLUI_Master.create_glui_subwindow(main_window, GLUI_SUBWINDOW_RIGHT);
    glui->set_main_gfx_window(main_window);

    // colormap
    GLUI_Panel *colormap_options = new GLUI_Panel(glui, "Colormap");
    colormap_options->set_alignment(GLUI_ALIGN_LEFT);

    GLUI_Listbox *colormap_list = new GLUI_Listbox(colormap_options, "Colormap ", (int*) &selectedColormap, COLORMAP_LIST, buttonHandler);
    colormap_list->set_alignment(GLUI_ALIGN_RIGHT);
    colormap_list->add_item(Visualization::GRAYSCALE, "Grayscale");
    colormap_list->add_item(Visualization::BGRADIENT, "Black Gradient");
    colormap_list->add_item(Visualization::WGRADIENT, "White Gradient");
    colormap_list->add_item(Visualization::RAINBOW, "Rainbow");
    colormap_list->add_item(Visualization::HEATMAP, "Heatmap");
    colormap_list->add_item(Visualization::BLUEYELLOWGREEN, "Blue Yellow Green");
    colormap_list->add_item(Visualization::ZEBRA, "Zebra");

    GLUI_Spinner *hue_spinner = new GLUI_Spinner(colormap_options, "Hue ", &hueValue, HUE_SPINNER, buttonHandler);
    hue_spinner->set_float_limits(0.0, 1.0, GLUI_LIMIT_CLAMP);
    hue_spinner->set_alignment(GLUI_ALIGN_RIGHT);

    GLUI_Spinner *sat_spinner = new GLUI_Spinner(colormap_options, "Saturation ", &saturationValue, SATURATION_SPINNER, buttonHandler);
    sat_spinner->set_float_limits(0.0, 1.0, GLUI_LIMIT_CLAMP);
    sat_spinner->set_alignment(GLUI_ALIGN_RIGHT);

    GLUI_Listbox *num_of_colors_list = new GLUI_Listbox(colormap_options, "Band ", &selectedNumOfColors, NUMBER_OF_COLORS_LIST, buttonHandler);
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
    glui->add_statictext_to_panel(colormap_options, "                                              ");

    // dataset
    GLUI_Panel *datasetOptions = new GLUI_Panel(glui, "Dataset");
    datasetOptions->set_alignment(GLUI_ALIGN_LEFT);

    //GLUI_Panel *scalarDatasets = new GLUI_Panel(datasetOptions, "Scalar");
    GLUI_Listbox *scalarDatasetsGroup = new GLUI_Listbox(datasetOptions, "Scalar ", (int*) &scalarDataset, SCALAR_DATASET_LIST, buttonHandler);
    scalarDatasetsGroup->set_alignment(GLUI_ALIGN_RIGHT);
    scalarDatasetsGroup->add_item(Visualization::DENSITY, "Density rho");
    scalarDatasetsGroup->add_item(Visualization::VELOCITY_MAGN, "Velocity |v|");
    scalarDatasetsGroup->add_item(Visualization::FORCE_MAGN, "Force |f|");

    GLUI_Listbox *application_mode_list = new GLUI_Listbox(datasetOptions, "Mode ", (int*) &scalarMode, SCALAR_MODE_LIST, buttonHandler);
    application_mode_list->add_item(Visualization::SCALING, "Scale");
    application_mode_list->add_item(Visualization::CLAMPING, "Clamp");
    application_mode_list->set_alignment(GLUI_ALIGN_RIGHT);

    GLUI_Spinner *scalar_max_spinner = new GLUI_Spinner(datasetOptions, "Clamp Max ", &scalarMax, SCALAR_MAX_SPINNER, buttonHandler);
    scalar_max_spinner->set_alignment(GLUI_ALIGN_RIGHT);
    GLUI_Spinner *scalar_min_spinner = new GLUI_Spinner(datasetOptions, "Clamp Min ", &scalarMin, SCALAR_MIN_SPINNER, buttonHandler);
    scalar_min_spinner->set_alignment(GLUI_ALIGN_RIGHT);


    glui->add_separator_to_panel(datasetOptions);
    GLUI_Listbox *vectorDatasetsGroup = new GLUI_Listbox(datasetOptions, "Vector ", (int*) &vectorDataset, VECTOR_DATASET_LIST, buttonHandler);
    vectorDatasetsGroup->add_item(Visualization::VELOCITY, "Velocity v");
    vectorDatasetsGroup->add_item(Visualization::FORCE, "Force f");
    vectorDatasetsGroup->add_item(Visualization::DENSITY_GRADIENT, "Density Gradient");
    vectorDatasetsGroup->add_item(Visualization::VELOCITY_MAGN_GRADIENT, "Velocity |V| Gradient");
    vectorDatasetsGroup->set_alignment(GLUI_ALIGN_RIGHT);
    glui->add_statictext_to_panel(datasetOptions, "                                              ");

    // visualization technique
    GLUI_Panel *visualization_options = new GLUI_Panel(glui, "Options");
    visualization_options->set_alignment(GLUI_ALIGN_LEFT);
    GLUI_Checkbox *glyphs_box = new GLUI_Checkbox(visualization_options, "Glyphs", &visualization.options[Visualization::DRAW_GLYPHS]);
    glyphs_box->set_alignment(GLUI_ALIGN_RIGHT);
    GLUI_Checkbox *smoke_box = new GLUI_Checkbox(visualization_options, "Smoke", &visualization.options[Visualization::DRAW_SMOKE]);
    smoke_box->set_alignment(GLUI_ALIGN_RIGHT);
    GLUI_Checkbox *gradient_box = new GLUI_Checkbox(visualization_options, "Gradient", &visualization.options[Visualization::GRADIENT]);
    gradient_box->set_alignment(GLUI_ALIGN_RIGHT);
    GLUI_Listbox *glyphTypeList = new GLUI_Listbox(visualization_options, "Glyph ", (int*) &glyphType, GLYPH_TYPE_LIST, buttonHandler);
    glyphTypeList->set_alignment(GLUI_ALIGN_RIGHT);
    glyphTypeList->add_item(Visualization::SIMPLE_ARROWS, "Simple Arrows");
    glyphTypeList->add_item(Visualization::CONES_3D, "3D Cones");
    glyphTypeList->add_item(Visualization::ARROWS_3D, "3D Arrows");
    glui->add_statictext_to_panel(visualization_options, "                                              ");

    GLUI_Spinner *dim_spinner = new GLUI_Spinner(glui, "DIM ", &dim, DIM_SPINNER, buttonHandler);
    dim_spinner->set_alignment(GLUI_ALIGN_RIGHT);
    dim_spinner->set_int_limits(40, 80, GLUI_LIMIT_CLAMP);

    GLUI_Spinner *sample_x_spinner = new GLUI_Spinner(glui, "X Sample ", &sample_x, SAMPLE_X_SPINNER, buttonHandler);
    sample_x_spinner->set_alignment(GLUI_ALIGN_RIGHT);
    sample_x_spinner->set_int_limits(10, 100, GLUI_LIMIT_CLAMP);
    GLUI_Spinner *sample_y_spinner = new GLUI_Spinner(glui, "Y Sample ", &sample_y, SAMPLE_Y_SPINNER, buttonHandler);
    sample_y_spinner->set_alignment(GLUI_ALIGN_RIGHT);
    sample_y_spinner->set_int_limits(10, 100, GLUI_LIMIT_CLAMP);

    // quit
    GLUI_Button *quit = new GLUI_Button(glui, "Quit", QUIT_BUTTON, buttonHandler);
    quit->set_alignment(GLUI_ALIGN_CENTER);

}

void Application::quit() {
    cout << "Quit.\n";
    exit(0);
}
