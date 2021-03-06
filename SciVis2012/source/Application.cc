#include "Application.h"
#include <iostream>
#include <GLUT/glut.h>
#include <GLUI/glui.h>
#include <map>
#include <sstream>
#include <string>
using namespace std;

Simulation Application::simulation; // the smoke simulation
Visualization Application::visualization; // visualization of the simulation
DataBuffer Application::timeslices(10, 40);

//size of the graphics window, in pixels
int Application::winWidth;
int Application::winHeight;

GLUI* Application::glui; // user interface
int Application::main_window;
int Application::menu_window;

Visualization::ColorMode Application::selectedColormap;
std::map<Visualization::ColorMode, Colormap*> Application::colormaps;
Colormap* Application::colormap;
int Application::selectedNumOfColors;
float Application::hueValue;
float Application::saturationValue;
float Application::densityIsoline;
float Application::densityRHO1Isoline;
float Application::densityRHO2Isoline;
int Application::numberIsolines;
int Application::angle;
int Application::translate_x;
int Application::translate_y;
int Application::translate_z;
int Application::seed_x = 500;
int Application::seed_y = 500;
int Application::seed_z = 0;
int Application::distance;
Visualization::DatasetType Application::scalarDataset;
float Application::scalarMax;
float Application::scalarMin;
GLUI_Panel* Application::streamtube_options;
Visualization::DatasetType Application::vectorDataset;
Visualization::DatasetType Application::heightplotDataset;
Visualization::GlyphType Application::glyphType;
int Application::dim;
int Application::numberOfSegments;
int Application::sample_x;
int Application::sample_y;

Visualization::Mode Application::scalarMode;

void Application::update() {
    glutSetWindow(main_window);
    simulation.do_one_simulation_step();
    timeslices.addTimeslice(simulation.rho, simulation.vx, simulation.vy, simulation.fx, simulation.fy);
    glutPostRedisplay();
}

void Application::updateMenu() {
    glutSetWindow(menu_window);
    glutPostRedisplay();
}

void Application::displayMenu() {
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glFlush();
    glutSwapBuffers();
}

void Application::initialize(int *argc, char** argv) {
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(450, 650);

    menu_window = glutCreateWindow("Options");
    glutDisplayFunc(displayMenu);
    glutInitWindowSize(1000, 800);

    main_window = glutCreateWindow("Real-time smoke simulation and visualization");

    // pass static functions as callback to GLUT
    glutDisplayFunc(display);
    glutMotionFunc(drag);
    glutMouseFunc(click);
    glutSpecialFunc(special);
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

    densityIsoline = visualization.getDensityIsoline();
    numberOfSegments = visualization.getNumSegmentsStreamtubes();
    dim = simulation.get_DIM();

    sample_x = visualization.getSampleX();
    sample_y = visualization.getSampleY();

    initUI();
    glui->sync_live();

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
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, (GLsizei) winWidth, (GLsizei) winHeight);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat) winWidth / winHeight,
            1.0, 10000.0);

    glEnable(GL_LIGHTING); // so the renderer considers light
    glEnable(GL_LIGHT0); // turn LIGHT0 on
    glEnable(GL_DEPTH_TEST); // so the renderer considers depth
    glEnable(GL_COLOR_MATERIAL); // to be able to color objects when lighting is on
    //glEnable(GL_NORMALIZE);

    if (visualization.options[Visualization::SMOOTH_SHADING]) {
        glShadeModel(GL_SMOOTH);
    } else {
        glShadeModel(GL_FLAT);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    gluLookAt(0.0, 1500.0 - distance, 1000.0 - distance,
            0.0 + translate_x, 0.0 + translate_y, 0.0 + translate_z,
            0.0, 1.0, 0.0);

    GLfloat black[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat cyan[] = {0.0, 1.0, 1.0, 1.0};
    GLfloat white[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat direction[] = {0.0, 1000.0, 0, 0.0};



    glPushMatrix();
    glTranslatef(0.0 + translate_x, 0.0 + translate_y, 0.0 + translate_z);
    glRotatef(angle, 1, 0, 0);
    if (visualization.options[Visualization::DRAW_FIXPOINT]) {
        glPushMatrix();
        glutSolidSphere(10, 10, 10);
        glTranslatef(-600, 1000, 500);
        glutSolidSphere(10, 10, 10);
        glTranslatef(0, 0, -1000);
        glutSolidSphere(10, 10, 10);
        glTranslatef(1200, 0, 0);
        glutSolidSphere(10, 10, 10);
        glTranslatef(0, 0, 1000);
        glutSolidSphere(10, 10, 10);
        glPopMatrix();
    }
    glBegin(GL_QUAD_STRIP);
    glColor3f(.8, .8, .8);
    glNormal3f(0, -1, 0);
    glVertex3f(-10000, -1000, -10000);

    glNormal3f(0, -1, 0);
    glVertex3f(-10000, -1000, 10000);

    glNormal3f(0, -1, 0);
    glVertex3f(10000, -1000, -10000);

    glNormal3f(0, -1, 0);
    glVertex3f(10000, -1000, 10000);
    glEnd();

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, cyan);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 30);

    glLightfv(GL_LIGHT0, GL_AMBIENT, black);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);
    glLightfv(GL_LIGHT0, GL_POSITION, direction);



    glPushMatrix();
    glScalef(-1, 1, 1);
    glRotatef(180, 0, 1, 0);
    glTranslatef(-winWidth / 2, 0, -winHeight / 2);
    visualization.visualize3D(simulation, winWidth, winHeight);
    glPopMatrix();
    glPopMatrix();
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING); // so the renderer considers light

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (GLdouble) winWidth, 0.0, (GLdouble) winHeight, -10, 10);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    visualization.visualize(simulation, winWidth, winHeight);
    renderColormap();

    glFlush();
    glutSwapBuffers();
    GLenum error = glGetError();
    if (error != 0) {
        std::cout << error << '\n';
    }
}

void Application::renderColormap() {
    glDisable(GL_LIGHTING);
    float min = 0.0;
    float max = 0.0;
    switch (scalarMode) {
        case Visualization::CLAMPING:
            min = visualization.getScalarMin();
            max = visualization.getScalarMax();
            break;
        case Visualization::SCALING:
            min = visualization.getScalarScaleMin();
            max = visualization.getScalarScaleMax();
            break;
    }
    visualization.getColormap()->render(min, max, 5);
    glEnable(GL_LIGHTING);
}

//reshape: Handle window resizing (reshaping) events

void Application::reshape(int w, int h) {
    winWidth = w;
    winHeight = h;
}

void Application::special(int key, int x, int y) {
    int magnitude = 25;
    switch (key) {
        case GLUT_KEY_UP:
            translate_z += magnitude;
            break;
        case GLUT_KEY_DOWN:
            translate_z -= magnitude;
            break;
        case GLUT_KEY_LEFT:
            translate_x += magnitude;
            break;
        case GLUT_KEY_RIGHT:
            translate_x -= magnitude;
            break;
    }

    GLUI_Master.sync_live_all();
}
//keyboard: Handle key presses

void Application::keyboard(unsigned char key, int x, int y) {
    int magnitude = 25;
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
        case 'u':
            angle = (angle + 1) % 360;
            break;
        case 'j':
            angle = (angle - 1) % 360;
            break;
        case 'i':
            translate_y += magnitude;
            break;
        case 'k':
            translate_y -= magnitude;
            break;
        case 'n':
            distance += 100;
            break;
        case 'm':
            distance -= 100;
            break;
        default:
        {
        }
            break;
    }

    // sync live variables

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
        case DENSITY_ISOLINE_SPINNER:
            visualization.setDensityIsoline(densityIsoline);
            break;
        case DENSITY_RHO1_ISOLINE_SPINNER:
            if (densityRHO1Isoline >= densityRHO2Isoline) {
                densityRHO1Isoline = densityRHO2Isoline;
            }
            visualization.setDensityRHO1Isoline(densityRHO1Isoline);
            break;
        case DENSITY_RHO2_ISOLINE_SPINNER:
            visualization.setDensityRHO2Isoline(densityRHO2Isoline);
            break;
        case NUMBER_ISOLINES_SPINNER:
            visualization.setNumIsolines(numberIsolines);
            break;
        case HEIGHTPLOT_DATASET_LIST:
            visualization.setHeightplotDataset(heightplotDataset);
            break;
        case SEGMENT_SPINNER:
            visualization.setNumSegmentsStreamtubes(numberOfSegments);
            break;
        case ADD_SEEDPOINT_BUTTON:
            visualization.addSeedpoint(seed_x, seed_y, seed_z);
            stringstream pointtext(stringstream::in | stringstream::out);
            pointtext << seed_x << ", " << seed_y << ", " << seed_z;
            glui->add_statictext_to_panel(streamtube_options, pointtext.str().c_str());
            break;
    }
    glui->sync_live();
}

void Application::initUI() {
    // main window
    glui = GLUI_Master.create_glui_subwindow(menu_window, GLUI_SUBWINDOW_TOP);
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
    scalarDatasetsGroup->add_item(Visualization::VELOCITY_DIV, "Div Velocity");
    scalarDatasetsGroup->add_item(Visualization::FORCE_DIV, "Div Force");

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

    GLUI_Listbox *glyphTypeList = new GLUI_Listbox(datasetOptions, "Glyph ", (int*) &glyphType, GLYPH_TYPE_LIST, buttonHandler);
    glyphTypeList->set_alignment(GLUI_ALIGN_LEFT);
    glyphTypeList->add_item(Visualization::HEDGEHOGS, "Hedgehogs");
    glyphTypeList->add_item(Visualization::SIMPLE_ARROWS, "Simple Arrows");
    glyphTypeList->add_item(Visualization::CONES_3D, "3D Cones");
    glyphTypeList->add_item(Visualization::ARROWS_3D, "3D Arrows");
    glui->add_statictext_to_panel(datasetOptions, "                                              ");

    glui->add_statictext_to_panel(datasetOptions, "                                              ");

    // contouring
    GLUI_Panel *contouring_options = new GLUI_Panel(glui, "Contouring");
    contouring_options->set_alignment(GLUI_ALIGN_LEFT);

    GLUI_Spinner *density_isoline_spinner = new GLUI_Spinner(contouring_options, "Density rho ", &densityIsoline, DENSITY_ISOLINE_SPINNER, buttonHandler);
    density_isoline_spinner->set_alignment(GLUI_ALIGN_RIGHT);

    glui->add_separator_to_panel(contouring_options);
    GLUI_Spinner *density_rho1_isoline_spinner = new GLUI_Spinner(contouring_options, "Density rho1 ", &densityRHO1Isoline, DENSITY_RHO1_ISOLINE_SPINNER, buttonHandler);
    density_rho1_isoline_spinner->set_alignment(GLUI_ALIGN_RIGHT);
    GLUI_Spinner *density_rho2_isoline_spinner = new GLUI_Spinner(contouring_options, "Density rho2 ", &densityRHO2Isoline, DENSITY_RHO2_ISOLINE_SPINNER, buttonHandler);
    density_rho2_isoline_spinner->set_alignment(GLUI_ALIGN_RIGHT);
    GLUI_Spinner *number_isolines_spinner = new GLUI_Spinner(contouring_options, "N Isolines ", &numberIsolines, NUMBER_ISOLINES_SPINNER, buttonHandler);
    number_isolines_spinner->set_alignment(GLUI_ALIGN_RIGHT);

    GLUI_Checkbox *isoline_colorize_box = new GLUI_Checkbox(contouring_options, "Colorize ", &visualization.options[Visualization::COLORIZE]);
    isoline_colorize_box->set_alignment(GLUI_ALIGN_RIGHT);
    glui->add_statictext_to_panel(contouring_options, "                                              ");


    GLUI_Panel *heightplot_options = new GLUI_Panel(glui, "Heightplot");
    GLUI_Listbox *heightplotDatasetsGroup = new GLUI_Listbox(heightplot_options, "Dataset ", (int*) &heightplotDataset, HEIGHTPLOT_DATASET_LIST, buttonHandler);
    heightplotDatasetsGroup->set_alignment(GLUI_ALIGN_RIGHT);
    heightplotDatasetsGroup->add_item(Visualization::DENSITY, "Density rho");
    heightplotDatasetsGroup->add_item(Visualization::VELOCITY_MAGN, "Velocity |v|");
    heightplotDatasetsGroup->add_item(Visualization::FORCE_MAGN, "Force |f|");
    GLUI_Checkbox *normals_box = new GLUI_Checkbox(heightplot_options, "Draw Normals", &visualization.options[Visualization::DRAW_NORMALS]);
    normals_box->set_alignment(GLUI_ALIGN_RIGHT);
    glui->add_statictext_to_panel(heightplot_options, "                                              ");




    GLUI_Spinner *dim_spinner = new GLUI_Spinner(glui, "DIM ", &dim, DIM_SPINNER, buttonHandler);
    dim_spinner->set_alignment(GLUI_ALIGN_RIGHT);
    dim_spinner->set_int_limits(10, 100, GLUI_LIMIT_CLAMP);

    GLUI_Spinner *sample_x_spinner = new GLUI_Spinner(glui, "X Sample ", &sample_x, SAMPLE_X_SPINNER, buttonHandler);
    sample_x_spinner->set_alignment(GLUI_ALIGN_RIGHT);
    sample_x_spinner->set_int_limits(10, 100, GLUI_LIMIT_CLAMP);
    GLUI_Spinner *sample_y_spinner = new GLUI_Spinner(glui, "Y Sample ", &sample_y, SAMPLE_Y_SPINNER, buttonHandler);
    sample_y_spinner->set_alignment(GLUI_ALIGN_RIGHT);
    sample_y_spinner->set_int_limits(10, 100, GLUI_LIMIT_CLAMP);

    glui->add_column(false);
    // visualization technique
    GLUI_Panel *visualization_options = new GLUI_Panel(glui, "Options");
    visualization_options->set_alignment(GLUI_ALIGN_LEFT);
    GLUI_Checkbox *glyphs_box = new GLUI_Checkbox(visualization_options, "Glyphs", &visualization.options[Visualization::DRAW_GLYPHS]);
    glyphs_box->set_alignment(GLUI_ALIGN_LEFT);
    GLUI_Checkbox *smoke_box = new GLUI_Checkbox(visualization_options, "Smoke", &visualization.options[Visualization::DRAW_SMOKE]);
    smoke_box->set_alignment(GLUI_ALIGN_LEFT);
    GLUI_Checkbox *isoline_box = new GLUI_Checkbox(visualization_options, "Isolines ", &visualization.options[Visualization::DRAW_ISOLINES]);
    isoline_box->set_alignment(GLUI_ALIGN_LEFT);
    GLUI_Checkbox *height_box = new GLUI_Checkbox(visualization_options, "Heightplot ", &visualization.options[Visualization::DRAW_HEIGHTPLOT]);
    height_box->set_alignment(GLUI_ALIGN_LEFT);
    GLUI_Checkbox *streamtubes_box = new GLUI_Checkbox(visualization_options, "Streamtubes ", &visualization.options[Visualization::DRAW_STREAMTUBES]);
    streamtubes_box->set_alignment(GLUI_ALIGN_LEFT);
    GLUI_Checkbox *orient = new GLUI_Checkbox(visualization_options, "Orientation Point", &visualization.options[Visualization::DRAW_FIXPOINT]);
    orient->set_alignment(GLUI_ALIGN_LEFT);
    GLUI_Checkbox *shading_box = new GLUI_Checkbox(visualization_options, "Smooth Shading", &visualization.options[Visualization::SMOOTH_SHADING]);
    orient->set_alignment(GLUI_ALIGN_LEFT);
    glui->add_statictext_to_panel(visualization_options, "                                              ");



    streamtube_options = new GLUI_Panel(glui, "Streamtubes");
    streamtube_options->set_alignment(GLUI_ALIGN_LEFT);
    GLUI_Checkbox *field3dbox = new GLUI_Checkbox(streamtube_options, "Show 3D Field", &visualization.options[Visualization::DRAW_3DFIELD]);
    field3dbox->set_alignment(GLUI_ALIGN_LEFT);
    GLUI_Checkbox *thickTubes = new GLUI_Checkbox(streamtube_options, "Thick Tubes", &visualization.options[Visualization::DRAW_THICKTUBES]);
    thickTubes->set_alignment(GLUI_ALIGN_LEFT);
    GLUI_Spinner *segment_spinner = new GLUI_Spinner(streamtube_options, "Surface Segments ", &numberOfSegments, SEGMENT_SPINNER, buttonHandler);
    segment_spinner->set_alignment(GLUI_ALIGN_RIGHT);
    segment_spinner->set_int_limits(3, 20, GLUI_LIMIT_CLAMP);
    glui->add_separator_to_panel(streamtube_options);
    GLUI_Spinner *seedx_spinner = new GLUI_Spinner(streamtube_options, "x", &seed_x, SEED_X, buttonHandler);
    seedx_spinner->set_alignment(GLUI_ALIGN_CENTER);
    GLUI_Spinner *seedy_spinner = new GLUI_Spinner(streamtube_options, "y", &seed_y, SEED_Y, buttonHandler);
    seedy_spinner->set_alignment(GLUI_ALIGN_CENTER);
    GLUI_Spinner *seedz_spinner = new GLUI_Spinner(streamtube_options, "y", &seed_z, SEED_Z, buttonHandler);
    seedz_spinner->set_alignment(GLUI_ALIGN_CENTER);
    GLUI_Button *add_seedpointbutton = new GLUI_Button(streamtube_options, "Add Seedpoint", ADD_SEEDPOINT_BUTTON, buttonHandler);
    add_seedpointbutton->set_alignment(GLUI_ALIGN_CENTER);
    glui->add_separator_to_panel(streamtube_options);



    glui->add_statictext_to_panel(streamtube_options, "                                              ");


    // quit
    GLUI_Button *quit = new GLUI_Button(glui, "Quit", QUIT_BUTTON, buttonHandler);
    quit->set_alignment(GLUI_ALIGN_CENTER);
}

 void Application::click(int button,int state,int x, int y) {
     seed_x = x;
     seed_y = y;
     glui->sync_live();
     
 }

void Application::quit() {
    cout << "Quit.\n";
    exit(0);
}
