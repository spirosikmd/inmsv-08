#include <algorithm>
#include <cmath>
#include <GLUT/glut.h>
#include <vector>
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
    heightplotDataset = DENSITY;
    numSegments = 5;
    options[UseDirectionColoring] = true; // not used for now
    options[DRAW_SMOKE] = false;
    options[DRAW_GLYPHS] = false;
    options[DRAW_ISOLINES] = false;
    options[SMOOTH_SHADING] = true;
    options[DRAW_HEIGHTPLOT] = false;
    options[DrawVectorField] = false; // not used for now


    datasets.insert(make_pair(DENSITY, Dataset(0, 1, CLAMPING)));
    datasets.insert(make_pair(VELOCITY_MAGN, Dataset(0.01, 0.08, CLAMPING)));
    datasets.insert(make_pair(FORCE_MAGN, Dataset(0, 0.15, CLAMPING)));
    datasets.insert(make_pair(VELOCITY_DIV, Dataset(0, 0.03, CLAMPING)));
    datasets.insert(make_pair(FORCE_DIV, Dataset(0, 0.15, CLAMPING)));
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

void Visualization::setHeightplotDataset(DatasetType hdm) {
    heightplotDataset = hdm;
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

float Visualization::getHeight(float vy, float maxheight) {
    Dataset dataset = datasets[heightplotDataset];
    float min, max;
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
    return scale(vy, min, max, 0, maxheight);
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

float Visualization::scaleScalar(float vy, float smin, float smax) {

    Dataset dataset = datasets[scalarDataset];
    float min, max;
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

    return scale(vy, min, max, smin, smax);

}

Colormap* Visualization::getColormap() {
    return colormap;
}

Colormap* Visualization::loadColormap(ColorMode cm) {
    colormap = colormaps[cm];
    return colormap;
}


//visualize: This is the main visualization function

void Visualization::visualize3D(Simulation const &simulation, int winWidth, int winHeight) {
    const int DIM = Simulation::DIM;
    fftw_real wn = (fftw_real) winWidth / (fftw_real) (DIM + 1); // Computational Grid cell width
    fftw_real hn = (fftw_real) winHeight / (fftw_real) (DIM + 1); // Computational Grid cell heigh
    fftw_real zn = winHeight / Application::timeslices.getCapacity();

    datasets[scalarDataset].scaleMax = -INFINITY;
    datasets[scalarDataset].scaleMin = INFINITY;
    for (int idx = 0; idx < DIM * DIM; idx++) {
        float value = pick_scalar_field_value(simulation, idx);
        if (value > datasets[scalarDataset].scaleMax) datasets[scalarDataset].scaleMax = value;
        if (value < datasets[scalarDataset].scaleMin) datasets[scalarDataset].scaleMin = value;
    }

    datasets[heightplotDataset].scaleMax = -INFINITY;
    datasets[heightplotDataset].scaleMin = INFINITY;
    for (int idx = 0; idx < DIM * DIM; idx++) {
        float value = pick_scalar_field_value(heightplotDataset, simulation, idx);
        if (value > datasets[heightplotDataset].scaleMax) datasets[heightplotDataset].scaleMax = value;
        if (value < datasets[heightplotDataset].scaleMin) datasets[heightplotDataset].scaleMin = value;
    }

    if (options[DRAW_HEIGHTPLOT] || options[DRAW_STREAMTUBES]) {
        float x_max = DIM * wn + wn;
        float y_max = DIM * hn + hn;
        float z_max = Application::timeslices.getCapacity() * zn;

        glBegin(GL_LINE_LOOP);
        glColor3f(1, 1, 1);

        glVertex3f(wn, hn, 0);
        glVertex3f(x_max - wn, hn, 0);
        glVertex3f(x_max - wn, y_max - hn, 0);
        glVertex3f(wn, y_max - hn, 0);
        glEnd();
        glBegin(GL_LINE_LOOP);
        glColor3f(1, 1, 1);
        glVertex3f(wn, hn, z_max);
        glVertex3f(x_max - wn, hn, z_max);
        glVertex3f(x_max - wn, y_max - hn, z_max);
        glVertex3f(wn, y_max - hn, z_max);
        glEnd();
        glBegin(GL_LINES);
        glColor3f(1, 1, 1);
        glVertex3f(wn, hn, 0);
        glVertex3f(wn, hn, z_max);
        glVertex3f(wn, y_max - hn, 0);
        glVertex3f(wn, y_max - hn, z_max);
        glVertex3f(x_max - wn, hn, 0);
        glVertex3f(x_max - wn, hn, z_max);
        glVertex3f(x_max - wn, y_max - hn, 0);
        glVertex3f(x_max - wn, y_max - hn, z_max);
        glEnd();
    }

    if (options[DRAW_HEIGHTPLOT]) {
        draw_heightplot(simulation, DIM, wn, hn);
    }
    if (options[DRAW_STREAMTUBES]) {
        draw_timedependent_vector_field(simulation, DIM, wn, hn, zn);
    }


}

void Visualization::visualize(Simulation const &simulation, int winWidth, int winHeight) {
    const int DIM = Simulation::DIM;
    fftw_real wn = (fftw_real) winWidth / (fftw_real) (DIM + 1); // Computational Grid cell width
    fftw_real hn = (fftw_real) winHeight / (fftw_real) (DIM + 1); // Computational Grid cell heigh


    fftw_real wn_sample = (fftw_real) winWidth / (fftw_real) (sample_x + 1); // Sample Grid cell width 
    fftw_real hn_sample = (fftw_real) winHeight / (fftw_real) (sample_y + 1); // Sample Grid cell heigh



    if (options[DRAW_SMOKE]) {
        draw_smoke(simulation, DIM, wn, hn);
    }

    if (options[DRAW_ISOLINES]) {
        draw_isoline(simulation, DIM, wn, hn, densityIsoline);

        float range = fabs(densityRHO2Isoline - densityRHO1Isoline);
        float isolineStep = range / numIsolines;
        
        for (int i = 0; i < numIsolines; i++) {
            draw_isoline(simulation, DIM, wn, hn, densityRHO1Isoline + (isolineStep * (i + 1)));
        }
    }


    if (options[DRAW_GLYPHS]) {
        draw_glyphs(simulation, DIM, wn, hn, wn_sample, hn_sample);
        //        draw_streamlines(simulation, DIM, wn, hn);
    }


}

int lex(int n1, int n2, int dim) {
    return n1 + n2*dim;
}

float getIntersection(float pi, float pj, float vi, float vj, float v) {
    return (pi * (vj - v) + pj * (v - vi)) / (vj - vi);
}

void getCell(int c, int *v, int DIM) {
    int C[2];
    int P = DIM - 1;

    C[1] = c / P;
    c -= C[1] * P;
    C[0] = c;

    v[0] = lex(C[0], C[1], DIM);
    v[1] = lex(C[0] + 1, C[1] + 0, DIM);
    v[2] = lex(C[0] + 1, C[1] + 1, DIM);
    v[3] = lex(C[0] + 0, C[1] + 1, DIM);
}

void getPoint(int i, float *p, int dim, const fftw_real wn, const fftw_real hn) {
    p[0] = wn + (i % dim) * wn;
    p[1] = hn + (i / dim) * hn;
}

void Visualization::draw_isoline(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn, float isovalue) {
    int numberOfCells = (DIM - 1) * (DIM - 1);
    //only works if density is current dataset
    int *cellStates = new int[numberOfCells];

    for (int cellIndex = 0; cellIndex < numberOfCells; cellIndex++) {
        int *v = new int[4];
        getCell(cellIndex, v, DIM);

        for (int cellVertex = 0; cellVertex < 4; cellVertex++) {
            int idx = v[cellVertex];
            float v = pick_scalar_field_value(simulation, idx);
            if (v <= isovalue) {
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

        glPushMatrix();

        colormap->loadColormapTexture();
        if (options[COLORIZE]) {
            glEnable(GL_TEXTURE_1D);
            setColor(isovalue, TEXTURE);
        } else {
            glColor3f(0, 0, 0);
        }

        switch (cellState) {
            case 0:
            case 15:
                //do nothing
                break;
            case 4:
            case 11:
                re = getIntersection(p1[1], p2[1], vl1, vl2, isovalue);
                te = getIntersection(p3[0], p2[0], vl3, vl2, isovalue);
                glBegin(GL_LINES);
                glVertex2f(te, p3[1]);
                glVertex2f(p1[0], re);
                glEnd();
                break;
            case 5:
                re = getIntersection(p1[1], p2[1], vl1, vl2, isovalue);
                te = getIntersection(p3[0], p2[0], vl3, vl2, isovalue);
                le = getIntersection(p0[1], p3[1], vl0, vl3, isovalue);
                be = getIntersection(p0[0], p1[0], vl0, vl1, isovalue);
                glBegin(GL_LINES);
                glVertex2f(te, p3[1]);
                glVertex2f(p1[0], re);
                glEnd();
                glBegin(GL_LINES);
                glVertex2f(p0[0], le);
                glVertex2f(be, p0[1]);
                glEnd();
                break;
            case 6:
            case 9:
                te = getIntersection(p3[0], p2[0], vl3, vl2, isovalue);
                be = getIntersection(p0[0], p1[0], vl0, vl1, isovalue);
                glBegin(GL_LINES);
                glVertex2f(te, p3[1]);
                glVertex2f(be, p0[1]);
                glEnd();
                break;
            case 7:
            case 8:
                te = getIntersection(p3[0], p2[0], vl3, vl2, isovalue);
                le = getIntersection(p0[1], p3[1], vl0, vl3, isovalue);
                glBegin(GL_LINES);
                glVertex2f(te, p3[1]);
                glVertex2f(p0[0], le);
                glEnd();
                break;
            case 10:
                re = getIntersection(p1[1], p2[1], vl1, vl2, isovalue);
                te = getIntersection(p3[0], p2[0], vl3, vl2, isovalue);
                le = getIntersection(p0[1], p3[1], vl0, vl3, isovalue);
                be = getIntersection(p0[0], p1[0], vl0, vl1, isovalue);
                glBegin(GL_LINES);
                glVertex2f(te, p3[1]);
                glVertex2f(p0[0], le);
                glEnd();
                glBegin(GL_LINES);
                glVertex2f(p1[0], re);
                glVertex2f(be, p0[1]);
                glEnd();
                break;
            case 12:
            case 3:

                le = getIntersection(p0[1], p3[1], vl0, vl3, isovalue);
                re = getIntersection(p1[1], p2[1], vl1, vl2, isovalue);
                glBegin(GL_LINES);
                glVertex2f(p1[0], re);
                glVertex2f(p0[0], le);
                glEnd();
                break;
            case 13:
            case 2:
                be = getIntersection(p0[0], p1[0], vl0, vl1, isovalue);
                re = getIntersection(p1[1], p2[1], vl1, vl2, isovalue);
                glBegin(GL_LINES);
                glVertex2f(p1[0], re);
                glVertex2f(be, p0[1]);
                glEnd();
                break;
            case 14:
            case 1:
                be = getIntersection(p0[0], p1[0], vl0, vl1, isovalue);
                le = getIntersection(p0[1], p3[1], vl0, vl3, isovalue);
                glBegin(GL_LINES);
                glVertex2f(p0[0], le);
                glVertex2f(be, p0[1]);
                glEnd();

                break;
        }
        glDisable(GL_TEXTURE_1D);
        glPopMatrix();
    }
}

void Visualization::draw_timedependent_vector_field(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn, const fftw_real zn) {

    double px, py, pz;

    int capacity = Application::timeslices.getCapacity();

    float *v = new float[2];

    for (int time = 0; time < capacity; time++) {

        for (int idx = 0; idx < DIM * DIM; idx++) {
            float x = idx % DIM;
            float y = idx / DIM;

            float vx = pick_timescalar_field_value(idx, time);
            pick_timevector_field_value(idx, v, time);
            px = wn + (fftw_real) x * wn;
            py = hn + (fftw_real) y * hn;
            pz = time *zn;
            if (options[DRAW_3DFIELD]) {
                glPushMatrix();
                glTranslatef(px, py, pz);
                glBegin(GL_LINES);
                setColor(vx, SIMPLE);
                glVertex3f(0, 0, 0);
                glVertex3f(v[0]*1000, v[1]*1000, 20);
                glEnd();
                glPopMatrix();
            }
        }
    }

    float dt = 10;
    int maxLength = 1000;

    for (size_t i = 0; i < seedpoints.size(); i++) {
        draw_streamtube(calculateStreamtubePoints(seedpoints[i].x, seedpoints[i].y, seedpoints[i].z, dt, maxLength, DIM, wn, hn, zn));
    }
}

void Visualization::draw_streamtube(vector<vector<float > > points) {

    if (points.size() < 2) {
        return;
    }

    for (size_t sp = 0; sp < points.size() - 1; sp++) {
        float target[3] = {points[sp + 1][0] - points[sp][0], points[sp + 1][1] - points[sp][1], points[sp + 1][2] - points[sp][2]};
        normalize3(target);
        float previous[3] = {0, 0, 1};
        if (sp != 0) {
            previous[0] = points[sp][0] - points[sp - 1][0];
            previous[1] = points[sp][1] - points[sp - 1][1];
            previous[2] = points[sp][2] - points[sp - 1][2];
        }
        normalize3(previous);

        float up[3] = {0, 1, 0};
        normalize3(up);

        float local[3];
        crossproduct(target, up, local);
        normalize3(local);


        glEnable(GL_TEXTURE_1D);
        glPushMatrix();
        GLfloat R[16] = {
            local[0]     , local[1]     , local[2]     , 0,
            up[0]        , up[1]        , up[2]        , 0,
            target[0]    , target[1]    , target[2]    , 0,
            points[sp][0], points[sp][1], points[sp][2], 1};
        glMultMatrixf(R);


        int r = scaleScalar(points[sp][3], 10, 20);
        int rn = scaleScalar(points[sp + 1][3], 10, 20);
        
        if (!options[DRAW_THICKTUBES]) {
            rn = r = 15;
        }

        int n = numSegments;

        colormap->loadColormapTexture();

        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= 360; i = i + (360 / n)) {
            float degInRad = i * (M_PI / 180);
            float degInRad2 = (i + 1)*(M_PI / 180);
            float normal[3] = {};
            float vz[3] = {(cos(degInRad) * r) - (cos(degInRad) * rn), (sin(degInRad) * r)-(sin(degInRad) * rn), 10};
            normalize3(vz);
            float vx[3] = {(cos(degInRad) * r) - (cos(degInRad2) * r), (sin(degInRad) * r)-(sin(degInRad2) * r), 0};
            normalize3(vx);
            crossproduct(vz, vx, normal);

            setColor(points[sp][3], TEXTURE);
            glNormal3f(normal[0], normal[1], normal[2]);
            glVertex3f(cos(degInRad) * r, sin(degInRad) * r, 0);

            setColor(points[sp + 1][3], TEXTURE);
            glNormal3f(normal[0], normal[1], normal[2]);
            glVertex3f(cos(degInRad) * rn, sin(degInRad) * rn, 15);

            setColor(points[sp][3], TEXTURE);
            glNormal3f(normal[0], normal[1], normal[2]);
            glVertex3f(cos(degInRad2) * r, sin(degInRad2) * r, 0);

            setColor(points[sp + 1][3], TEXTURE);
            glNormal3f(normal[0], normal[1], normal[2]);
            glVertex3f(cos(degInRad2) * rn, sin(degInRad2) * rn, 15);
        }
        glEnd();
        glPopMatrix();
        glDisable(GL_TEXTURE_1D);
    }
}

vector<vector<float > > Visualization::calculateStreamtubePoints(float x, float y, float z, float dt, int maxLength, int DIM, float wn, float hn, float zn) {

    int capacity = Application::timeslices.getCapacity();
    vector<vector<float > > points;



    for (int step = 0; step < maxLength; step++) {
        int x_point = floor(x / wn) - 1;
        int y_point = floor(y / hn) - 1;
        int z_point = floor(z / zn);

        if (x_point < 0 || x_point >= DIM - 1 || y_point < 0 || y_point >= DIM - 1 || z_point < 0 || z_point >= capacity - 1) {
            return points;
        }

        float *interpolated = new float[2];
        int *v = new int[8];
        float *v000 = new float[3];
        float *v010 = new float[3];
        float *v110 = new float[3];
        float *v100 = new float[3];
        float *v001 = new float[3];
        float *v011 = new float[3];
        float *v111 = new float[3];
        float *v101 = new float[3];

        int c = (y_point * (DIM - 1)) + x_point;
        getCell(c, v, DIM);

        pick_timevector_field_value(v[0], v000, z_point);
        pick_timevector_field_value(v[3], v010, z_point);
        pick_timevector_field_value(v[2], v110, z_point);
        pick_timevector_field_value(v[1], v100, z_point);
        pick_timevector_field_value(v[0], v001, z_point + 1);
        pick_timevector_field_value(v[3], v011, z_point + 1);
        pick_timevector_field_value(v[2], v111, z_point + 1);
        pick_timevector_field_value(v[1], v101, z_point + 1);

        float sv000 = pick_timescalar_field_value(v[0], z_point);
        float sv010 = pick_timescalar_field_value(v[3], z_point);
        float sv110 = pick_timescalar_field_value(v[2], z_point);
        float sv100 = pick_timescalar_field_value(v[1], z_point);
        float sv001 = pick_timescalar_field_value(v[0], z_point + 1);
        float sv011 = pick_timescalar_field_value(v[3], z_point + 1);
        float sv111 = pick_timescalar_field_value(v[2], z_point + 1);
        float sv101 = pick_timescalar_field_value(v[1], z_point + 1);

        // trilinear interpolation
        float x0, y0, z0, x1, y1, z1;
        x0 = wn + (fftw_real) (x_point) * wn;
        x1 = wn + (fftw_real) (x_point + 1) * wn;
        y0 = hn + (fftw_real) (y_point) * hn;
        y1 = hn + (fftw_real) (y_point + 1) * hn;
        z0 = zn + (fftw_real) (z_point) * zn;
        z1 = zn + (fftw_real) (z_point + 1) * zn;

        float xd, yd, zd;
        xd = (x - x0) / (x1 - x0);
        yd = (y - y0) / (y1 - y0);
        zd = (z - z0) / (z1 - z0);

        float c00, c01, c10, c11, c0, c1;
        c00 = v000[0]*(1 - xd) + v100[0] * xd;
        c10 = v010[0]*(1 - xd) + v110[0] * xd;
        c01 = v001[0]*(1 - xd) + v101[0] * xd;
        c11 = v011[0]*(1 - xd) + v111[0] * xd;
        c0 = c00 * (1 - yd) + c10*yd;
        c1 = c01 * (1 - yd) + c11*yd;
        interpolated[0] = c0 * (1 - zd) + c1*zd;

        c00 = v000[1]*(1 - xd) + v100[1] * xd;
        c10 = v010[1]*(1 - xd) + v110[1] * xd;
        c01 = v001[1]*(1 - xd) + v101[1] * xd;
        c11 = v011[1]*(1 - xd) + v111[1] * xd;
        c0 = c00 * (1 - yd) + c10*yd;
        c1 = c01 * (1 - yd) + c11*yd;
        interpolated[1] = c0 * (1 - zd) + c1*zd;
        interpolated[2] = 1;

        c00 = sv000 * (1 - xd) + sv100 * xd;
        c10 = sv010 * (1 - xd) + sv110 * xd;
        c01 = sv001 * (1 - xd) + sv101 * xd;
        c11 = sv011 * (1 - xd) + sv111 * xd;
        c0 = c00 * (1 - yd) + c10*yd;
        c1 = c01 * (1 - yd) + c11*yd;
        float scalarValue = c0 * (1 - zd) + c1*zd;


        normalize2(interpolated);
        normalize3(interpolated);

        vector<float> stp;
        stp.resize(4);
        stp[0] = x;
        stp[1] = y;
        stp[2] = z;
        stp[3] = scalarValue;
        points.push_back(stp);

        x = x + interpolated[0] * dt;
        y = y + interpolated[1] * dt;
        z = z + interpolated[2] * dt;


        delete[] v;
        delete[] interpolated;
        delete[] v000;
        delete[] v010;
        delete[] v110;
        delete[] v100;
        delete[] v001;
        delete[] v011;
        delete[] v111;
        delete[] v101;
    }
    return points;
}

void Visualization::draw_heightplot(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn) {

    int i, j, idx;
    double px, py, pz;

    float maxHeight = 100;
    //calculate normals
    int v0, v1, v2, v3;
    int *v = new int[4];
    float z0, z1, z2, z3;
    float *p0 = new float[2];
    float *p1 = new float[2];
    float *p2 = new float[2];
    float *p3 = new float[2];
    float *N1 = new float[3];
    float *N2 = new float[3];
    int numberOfCells = (DIM - 1) * (DIM - 1);

    vector<vector<vector<float> > > surfaceNormals;
    surfaceNormals.resize(numberOfCells);
    for (int i = 0; i < numberOfCells; ++i) {
        surfaceNormals[i].resize(2);
        for (int j = 0; j < 2; ++j)
            surfaceNormals[i][j].resize(3);
    }
    for (int cellIndex = 0; cellIndex < numberOfCells; cellIndex++) {
        getCell(cellIndex, v, DIM);
        v0 = v[0];
        v1 = v[1];
        v2 = v[2];
        v3 = v[3];
        z0 = getHeight(pick_scalar_field_value(heightplotDataset, simulation, v0), maxHeight);
        z1 = getHeight(pick_scalar_field_value(heightplotDataset, simulation, v1), maxHeight);
        z2 = getHeight(pick_scalar_field_value(heightplotDataset, simulation, v2), maxHeight);
        z3 = getHeight(pick_scalar_field_value(heightplotDataset, simulation, v3), maxHeight);

        getPoint(v0, p0, DIM, wn, hn);
        getPoint(v1, p1, DIM, wn, hn);
        getPoint(v2, p2, DIM, wn, hn);
        getPoint(v3, p3, DIM, wn, hn);

        float U1[] = {p3[0] - p0[0], p3[1] - p0[1], z3 - z0};
        float V1[] = {p0[0] - p1[0], p0[1] - p1[1], z0 - z1};


        float U2[] = {p2[0] - p1[0], p2[1] - p1[1], z2 - z1};
        float V2[] = {p1[0] - p3[0], p1[1] - p3[1], z1 - z3};
        normalize3(U1);
        normalize3(V1);
        normalize3(U2);
        normalize3(V2);
        crossproduct(U1, V1, N1);
        crossproduct(U2, V2, N2);
        normalize3(N1);
        normalize3(N2);
        surfaceNormals[cellIndex][0][0] = N1[0]* 1;
        surfaceNormals[cellIndex][0][1] = N1[1]* 1;
        surfaceNormals[cellIndex][0][2] = N1[2] *1;
        surfaceNormals[cellIndex][1][0] = N2[0]*1;
        surfaceNormals[cellIndex][1][1] = N2[1]* 1;
        surfaceNormals[cellIndex][1][2] = N2[2] *1;
    }

    int numberOfVertices = DIM * DIM;
    vector<vector<float> > vertexNormals;
    vertexNormals.resize(numberOfVertices);
    for (int i = 0; i < numberOfVertices; ++i) {
        vertexNormals[i].resize(3);
    }
    for (int vertexIndex = 0; vertexIndex < numberOfVertices; vertexIndex++) {
        int j = vertexIndex / DIM;
        int i = vertexIndex % DIM;
        int lbc = ((j - 1)*(DIM - 1)) + (i - 1);
        int rbc = ((j - 1)*(DIM - 1)) + i;
        int ltc = (j * (DIM - 1)) + (i - 1);
        int rtc = (j * (DIM - 1)) + i;

        if (i == 0) {
            ltc = -1;
            lbc = -1;
        } else if (i == DIM - 1) {
            rtc = -1;
            rbc = -1;
        }
        if (j == 0) {
            lbc = -1;
            rbc = -1;
        } else if (j == DIM - 1) {
            ltc = -1;
            rtc = -1;
        }
        float N = 0;
        float x = 0, y = 0, z = 0;

        if (ltc > -1) {
            x = x + surfaceNormals[ltc][0][0];
            x = x + surfaceNormals[ltc][1][0];
            y = y + surfaceNormals[ltc][0][1];
            y = y + surfaceNormals[ltc][1][1];

            z = z + surfaceNormals[ltc][0][2];
            z = z + surfaceNormals[ltc][1][2];
            N = N + 2;
        }
        if (rtc > -1) {
            x = x + surfaceNormals[rtc][0][0];
            y = y + surfaceNormals[rtc][0][1];
            z = z + surfaceNormals[rtc][0][2];
            N = N + 1;
        }
        if (rbc > -1) {
            x = x + surfaceNormals[rbc][0][0];
            x = x + surfaceNormals[rbc][1][0];
            y = y + surfaceNormals[rbc][0][1];
            y = y + surfaceNormals[rbc][1][1];
            z = z + surfaceNormals[rbc][0][2];
            z = z + surfaceNormals[rbc][1][2];
            N = N + 2;
        }
        if (lbc > -1) {
            x = x + surfaceNormals[lbc][0][0];
            y = y + surfaceNormals[lbc][0][1];
            z = z + surfaceNormals[lbc][0][2];
            N = N + 1;
        }
        x = x / N;
        y = y / N;
        z = z / N;
        float m = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
        vertexNormals[vertexIndex][0] = x / m;
        vertexNormals[vertexIndex][1] = y / m;
        vertexNormals[vertexIndex][2] = z / m;
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
        pz = getHeight(pick_scalar_field_value(heightplotDataset, simulation, idx), maxHeight);
        setColor(pick_scalar_field_value(simulation, idx), TEXTURE);
        glNormal3f(vertexNormals[idx][0], vertexNormals[idx][2], vertexNormals[idx][1]);
        glVertex3f(px, pz, py);

        for (i = 0; i < DIM - 1; i++) {
            px = wn + (fftw_real) i * wn;
            py = hn + (fftw_real) (j + 1) * hn;
            idx = ((j + 1) * DIM) + i;
            pz = getHeight(pick_scalar_field_value(heightplotDataset, simulation, idx), maxHeight);
            setColor(pick_scalar_field_value(simulation, idx), TEXTURE);
            glNormal3f(vertexNormals[idx][0], vertexNormals[idx][2], vertexNormals[idx][1]);
            glVertex3f(px, pz, py);
            px = wn + (fftw_real) (i + 1) * wn;
            py = hn + (fftw_real) j * hn;
            idx = (j * DIM) + (i + 1);
            pz = getHeight(pick_scalar_field_value(heightplotDataset, simulation, idx), maxHeight);
            setColor(pick_scalar_field_value(simulation, idx), TEXTURE);
            glNormal3f(vertexNormals[idx][0], vertexNormals[idx][2], vertexNormals[idx][1]);
            glVertex3f(px, pz, py);
        }

        px = wn + (fftw_real) (DIM - 1) * wn;
        py = hn + (fftw_real) (j + 1) * hn;
        idx = ((j + 1) * DIM) + (DIM - 1);
        pz = getHeight(pick_scalar_field_value(heightplotDataset, simulation, idx), maxHeight);
        setColor(pick_scalar_field_value(simulation, idx), TEXTURE);
        glNormal3f(vertexNormals[idx][0], vertexNormals[idx][2], vertexNormals[idx][1]);
        glVertex3f(px, pz, py);
        glEnd();
    }

    if (options[DRAW_NORMALS]) {
        for (int idx = 0; idx < DIM * DIM; idx++) {
            int a = idx / DIM;
            int b = idx % DIM;
            px = wn + (fftw_real) b * wn;
            py = hn + (fftw_real) a * hn;
            pz = getHeight(pick_scalar_field_value(heightplotDataset, simulation, idx), maxHeight);
            glBegin(GL_LINES);
            setColor(1, TEXTURE);
            glVertex3f(px, pz, py);
            glVertex3f(px + vertexNormals[idx][0] * 20, pz + vertexNormals[idx][2]* 20, py + vertexNormals[idx][1]* 20);
            glEnd();
        }
    }
    glDisable(GL_TEXTURE_1D);

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

int findCell(float x, float y, int DIM, float wn, float hn) {
    int C[2];

    // compute cell coordinates C[0], C[1]
    C[0] = int(x * DIM / wn);
    C[1] = int(y * DIM / hn);

    // test if p is inside the dataset
    if (C[0] < 0 || C[0] >= DIM - 1 || C[1] < 0 || C[1] >= DIM - 1)
        return -1;

    // go from cell coordinates to cell index
    return C[0] + C[1] * DIM;
}

void Visualization::draw_streamlines(Simulation const &simulation, const int DIM, const fftw_real wn, const fftw_real hn) {
    //    int i, j, idx;
    //    double px, py;

    float dt = 1.0 / 3 * sqrt(pow(wn, 2) + pow(hn, 2));
    int maxLength = 10;
    float x = 500;
    float y = 500;

    glBegin(GL_LINE_STRIP);
    glVertex2f(x, y);
    for (int step = 0; step < maxLength; step++) {

        float *xy_new = new float[2];
        int *v = new int[4];
        float *sample_x1_y1 = new float[2];
        float *sample_x2_y2 = new float[2];
        float *sample_x3_y3 = new float[2];
        float *sample_x4_y4 = new float[2];

        int x_point = floor(x / wn) - 1;
        int y_point = floor(y / hn) - 1;

        int c = (y_point * (DIM - 1)) + x_point;

        getCell(c, v, DIM);

        pick_vector_field_value(simulation, v[2], sample_x1_y1);
        pick_vector_field_value(simulation, v[1], sample_x2_y2);
        pick_vector_field_value(simulation, v[0], sample_x3_y3);
        pick_vector_field_value(simulation, v[3], sample_x4_y4);

        // bilinear interpolation
        GLfloat x1, y1, x2, y2;
        x1 = wn + (fftw_real) (x_point) * wn;
        y1 = hn + (fftw_real) (y_point) * hn;
        x2 = wn + (fftw_real) (x_point + 1) * wn;
        y2 = hn + (fftw_real) (y_point + 1) * hn;


        GLfloat f1x = sample_x4_y4[0]*(x2 - x)*(y2 - y);
        GLfloat f2x = sample_x2_y2[0]*(x - x1)*(y2 - y);
        GLfloat f3x = sample_x3_y3[0]*(x2 - x)*(y - y1);
        GLfloat f4x = sample_x1_y1[0]*(x - x1)*(y - y1);
        xy_new[0] = (1 / ((x2 - x1)*(y2 - y1)))*(f1x + f2x + f3x + f4x);
        GLfloat f1y = sample_x4_y4[1]*(x2 - x)*(y2 - y);
        GLfloat f2y = sample_x2_y2[1]*(x - x1)*(y2 - y);
        GLfloat f3y = sample_x3_y3[1]*(x2 - x)*(y - y1);
        GLfloat f4y = sample_x1_y1[1]*(x - x1)*(y - y1);
        xy_new[1] = (1 / ((x2 - x1)*(y2 - y1)))*(f1y + f2y + f3y + f4y);

        x = x + xy_new[0] * dt;
        y = y + xy_new[1] * dt;

        glVertex2f(x, y);

        delete[] v;
        delete[] xy_new;
        delete[] sample_x1_y1;
        delete[] sample_x2_y2;
        delete[] sample_x3_y3;
        delete[] sample_x4_y4;
    }
    glEnd();

    GLenum error = glGetError();
    if (error != 0) {
        std::cout << error << '\n';
    }
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
                    glEnable(GL_LIGHTING); // so the renderer considers light
                    glEnable(GL_LIGHT0); // turn LIGHT0 on
                    glEnable(GL_DEPTH_TEST); // so the renderer considers depth
                    glEnable(GL_COLOR_MATERIAL); // to be able to color objects when lighting is on
                    glShadeModel(GL_SMOOTH);

                    draw_3d_cone(magn, x_start, y_start, angle, value_for_color);
                    glDisable(GL_COLOR_MATERIAL);
                    glDisable(GL_DEPTH_TEST);
                    glDisable(GL_LIGHT0);
                    glDisable(GL_LIGHTING); // so the renderer considers light
                    break;
                case ARROWS_3D:
                    glEnable(GL_LIGHTING); // so the renderer considers light
                    glEnable(GL_LIGHT0); // turn LIGHT0 on
                    glEnable(GL_DEPTH_TEST); // so the renderer considers depth
                    glEnable(GL_COLOR_MATERIAL); // to be able to color objects when lighting is on
                    glShadeModel(GL_SMOOTH);
                    draw_3d_arrow(magn, x_start, y_start, angle, value_for_color);
                    glDisable(GL_COLOR_MATERIAL);
                    glDisable(GL_DEPTH_TEST);
                    glDisable(GL_LIGHT0);
                    glDisable(GL_LIGHTING); // so the renderer considers lightow(magn, x_start, y_start, angle, value_for_color);

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
                    glEnable(GL_LIGHTING); // so the renderer considers light
                    glEnable(GL_LIGHT0); // turn LIGHT0 on
                    glEnable(GL_DEPTH_TEST); // so the renderer considers depth
                    glEnable(GL_COLOR_MATERIAL); // to be able to color objects when lighting is on
                    glShadeModel(GL_SMOOTH);

                    draw_3d_cone(magn, x_start, y_start, angle, value_for_color);
                    glDisable(GL_COLOR_MATERIAL);
                    glDisable(GL_DEPTH_TEST);
                    glDisable(GL_LIGHT0);
                    glDisable(GL_LIGHTING);
                    break;
                case ARROWS_3D:
                    glEnable(GL_LIGHTING); // so the renderer considers light
                    glEnable(GL_LIGHT0); // turn LIGHT0 on
                    glEnable(GL_DEPTH_TEST); // so the renderer considers depth
                    glEnable(GL_COLOR_MATERIAL); // to be able to color objects when lighting is on
                    glShadeModel(GL_SMOOTH);

                    draw_3d_arrow(magn, x_start, y_start, angle, value_for_color);
                    glDisable(GL_COLOR_MATERIAL);
                    glDisable(GL_DEPTH_TEST);
                    glDisable(GL_LIGHT0);
                    glDisable(GL_LIGHTING);
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
    GLfloat scale_x = magn * 2.5;
    GLfloat scale_y_quad = magn * 0.5;
    GLfloat scale_y_triangle = magn * 0.6;
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

float Visualization::pick_scalar_field_value(DatasetType ds, Simulation const &simulation, size_t idx) {
    float value = 0.0;
    switch (ds) {
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

float Visualization::pick_scalar_field_value(Simulation const &simulation, size_t idx) {
    float value = 0.0;
    switch (scalarDataset) {
        case VELOCITY_MAGN:
            value = magnitude(simulation.vx[idx], simulation.vy[idx]);
            break;
        case FORCE_MAGN:
            value = magnitude(simulation.fx[idx], simulation.fy[idx]);
            break;
        case VELOCITY_DIV:
            value = divergence(simulation, idx, 0);
            break;
        case FORCE_DIV:
            value = divergence(simulation, idx, 1);
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
            //            divergence(simulation, i, j, wn, hn, DIM, values);
            break;
        case VELOCITY_MAGN_GRADIENT:
            //            divergence(simulation, i, j, wn, hn, DIM, values);
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

void Visualization::pick_timevector_field_value(size_t idx, float values[], int t) {
    switch (vectorDataset) {
        case FORCE:
            values[0] = Application::timeslices.getValue(t, DataBuffer::FX, idx);
            values[1] = Application::timeslices.getValue(t, DataBuffer::FY, idx);
            break;
        case VELOCITY:
            values[0] = Application::timeslices.getValue(t, DataBuffer::VX, idx);
            values[1] = Application::timeslices.getValue(t, DataBuffer::VY, idx);

        default:
            break;
    }
    values[2] = 1;
}

float Visualization::pick_timescalar_field_value(size_t idx, int t) {
    float value = 0.0;
    switch (scalarDataset) {
        case VELOCITY_MAGN:
            value = magnitude(Application::timeslices.getValue(t, DataBuffer::VX, idx), Application::timeslices.getValue(t, DataBuffer::VY, idx));
            break;
        case FORCE_MAGN:
            value = magnitude(Application::timeslices.getValue(t, DataBuffer::FX, idx), Application::timeslices.getValue(t, DataBuffer::FY, idx));
            break;
        case DENSITY:
            value = Application::timeslices.getValue(t, DataBuffer::RHO, idx);

        default:
            break;
    }
    return value;
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

float Visualization::divergence(Simulation const &simulation, int idx, int t) {
    int DIM = Simulation::DIM;
    int x = idx % DIM;
    int y = idx / DIM;
    int right = modIndex(x + 1, y, DIM);
    int left = modIndex(x - 1, y, DIM);
    int top = modIndex(x, y - 1, DIM);
    int bottom = modIndex(x, y + 1, DIM);

    float dx = 0.0;
    float dy = 0.0;

    if (t == 0) { // to use the velocity vector dataset
        dx = simulation.vx[right] - simulation.vx[left];
        dy = simulation.vy[top] - simulation.vy[bottom];
    } else { // to use the force vector dataset
        dx = simulation.fx[right] - simulation.fx[left];
        dy = simulation.fy[top] - simulation.fy[bottom];
    }

    return dx + dy;
}

int Visualization::modIndex(int x, int y, int DIM) {
    int xmod = x % DIM;
    int ymod = y % DIM;
    return ymod * DIM + xmod;
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

void Visualization::setDensityRHO1Isoline(float di) {
    densityRHO1Isoline = di;
}

void Visualization::setDensityRHO2Isoline(float di) {
    densityRHO2Isoline = di;
}

int Visualization::getNumSegmentsStreamtubes() {
    return numSegments;
}

void Visualization::setNumSegmentsStreamtubes(int n) {
    numSegments = n;
}

void Visualization::setNumIsolines(int n) {
    numIsolines = n;
}

vector<Point> Visualization::getSeedpoints() {
    return seedpoints;
}

void Visualization::addSeedpoint(int seed_x, int seed_y, int seed_z) {
    Point sp;
    sp.x = seed_x;
    sp.y = seed_y;
    sp.z = seed_z;
    seedpoints.push_back(sp);
}