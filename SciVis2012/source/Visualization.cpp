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

Visualization::Visualization() {
    vec_scale = 1000;
    sample_x = 40;
    sample_y = 40;
    densityIsoline = .5;
    scalarDataset = DENSITY;
    vectorDataset = VELOCITY;
    options[UseDirectionColoring] = false; // not used for now
    options[DRAW_SMOKE] = false;
    options[DRAW_GLYPHS] = true;
    options[DrawVectorField] = true; // not used for now
    options[GRADIENT] = false;

    datasets.insert(make_pair(DENSITY, Dataset(0, 1, CLAMPING)));
    datasets.insert(make_pair(VELOCITY_MAGN, Dataset(0.01, 0.08, CLAMPING)));
    datasets.insert(make_pair(FORCE_MAGN, Dataset(0, 0.15, CLAMPING)));
    datasets.insert(make_pair(FORCE, Dataset(0, 1, CLAMPING)));
    datasets.insert(make_pair(VELOCITY, Dataset(0, 1, CLAMPING)));
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
    for (int i = 0; i < 256; i = i + ((256 / 32)*2)) {
        for (int j = 0; j < (256 / 32); j++) {
            zebra->putColor(BLACK, i + j);
        }
    }
    for (int i = (256 / 32); i < 256; i = i + ((256 / 32)*2)) {
        for (int j = 0; j < (256 / 32); j++) {
            zebra->putColor(WHITE, i + j);
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

void Visualization::setSampleX(int x) {
    sample_x = x;
}

int Visualization::getSampleX() {
    return sample_x;
}

void Visualization::setSampleY(int y) {
    sample_y = y;
}

int Visualization::getSampleY() {
    return sample_y;
}

void Visualization::setColor(float vy, ColorType t) {

    Dataset dataset = datasets[scalarDataset];
    float colorIndex = 0, min, max;
    switch (dataset.mode) {
        case CLAMPING:
            if (vy > dataset.max) vy = dataset.max;
            if (vy < dataset.min) vy = dataset.min;
            max = dataset.max;
            min = dataset.min;
            break;
        case SCALING:
            max = dataset.scaleMax;
            min = dataset.scaleMin;
            break;
    }

    switch (t) {
        case TEXTURE:
            colorIndex = scale(vy, min, max, 0, 1);
            glTexCoord1f(colorIndex);
            break;
        case SIMPLE:
            colorIndex = scale(vy, min, max, 0, 255);
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


    datasets[scalarDataset].scaleMax = -INFINITY;
    datasets[scalarDataset].scaleMin = INFINITY;
    for (int idx = 0; idx < DIM * DIM; idx++) {
        float value = pick_scalar_field_value(simulation, idx);
        if (value > datasets[scalarDataset].scaleMax) datasets[scalarDataset].scaleMax = value;
        if (value < datasets[scalarDataset].scaleMin) datasets[scalarDataset].scaleMin = value;
    }

    if (options[DRAW_SMOKE]) {
        draw_smoke(simulation, DIM, wn, hn);
    }

    draw_isolines(simulation, DIM, wn, hn);

    if (options[DRAW_GLYPHS]) {
        draw_glyphs(simulation, DIM, wn, hn, wn_sample, hn_sample);
    }
}

int lex(int n1, int n2, int dim) {
    return n1 + n2*dim;
}

float intersect(float pi, float pj, float vi, float vj, float v) {

    return (pi * (vj - v) + pj * (v - vi)) / (vj - vi);
}

void getCell(int c, int *v, int DIM) {
    int C[2];
    int P = DIM;
    // compute cell coordinates C[0], C[1]
    C[1] = c / P;
    c -= C[1] * P;
    C[0] = c;

    // now go from cell coordinates to vertex coordinates
    int i[2];
    int j = 0;

    v[0] = lex(C[0], C[1], DIM);
    v[1] = lex(C[0] + 1, C[1] + 0, DIM);
    v[2] = lex(C[0] + 1, C[1] + 1, DIM);
    v[3] = lex(C[0] + 0, C[1] + 1, DIM);

}

void getPoint(int i, float *p, int dim, const fftw_real wn, const fftw_real hn) {
    p[0] = (i % dim) * wn;
    p[1] = (i / dim) * hn;
}

void Visualization::draw_isolines(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn) {
    int numberOfCells = (DIM - 1) * (DIM - 1);
    //only works if density is current dataset
    int *cellStates = new int[numberOfCells];

    for (int cellIndex = 0; cellIndex < numberOfCells; cellIndex++) {
        int *v = new int[4];
        getCell(cellIndex, v, DIM);

        for (int cellVertex = 0; cellVertex < 4; cellVertex++) {
            int idx = v[cellVertex];
            float v = pick_scalar_field_value(simulation, idx);
            if (v <= densityIsoline) {
                cellStates[cellIndex] |= 1 << cellVertex;
                //inside
            }
        }
    }
    //     marching squares      
    int v0, v1, v2, v3;
    float vl0, vl1, vl2, vl3;
    float *p0 = new float[2];
    float *p1 = new float[2];
    float *p2 = new float[2];
    float *p3 = new float[2];
    float be, le, te, re;

    for (int cellIndex = 0; cellIndex < numberOfCells; cellIndex++) {

        int *v = new int[4];
        getCell(cellIndex, v, DIM);
        int cellState = cellStates[cellIndex];

        v0 = v[0];
        v1 = v[1];
        v2 = v[2];
        v3 = v[3];

        vl0 = pick_scalar_field_value(simulation, v0);
        vl1 = pick_scalar_field_value(simulation, v1);
        vl2 = pick_scalar_field_value(simulation, v2);
        vl3 = pick_scalar_field_value(simulation, v3);

        getPoint(v0, p0, DIM, wn, hn);
        getPoint(v1, p1, DIM, wn, hn);
        getPoint(v2, p2, DIM, wn, hn);
        getPoint(v3, p3, DIM, wn, hn);


        switch (cellState) {
            case 0:
            case 15:
                //do nothing
                break;
            case 4:
            case 11:
                re = intersect(p1[1], p2[1], vl1, vl2, densityIsoline);
                te = intersect(p3[0], p2[0], vl3, vl2, densityIsoline);
                glBegin(GL_LINES);
                glColor3f(1, 0, 0);
                glVertex2f(te, p3[1]);
                glVertex2f(p1[0], re);
                glEnd();
                break;
            case 5:
                re = intersect(p1[1], p2[1], vl1, vl2, densityIsoline);
                te = intersect(p3[0], p2[0], vl3, vl2, densityIsoline);
                le = intersect(p0[1], p3[1], vl0, vl3, densityIsoline);
                be = intersect(p0[0], p1[0], vl0, vl1, densityIsoline);
                glBegin(GL_LINES);
                glColor3f(1, 0, 0);
                glVertex2f(te, p3[1]);
                glVertex2f(p1[0], re);
                glEnd();
                glBegin(GL_LINES);
                glColor3f(1, 0, 0);
                glVertex2f(p0[0], le);
                glVertex2f(be, p0[1]);
                glEnd();
                break;
            case 6:
            case 9:
                te = intersect(p3[0], p2[0], vl3, vl2, densityIsoline);
                be = intersect(p0[0], p1[0], vl0, vl1, densityIsoline);
                glBegin(GL_LINES);
                glColor3f(1, 0, 0);
                glVertex2f(te, p3[1]);
                glVertex2f(be, p0[1]);
                glEnd();
                break;
            case 7:
                case 8:
                te = intersect(p3[0], p2[0], vl3, vl2, densityIsoline);
                le = intersect(p0[1], p3[1], vl0, vl3, densityIsoline);
                glBegin(GL_LINES);
                glColor3f(1, 0, 0);
                glVertex2f(te, p3[1]);
                glVertex2f(p0[0], le);
                glEnd();
                break;
            case 10:
                re = intersect(p1[1], p2[1], vl1, vl2, densityIsoline);
                te = intersect(p3[0], p2[0], vl3, vl2, densityIsoline);
                le = intersect(p0[1], p3[1], vl0, vl3, densityIsoline);
                be = intersect(p0[0], p1[0], vl0, vl1, densityIsoline);
                glBegin(GL_LINES);
                glColor3f(1, 0, 0);
                glVertex2f(te, p3[1]);
                glVertex2f(p0[0], le);
                glEnd();
                glBegin(GL_LINES);
                glColor3f(1, 0, 0);
                glVertex2f(p1[0], re);
                glVertex2f(be, p0[1]);
                glEnd();
                break;
            case 12:
            case 3:
                //                cout << vl0 << " " << vl1 << " " << vl2 << " " << vl3 << " " << densityIsoline << "\n";
                //            cout << p0[0] << " " << p1[0] << " " << p2[0] << " " << p3[0] << "\n";
                //          cout << p0[1] << " " << p1[1] << " " << p2[1] << " " << p3[1] << "\n";
                
                le = intersect(p0[1], p3[1], vl0, vl3, densityIsoline);
                re = intersect(p1[1], p2[1], vl1, vl2, densityIsoline);
                
                
                glBegin(GL_LINES);
                glColor3f(1, 0, 0);
                glVertex2f(p0[0], le);
                glVertex2f(p1[0], re);
                glEnd();
                break;
            case 13:
            case 2:
                
                cout << "case 13 ";
                be = intersect(p0[0], p1[0], vl0, vl1, densityIsoline);
                re = intersect(p1[1], p2[1], vl1, vl2, densityIsoline);
                cout << "(" << p1[0] << "," << re << ")"  << " " << "(" << be << "," << p0[1] << ")" << "\n";
                glBegin(GL_LINES);
                glColor3f(1, 0, 0);
                glVertex2f(p1[0], re);
                glVertex2f(be, p0[1]);
                glEnd();
                break;
            case 14:
            case 1:
                cout << "case 14 ";
                be = intersect(p0[0], p1[0], vl0, vl1, densityIsoline);
                le = intersect(p0[1], p3[1], vl0, vl3, densityIsoline);
                glBegin(GL_LINES);
                glColor3f(1, 0, 0);
                glVertex2f(p0[0], le);
                glVertex2f(be, p0[1]);
                glEnd();
                break;
        }
    }


}

void Visualization::draw_smoke(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn) {
    int i, j, idx;
    double px, py;

    GLenum error = glGetError();
    if (error != 0) {
        std::cout << error << '\n';
    }
    glEnable(GL_TEXTURE_1D);
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
    glDisable(GL_TEXTURE_1D);
}

void Visualization::draw_glyphs(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn, const fftw_real wn_sample, const fftw_real hn_sample) {
    if (sample_x == DIM && sample_y == DIM)
        draw_glyphs_on_comp_grid(simulation, DIM, wn, hn, wn_sample, hn_sample);
    else
        draw_glyphs_on_sampled_grid(simulation, DIM, wn, hn, wn_sample, hn_sample);
}

void Visualization::draw_glyphs_on_comp_grid(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn, const fftw_real wn_sample, const fftw_real hn_sample) {
    size_t idx;
    GLfloat magn;
    float *xy = new float[2];

    for (int i = 0; i < DIM; i++) {
        for (int j = 0; j < DIM; j++) {
            idx = (j * DIM) + i;
            pick_vector_field_value(simulation, idx, xy, i, j, wn, hn, DIM);

            magn = magnitude(xy);
            magn = pick_scaled_field(magn);

            GLfloat x = xy[0];
            GLfloat y = xy[1];
            GLfloat angle = 0.0;

            angle = atan2(y, x) * 180 / M_PI;

            GLfloat x_start = wn + (fftw_real) i * wn;
            GLfloat y_start = hn + (fftw_real) j * hn;

            float value_for_color = pick_scalar_field_value(simulation, idx);

            switch (glyphType) {
                case HEDGEHOGS:
                    draw_hedgehogs(x_start, y_start, value_for_color, xy);
                    break;
                case SIMPLE_ARROWS:
                    draw_simple_arrow(magn, x_start, y_start, angle, value_for_color);
                    break;
                case CONES_3D:
                    draw_3d_cone(magn, x_start, y_start, angle, value_for_color);
                    break;
                case ARROWS_3D:
                    draw_3d_arrow(magn, x_start, y_start, angle, value_for_color);
                    break;
            }
        }
    }
}

void Visualization::draw_glyphs_on_sampled_grid(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn, const fftw_real wn_sample, const fftw_real hn_sample) {
    size_t idx_x1_y1, idx_x2_y2, idx_x3_y3, idx_x4_y4;
    GLfloat magn;
    float *sample_x1_y1 = new float[2];
    float *sample_x2_y2 = new float[2];
    float *sample_x3_y3 = new float[2];
    float *sample_x4_y4 = new float[2];

    for (int i = 0; i < sample_x; i++) {
        for (int j = 0; j < sample_y; j++) {
            GLfloat x_start = wn_sample + (fftw_real) i * wn_sample;
            GLfloat y_start = hn_sample + (fftw_real) j * hn_sample;
            // interpolate the sample values with the values of the computational grid points
            // divide the x,y of the sample point with the step of the computational grid (wn or hn),
            // ceil and then we get the coordinates of the 4 points of the computational grid to use
            // for bilinear interpolation
            int x_point = ceil(x_start / wn);
            int y_point = ceil(y_start / hn);
            idx_x1_y1 = ((y_point - 1) * DIM) + (x_point - 1);
            idx_x2_y2 = ((y_point - 2) * DIM) + (x_point - 1);
            idx_x3_y3 = ((y_point - 1) * DIM) + (x_point - 2);
            idx_x4_y4 = ((y_point - 2) * DIM) + (x_point - 2);
            pick_vector_field_value(simulation, idx_x1_y1, sample_x1_y1);
            pick_vector_field_value(simulation, idx_x2_y2, sample_x2_y2);
            pick_vector_field_value(simulation, idx_x3_y3, sample_x3_y3);
            pick_vector_field_value(simulation, idx_x4_y4, sample_x4_y4);
            // bilinear interpolation
            GLfloat x1, y1, x2, y2;
            x1 = wn + (fftw_real) (x_point - 1) * wn;
            y1 = hn + (fftw_real) (y_point - 1) * hn;
            x2 = wn + (fftw_real) (x_point - 2) * wn;
            y2 = hn + (fftw_real) (y_point - 2) * hn;
            GLfloat *xy = new GLfloat[2];
            GLfloat f1x = sample_x4_y4[0]*(x2 - x_start)*(y2 - y_start);
            GLfloat f2x = sample_x2_y2[0]*(x_start - x1)*(y2 - y_start);
            GLfloat f3x = sample_x3_y3[0]*(x2 - x_start)*(y_start - y1);
            GLfloat f4x = sample_x1_y1[0]*(x_start - x1)*(y_start - y1);
            xy[0] = (1 / ((x2 - x1)*(y2 - y1)))*(f1x + f2x + f3x + f4x);
            GLfloat f1y = sample_x4_y4[1]*(x2 - x_start)*(y2 - y_start);
            GLfloat f2y = sample_x2_y2[1]*(x_start - x1)*(y2 - y_start);
            GLfloat f3y = sample_x3_y3[1]*(x2 - x_start)*(y_start - y1);
            GLfloat f4y = sample_x1_y1[1]*(x_start - x1)*(y_start - y1);
            xy[1] = (1 / ((x2 - x1)*(y2 - y1)))*(f1y + f2y + f3y + f4y);

            magn = magnitude(xy[0], xy[1]);
            magn = pick_scaled_field(magn);

            GLfloat angle = atan2(xy[1], xy[0]) * 180 / M_PI;

            float value_for_color1 = pick_scalar_field_value(simulation, idx_x1_y1);
            float value_for_color2 = pick_scalar_field_value(simulation, idx_x2_y2);
            float value_for_color3 = pick_scalar_field_value(simulation, idx_x3_y3);
            float value_for_color4 = pick_scalar_field_value(simulation, idx_x4_y4);

            float fx4y4 = value_for_color4 * (x2 - x_start) * (y2 - y_start);
            float fx2y2 = value_for_color2 * (x_start - x1) * (y2 - y_start);
            float fx3y3 = value_for_color3 * (x2 - x_start) * (y_start - y1);
            float fx1y1 = value_for_color1 * (x_start - x1) * (y_start - y1);
            float value_for_color = (1 / ((x2 - x1) * (y2 - y1))) * (fx1y1 + fx2y2 + fx3y3 + fx4y4);


            switch (glyphType) {
                case HEDGEHOGS:
                    draw_hedgehogs(x_start, y_start, value_for_color, xy);
                    break;
                case SIMPLE_ARROWS:
                    draw_simple_arrow(magn, x_start, y_start, angle, value_for_color);
                    break;
                case CONES_3D:
                    draw_3d_cone(magn, x_start, y_start, angle, value_for_color);
                    break;
                case ARROWS_3D:
                    draw_3d_arrow(magn, x_start, y_start, angle, value_for_color);
                    break;
            }
        }
    }
}

void Visualization::draw_hedgehogs(GLfloat x_start, GLfloat y_start, float value, float values[]) {
    glEnable(GL_TEXTURE_1D);
    colormap->loadColormapTexture();
    glBegin(GL_LINES);
    setColor(value, TEXTURE);
    glVertex2f(x_start, y_start);
    glVertex2f(x_start + vec_scale * values[0], y_start + vec_scale * values[1]);
    glEnd();
    glDisable(GL_TEXTURE_1D);
}

void Visualization::draw_simple_arrow(GLfloat magn, GLfloat x_start, GLfloat y_start, GLfloat angle, float value) {
    glEnable(GL_TEXTURE_1D);
    colormap->loadColormapTexture();
    GLfloat scale_x = magn * 1.5;
    GLfloat scale_y_quad = magn * 0.2;
    GLfloat scale_y_triangle = magn * 0.3;
    glPushMatrix();
    glTranslatef(x_start, y_start, 0.0);
    glRotatef(angle, 0.0, 0.0, 1.0f);
    glTranslatef(-x_start, -y_start, 0.0);
    setColor(value, TEXTURE);
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
    glDisable(GL_TEXTURE_1D);
}

void Visualization::draw_3d_cone(GLfloat magn, GLfloat x_start, GLfloat y_start, GLfloat angle, float value) {
    GLfloat base_scale = magn;
    GLfloat height_scale = magn * 2;
    glPushMatrix();
    setColor(value, SIMPLE);
    glTranslatef(x_start, y_start, 0.0);
    glRotatef(angle, 0.0, 0.0, 1.0);
    glRotatef(90, 0.0, 1.0, 0.0);
    glutSolidCone(0.5 + base_scale, 5 + height_scale, 50, 10);
    glPopMatrix();
}

void Visualization::draw_3d_arrow(GLfloat magn, GLfloat x_start, GLfloat y_start, GLfloat angle, float value) {
    static GLUquadricObj *p = gluNewQuadric();
    gluQuadricDrawStyle(p, GLU_FILL);
    GLfloat base_scale = magn * 0.3;
    GLfloat height_scale = magn * 2;
    glPushMatrix();
    setColor(value, SIMPLE);
    glTranslatef(x_start, y_start, 0.0);
    glRotatef(angle, 0.0, 0.0, 1.0);
    glRotatef(90, 0.0, 1.0, 0.0);
    gluCylinder(p, 0.25 + base_scale, 0.1 + base_scale, 3 + height_scale, 10, 5);
    glTranslatef(0, 0, 3 + height_scale);
    gluCylinder(p, 2 + base_scale, 0, 2 + height_scale, 10, 5);
    glPopMatrix();
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

void Visualization::pick_vector_field_value(Simulation const &simulation, size_t idx, float values[], int i, int j, float wn, float hn, int DIM) {
    switch (vectorDataset) {
        case DENSITY_GRADIENT:
            gradient(simulation, i, j, wn, hn, DIM, values);
            break;
        case VELOCITY_MAGN_GRADIENT:
            gradient(simulation, i, j, wn, hn, DIM, values);
            break;
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
            value = scale(v, 0, 0.15, 0, 10);
            break;
        case DENSITY_GRADIENT:
            value = scale(v, 0.001, 5, 0, 10);
            break;
        case VELOCITY_MAGN_GRADIENT:
            value = scale(v, 0.00001, 0.08, 0, 10);
            break;
        case VELOCITY:
            value = scale(v, 0.00001, 0.08, 0, 10);
        default:
            break;
    }
    return value;
}

void Visualization::gradient(Simulation const &simulation, int i, int j, float wn, float hn, int DIM, float grad[]) {
    float x = wn + (fftw_real) i * wn;
    float y = hn + (fftw_real) j * hn;

    int x_point = ceil(x / wn);
    int y_point = ceil(y / hn);
    //    int topright = ((y_point - 1) * DIM) + (x_point - 1);
    int bottomright = ((y_point - 2) * DIM) + (x_point - 1);
    int topleft = ((y_point - 1) * DIM) + (x_point - 2);
    int bottomleft = ((y_point - 2) * DIM) + (x_point - 2);

    grad[0] = pick_scalar_field_value(simulation, bottomright) - pick_scalar_field_value(simulation, bottomleft);
    grad[1] = pick_scalar_field_value(simulation, topleft) - pick_scalar_field_value(simulation, bottomleft);
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
    return datasets[scalarDataset].min;
}

float Visualization::getScalarScaleMin() {
    return datasets[scalarDataset].scaleMin;
}

float Visualization::getScalarMax() {
    return datasets[scalarDataset].max;
}

float Visualization::getScalarScaleMax() {
    return datasets[scalarDataset].scaleMax;
}

Visualization::Mode Visualization::getScalarMode() {
    return datasets[scalarDataset].mode;
}

void Visualization::setGlyphType(GlyphType gt) {
    glyphType = gt;
}

void Visualization::setDensityIsoline(float di) {
    densityIsoline = di;
}

float Visualization::getDensityIsoline() {
    return densityIsoline;
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