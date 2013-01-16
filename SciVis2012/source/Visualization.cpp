#include <algorithm>
#include <cmath>
#include <GLUT/glut.h>
#include <iostream>
#include "Visualization.h"
#include "Simulation.h"
#include "Utilities.h"
#include "Colormap.h"
#include "Application.h"

using namespace std;

//std::map<Visualization::ColorMode, Colormap*> Visualization::colormaps;

Visualization::Visualization() {
    vec_scale = 1000;
    sample_x = 20;
    sample_y = 20;
    scalarDataset = DENSITY;
    vectorDataset = VELOCITY;
    options[UseDirectionColoring] = false; // not used for now
    options[DrawSmoke] = false;
    options[DrawGlyphs] = true;
    options[DrawVectorField] = true; // not used for now

    datasets.insert(std::make_pair(DENSITY, Dataset(0, 1, CLAMPING)));
    datasets.insert(std::make_pair(VELOCITY_MAGN, Dataset(0.01, 0.08, CLAMPING)));
    datasets.insert(std::make_pair(FORCE_MAGN, Dataset(0, 0.15, CLAMPING)));
    datasets.insert(std::make_pair(FORCE, Dataset(0, 1, CLAMPING)));
    datasets.insert(std::make_pair(VELOCITY, Dataset(0, 1, CLAMPING)));
}

void Visualization::initializeColormaps() {

    Colormap* rainbow = new Colormap();
    rainbow->putColor(RED, 255);
    rainbow->putColor(YELLOW, 191);
    rainbow->putColor(GREEN, 127);
    rainbow->putColor(AQUA, 63);
    rainbow->putColor(BLUE, 0);

    Colormap* heatmap = new Colormap();
    heatmap->putColor(RED, 63);
    heatmap->putColor(YELLOW, 191);
    heatmap->putColor(WHITE, 255);
    heatmap->putColor(BLACK, 0);

    Colormap* bwgradient = new Colormap();
    bwgradient->putColor(WHITE, 255);
    bwgradient->putColor(BLACK, 0);
    bwgradient->setSaturation(0);

    Colormap* bgradient = new Colormap();
    bgradient->putColor(MAGENTA, 255);
    bgradient->putColor(BLACK, 0);

    Colormap* wgradient = new Colormap();
    wgradient->putColor(MAGENTA, 255);
    wgradient->putColor(WHITE, 0);

    Colormap* byg = new Colormap();
    byg->putColor(GREEN, 255);
    byg->putColor(YELLOW, 127);
    byg->putColor(BLUE, 0);

    Colormap* zebra = new Colormap();
    for (int i = 0; i < 256; i = i + 64) {
        for (int j = 0; j < 32; j++) {
            zebra->putColor(WHITE, i + j);
        }
    }
    for (int i = 32; i < 256; i = i + 64) {
        for (int j = 0; j < 32; j++) {
            zebra->putColor(BLACK, i + j);
        }
    }

    colormaps.insert(make_pair(Visualization::RAINBOW, rainbow));
    colormaps.insert(make_pair(Visualization::HEATMAP, heatmap));
    colormaps.insert(make_pair(Visualization::GRAYSCALE, bwgradient));
    colormaps.insert(make_pair(Visualization::BGRADIENT, bgradient));
    colormaps.insert(make_pair(Visualization::WGRADIENT, wgradient));
    colormaps.insert(make_pair(Visualization::BLUEYELLOWGREEN, byg));
    colormaps.insert(make_pair(Visualization::ZEBRA, zebra));
}

//
//void Visualization::set_scalar_col(ColorMode colorMode)
//{
//    scalar_col = colorMode;
//}

void Visualization::toggle(Option option) {
    options[option] = !options[option];
}

void Visualization::enable(Option option) {
    options[option] = true;
}

void Visualization::disable(Option option) {
    options[option] = false;
}

bool Visualization::is_enabled(Option option) const {
    return options[option];
}

void Visualization::scale_hedgehogs(float scale) {
    vec_scale *= scale;
}

float Visualization::hedgehog_scale() const {
    return vec_scale;
}

void Visualization::set_hue(const float h) {
    hue = h;
}

void Visualization::set_saturation(const float s) {
    saturation = s;
}

void Visualization::setScalarDataset(DatasetType sdm) {
    scalarDataset = sdm;
}

void Visualization::setVectorDataset(DatasetType vdm) {
    vectorDataset = vdm;
}

void Visualization::set_sample_x(int x) {
    sample_x = x;
}

void Visualization::set_sample_y(int y) {
    sample_y = y;
}
//
////rainbow: Implements a color palette, mapping the scalar 'value' to a rainbow color RGB
//void Visualization::rainbow(float value, float* R, float* G, float* B)
//{
//    const float dx=0.8;
//    if (value<0) value=0; if (value>1) value=1;
//    value = (6-2*dx)*value+dx;	
//    *R = max(0.0, (3-fabs(value-4)-fabs(value-5)) / 2.0);
//    *G = max(0.0, (4-fabs(value-2)-fabs(value-4)) / 2.0);
//    *B = max(0.0, (3-fabs(value-1)-fabs(value-2)) / 2.0);
//}
//
//void Visualization::grayscale(float value, float* R, float* G, float* B)
//{
//    if (value<0) value=0; if (value>1) value=1;
//    *R = *G = *B = value;
//}
//
//void Visualization::custom(float value, float* R, float* G, float* B)
//{
//    if (value<0) value=0; if (value>1) value=1;
//    *R = 1;
//    *G = *B = value;
//}

//set_colormap: Sets three different types of colormaps

void Visualization::setColor(float vy, ColorType t) {
    Dataset dataset = datasets[scalarDataset];
    float colorIndex = 0;
    switch (dataset.mode) {
        case CLAMPING:
            if (vy > dataset.max) vy = dataset.max;
            if (vy < dataset.min) vy = dataset.min;
            switch (t) {
                case TEXTURE:
                    colorIndex = scale(vy, dataset.min, dataset.max, 0, 1);
                    break;
                case SIMPLE:
                    colorIndex = scale(vy, dataset.min, dataset.max, 0, 255);
                    break;
            }
            break;

        case SCALING:
            if (vy > dataset.scaleMax) datasets[scalarDataset].scaleMax = vy;
            if (vy < dataset.scaleMin) datasets[scalarDataset].scaleMin = vy;
            switch (t) {
                case TEXTURE:
                    colorIndex = scale(vy, dataset.scaleMin, dataset.scaleMax, 0, 1);
                    break;
                case SIMPLE:
                    colorIndex = scale(vy, dataset.scaleMin, dataset.scaleMax, 0, 255);
                    break;
            }
            break;
    }
    switch (t) {
        case TEXTURE:
            glTexCoord1f(colorIndex);
            break;
        case SIMPLE:
            RGB color = colormap->getColorAt(colorIndex);
            glColor3f(color.red, color.green, color.blue);
            break;
    }
}

Colormap* Visualization::getColormap() {
    return colormap;
}

Colormap* Visualization::loadColormap(ColorMode cm) {
    colormap = colormaps[cm];
    return colormap;
}


//visualize: This is the main visualization function

void Visualization::visualize(Simulation const &simulation, int winWidth, int winHeight) {
    const int DIM = Simulation::DIM;
    fftw_real wn = (fftw_real) winWidth / (fftw_real) (DIM + 1); // Computational Grid cell width
    fftw_real hn = (fftw_real) winHeight / (fftw_real) (DIM + 1); // Computational Grid cell heigh

    fftw_real wn_sample = (fftw_real) winWidth / (fftw_real) (sample_x + 1); // Sample Grid cell width 
    fftw_real hn_sample = (fftw_real) winHeight / (fftw_real) (sample_y + 1); // Sample Grid cell heigh

    if (options[DrawSmoke]) {
        glEnable(GL_TEXTURE_1D);
        draw_smoke(simulation, DIM, wn, hn);
        glDisable(GL_TEXTURE_1D);
    }
    if (options[DrawGlyphs]) {
        draw_glyphs(simulation, DIM, wn, hn, wn_sample, hn_sample);
    }
}

void Visualization::draw_smoke(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn) {
    int i, j, idx;
    double px, py;

    GLenum error = glGetError();
    if (error != 0) {
        std::cout << error << '\n';
    }
    colormap->loadColormapTexture();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    for (j = 0; j < DIM - 1; j++) //draw smoke
    {
        glBegin(GL_TRIANGLE_STRIP);

        i = 0;
        px = wn + (fftw_real) i * wn;
        py = hn + (fftw_real) j * hn;
        idx = (j * DIM) + i;
        setColor(pick_scalar_field_value(simulation, idx), TEXTURE);
        glVertex2f(px, py);

        for (i = 0; i < DIM - 1; i++) {
            px = wn + (fftw_real) i * wn;
            py = hn + (fftw_real) (j + 1) * hn;
            idx = ((j + 1) * DIM) + i;
            setColor(pick_scalar_field_value(simulation, idx), TEXTURE);
            glVertex2f(px, py);
            px = wn + (fftw_real) (i + 1) * wn;
            py = hn + (fftw_real) j * hn;
            idx = (j * DIM) + (i + 1);
            setColor(pick_scalar_field_value(simulation, idx), TEXTURE);
            glVertex2f(px, py);
        }

        px = wn + (fftw_real) (DIM - 1) * wn;
        py = hn + (fftw_real) (j + 1) * hn;
        idx = ((j + 1) * DIM) + (DIM - 1);
        setColor(pick_scalar_field_value(simulation, idx), TEXTURE);
        glVertex2f(px, py);
        glEnd();
    }
}

void Visualization::draw_glyphs(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn, const fftw_real wn_sample, const fftw_real hn_sample) {
    switch (glyphType) {
        case SIMPLE_ARROWS:
            draw_simple_arrows(simulation, DIM, wn, hn);
            break;
        case CONES_3D:
            draw_3d_cones(simulation, DIM, wn, hn);
            break;
        case ARROWS_3D:
            draw_3d_arrows(simulation, DIM, wn, hn);
            break;
    }
}

void Visualization::draw_simple_arrows(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn) {
    glEnable(GL_TEXTURE_1D);

    colormap->loadColormapTexture();

    size_t idx;
    GLfloat magn;
    float *values = new float[2];

    for (int i = 0; i < DIM; i++)
        for (int j = 0; j < DIM; j++) {
            idx = (j * DIM) + i;
            pick_vector_field_value(simulation, idx, values);

            magn = magnitude(values);
            magn = pick_scaled_field(magn);

            GLfloat x = values[0];
            GLfloat y = values[1];
            GLfloat angle = 0.0;

            angle = atan2(y, x) * 180 / M_PI;

            GLfloat x_start = wn + (fftw_real) i * wn;
            GLfloat y_start = hn + (fftw_real) j * hn;

            GLfloat scale_x = magn * 1.5;
            GLfloat scale_y_quad = magn * 0.2;
            GLfloat scale_y_triangle = magn * 0.3;

            glPushMatrix();
            glTranslatef(x_start, y_start, 0.0);
            glRotatef(angle, 0.0, 0.0, 1.0f);
            glTranslatef(-x_start, -y_start, 0.0);
            setColor(pick_scalar_field_value(simulation, idx), TEXTURE);
            glTranslatef(x_start, y_start, 0);
            glBegin(GL_QUADS);
            glVertex2f(1 + scale_x, 0.5 + scale_y_quad);
            glVertex2f(0, 0.5 + scale_y_quad);
            glVertex2f(0, -0.5 - scale_y_quad);
            glVertex2f(1 + scale_x, -0.5 - scale_y_quad);
            glEnd();
            glTranslatef(1 + scale_x, 0, 0);
            glBegin(GL_TRIANGLES);
            glVertex2f(1 + scale_x, 0);
            glVertex2f(0, 2 + scale_y_triangle);
            glVertex2f(0, -2 - scale_y_triangle);
            glEnd();
            glPopMatrix();
        }
    glDisable(GL_TEXTURE_1D);
}

void Visualization::draw_3d_cones(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn) {

    //    float *sample_values = new float[2];
    //    float *sample_values_x1_y1 = new float[2];
    //    float *sample_values_x2_y2 = new float[2];
    //    float *sample_values_x3_y3 = new float[2];
    ////    float *sample_values_x4_y4 = new float[2];
    //    int idx_x1_y1, idx_x2_y2, idx_x3_y3, idx_x4_y4;

    // construct a grid with computational points
    //    for (i = 0; i < DIM; i++)
    //        for (j = 0; j < DIM; j++) {
    //            idx = (j * DIM) + i;
    //            GLfloat x_start = wn + (fftw_real) i * wn;
    //            GLfloat y_start = hn + (fftw_real) j * hn;
    //            glBegin(GL_POINTS);
    //            // glColor3f(255, 255, 255);
    //            glVertex2f(x_start, y_start);
    //            glEnd();
    //        }

    // draw the glyphs on the sample points. if the sample points do not coincide 
    // with the grid points then use interpolation
    //    for (i = 0; i < sample_x; i++)
    //        for (j = 0; j < sample_y; j++) {
    //            idx = (j * sample_y) + i;
    //            pick_vector_field_value(simulation, idx, sample_values);
    //            GLfloat x_start = wn_sample + (fftw_real) i * wn_sample;
    //            GLfloat y_start = hn_sample + (fftw_real) j * hn_sample;
    //            // interpolate the sample values with the values of the computational grid points
    //            // divide the x,y of the sample point with the step of the computational grid (wn or hn),
    //            // ceil and then we get the coordinates of the 4 points of the computational grid to use
    //            // for bilinear interpolation
    //            int x_point = ceil(x_start / wn);
    //            int y_point = ceil(y_start / hn);
    //            idx_x1_y1 = ((y_point - 1) * DIM) + (x_point - 1);
    //            idx_x2_y2 = ((y_point - 2) * DIM) + (x_point - 1);
    //            idx_x3_y3 = ((y_point - 1) * DIM) + (x_point - 2);
    //            idx_x4_y4 = ((y_point - 2) * DIM) + (x_point - 2);
    //            pick_vector_field_value(simulation, idx_x1_y1, sample_values_x1_y1);
    //            pick_vector_field_value(simulation, idx_x2_y2, sample_values_x2_y2);
    //            pick_vector_field_value(simulation, idx_x3_y3, sample_values_x3_y3);
    //            pick_vector_field_value(simulation, idx_x4_y4, sample_values_x4_y4);
    //            // bilinear interpolation
    //            GLfloat x1, y1, x2, y2;
    //            x1 = wn + (fftw_real) (x_point - 1) * wn;
    //            y1 = hn + (fftw_real) (y_point - 1) * hn;
    //            x2 = wn + (fftw_real) (x_point - 2) * wn;
    //            y2 = hn + (fftw_real) (y_point - 2) * hn;
    //            GLfloat x, y;
    //            GLfloat f1x = sample_values_x4_y4[0]*(x2 - x_start)*(y2 - y_start);
    //            GLfloat f2x = sample_values_x2_y2[0]*(x_start - x1)*(y2 - y_start);
    //            GLfloat f3x = sample_values_x3_y3[0]*(x2 - x_start)*(y_start - y1);
    //            GLfloat f4x = sample_values_x1_y1[0]*(x_start - x1)*(y_start - y1);
    //            x = (1 / ((x2 - x1)*(y2 - y1)))*(f1x + f2x + f3x + f4x);
    //            GLfloat f1y = sample_values_x4_y4[1]*(x2 - x_start)*(y2 - y_start);
    //            GLfloat f2y = sample_values_x2_y2[1]*(x_start - x1)*(y2 - y_start);
    //            GLfloat f3y = sample_values_x3_y3[1]*(x2 - x_start)*(y_start - y1);
    //            GLfloat f4y = sample_values_x1_y1[1]*(x_start - x1)*(y_start - y1);
    //            y = (1 / ((x2 - x1)*(y2 - y1)))*(f1y + f2y + f3y + f4y);
    //            GLfloat angle = atan2(y, x) * 180 / M_PI;
    //            magn = magnitude(x, y);
    //            magn = pick_scaled_field(magn);
    //            glPushMatrix();
    //            glTranslatef(x_start, y_start, 0.0);
    //            glRotatef(angle, 0.0, 0.0, 1.0f);
    //            glTranslatef(-x_start, -y_start, 0.0);
    //            // draw the glyph (this needs to be refactored in order to draw other glyphs)
    //            // the glyphs need to be designed to scale, then we can use a scaling factor
    //            // for now I hardcoded the glyphs
    //            glBegin(GL_POLYGON);
    //            setColor(pick_scalar_field_value(simulation, idx));
    //            glVertex2f(x_start, y_start + 1);
    //            glVertex2f(x_start + magn, y_start + 1);
    //            glVertex2f(x_start + magn, y_start + 2);
    //            glVertex2f(x_start + magn + 3, y_start);
    //            glVertex2f(x_start + magn, y_start - 2);
    //            glVertex2f(x_start + magn, y_start - 1);
    //            glVertex2f(x_start, y_start - 1);
    //            glEnd();
    //            glPopMatrix();
    //            glBegin(GL_POINTS);
    //            //glColor3f(255, 0, 0);
    //            glVertex2f(x_start, y_start);
    //            glEnd();
    //        }
    //       int i, j, idx;
    //    float magn;
    //    float *values = new float[2];
    //    
    //    //glDisable(GL_TEXTURE);
    //    glPushMatrix();
    //    glTranslatef(300,300,0);
    //    glColor3f(1,1,0);
    //    glRotatef(-90, 1.0, 0.0,0.0f);
    //    glutSolidCone(100, 100, 10,10);
    //    
    //    glBegin(GL_QUADS);
    //    glVertex3f(100,100, 0);
    //    glVertex3f(100,200,0);
    //    glVertex3f(200,200,0);
    //    glVertex3f(200,100,0);
    //    glVertex3f(100,200,0);
    //    glVertex3f(100,200,200);
    //    glVertex3f(200,200,200);
    //    glVertex3f(200,200,0);
    //    glEnd();
    //
    //    glPopMatrix();
    
    size_t idx;
    GLfloat magn;
    float *values = new float[2];

    for (int i = 0; i < DIM; i++)
        for (int j = 0; j < DIM; j++) {
            idx = (j * DIM) + i;
            pick_vector_field_value(simulation, idx, values);

            magn = magnitude(values);
            magn = pick_scaled_field(magn);

            GLfloat x = values[0];
            GLfloat y = values[1];
            GLfloat angle = 0.0;

            angle = atan2(y, x) * 180 / M_PI;

            GLfloat x_start = wn + (fftw_real) i * wn;
            GLfloat y_start = hn + (fftw_real) j * hn;
            
            GLfloat base_scale = magn;
            GLfloat height_scale = magn * 2;

            glPushMatrix();
            setColor(pick_scalar_field_value(simulation, idx), SIMPLE);
            glTranslatef(x_start, y_start, 0.0);
            glRotatef(angle, 0.0, 0.0, 1.0);
            glRotatef(90, 0.0, 1.0, 0.0);
            glutSolidCone(0.5 + base_scale, 5 + height_scale, 50, 10);
            glPopMatrix();
        }
}

void Visualization::draw_3d_arrows(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn) {
    GLUquadricObj *p = gluNewQuadric();
    gluQuadricDrawStyle(p, GLU_FILL);
    
    size_t idx;
    GLfloat magn;
    float *values = new float[2];

    for (int i = 0; i < DIM; i++)
        for (int j = 0; j < DIM; j++) {
            idx = (j * DIM) + i;
            pick_vector_field_value(simulation, idx, values);

            magn = magnitude(values);
            magn = pick_scaled_field(magn);

            GLfloat x = values[0];
            GLfloat y = values[1];
            GLfloat angle = 0.0;

            angle = atan2(y, x) * 180 / M_PI;

            GLfloat x_start = wn + (fftw_real) i * wn;
            GLfloat y_start = hn + (fftw_real) j * hn;
            
            GLfloat base_scale = magn;
            GLfloat height_scale = magn * 2;

            glPushMatrix();
            setColor(pick_scalar_field_value(simulation, idx), SIMPLE);
            glTranslatef(x_start, y_start, 0.0);
            glRotatef(angle, 0.0, 0.0, 1.0);
            glRotatef(90, 0.0, 1.0, 0.0);
            gluCylinder(p, 2 + base_scale, 2 + base_scale, 10 + height_scale, 10, 5);
            // here probably we need to draw a 3d cone as the arrow tip
            glPopMatrix();
        }
}

float Visualization::pick_scalar_field_value(Simulation const &simulation, size_t idx) {
    float value = 0.0;

    switch (scalarDataset) {
        case VELOCITY_MAGN:
            value = magnitude(simulation.vx[idx], simulation.vy[idx]);
            break;
        case FORCE_MAGN:
            value = magnitude(simulation.fx[idx], simulation.fy[idx]);
            break;
        case DENSITY:
            value = simulation.rho[idx];
        default:
            break;
    }
    return value;
}

void Visualization::pick_vector_field_value(Simulation const &simulation, size_t idx, float values[]) {
    switch (vectorDataset) {
        case FORCE:
            values[0] = simulation.fx[idx];
            values[1] = simulation.fy[idx];
            break;
        case VELOCITY:
            values[0] = simulation.vx[idx];
            values[1] = simulation.vy[idx];
        default:
            break;
    }
}

GLfloat Visualization::pick_scaled_field(float v) {
    float value = 0.0;
    switch (vectorDataset) {
        case FORCE:
            value = scale(v, 0, 1, 0, vec_scale);
            break;
        case VELOCITY:
            value = scale(v, 0.00001, 0.08, 0, 10);
        default:
            break;
    }
    return value;
}

void Visualization::setScalarMin(float min) {
    datasets[scalarDataset].min = min;
}

void Visualization::setScalarMax(float max) {
    datasets[scalarDataset].max = max;
}

void Visualization::setScalarMode(Mode mode) {
    datasets[scalarDataset].mode = mode;
}

float Visualization::getScalarMin() {
    if (datasets[scalarDataset].mode == CLAMPING) {
        return datasets[scalarDataset].min;
    } else {
        return datasets[scalarDataset].scaleMin;
    }
}

float Visualization::getScalarMax() {
    if (datasets[scalarDataset].mode == CLAMPING) {
        return datasets[scalarDataset].max;
    } else {
        return datasets[scalarDataset].scaleMax;
    }
}

Visualization::Mode Visualization::getScalarMode() {
    return datasets[scalarDataset].mode;
}

void Visualization::setGlyphType(GlyphType gt) {
    glyphType = gt;
}

//void Visualization::draw_velocities(Simulation const &simulation, const int DIM, fftw_real wn, fftw_real hn) {
//    int i, j, idx;
//
//    glBegin(GL_LINES); //draw velocities
//    for (i = 0; i < DIM; i++)
//        for (j = 0; j < DIM; j++) {
//            idx = (j * DIM) + i;
//            magnitude_to_color(simulation.vx[idx], simulation.vy[idx]);
//            glVertex2f(wn + (fftw_real) i * wn, hn + (fftw_real) j * hn);
//            glVertex2f((wn + (fftw_real) i * wn) + vec_scale * simulation.vx[idx], (hn + (fftw_real) j * hn) + vec_scale * simulation.vy[idx]);
//        }
//    glEnd();
//}
//
//void Visualization::draw_forces(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn) {
//    int i, j, idx;
//
//    glBegin(GL_LINES); //draw forces
//    for (i = 0; i < DIM; i++)
//        for (j = 0; j < DIM; j++) {
//            idx = (j * DIM) + i;
//            magnitude_to_color(simulation.fx[idx], simulation.fy[idx]);
//            glVertex2f(wn + (fftw_real) i * wn, hn + (fftw_real) j * hn);
//            glVertex2f((wn + (fftw_real) i * wn) + vec_scale * simulation.fx[idx], (hn + (fftw_real) j * hn) + vec_scale * simulation.fy[idx]);
//        }
//    glEnd();
//}