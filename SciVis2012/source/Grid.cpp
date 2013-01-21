/* 
 * File:   Grid.cpp
 * Author: spyros
 * 
 * Created on January 20, 2013, 2:59 PM
 */

#include "Grid.h"

Grid::~Grid() {
}

void Grid::world2cell(int c, float *world, float *cell) {
    // implement 3.17 (book)
    if (c == findCell(world)) {
        int v[4];
        getCell(c, v);
        cell[0] = (world[0] - v[0]) * (v[1] - v[0]);
        cell[1] = (world[0] - v[0]) * (v[3] - v[0]);
    }
}

float& Grid::getC0Scalar(int i) {
    return c0_scalars[i];
}

float Grid::getC0Scalar(int c, float x, float y) {
    return getC0Scalar(c);
}

float& Grid::getC1Scalar(int i) {
    return c1_scalars[i];
}

float Grid::getC1Scalar(int c, float *p) {
    int cell[MAX_CELL_SIZE];
    int C = getCell(c, cell);
    float q[3];
    float f = 0;
    world2cell(c, p, q);
    for (int i = 0; i < C; i++)
        f += getC1Scalar(cell[i]) * Phi(i, q[0], q[1]);
    return f;
}

float* Grid::getC0Vector(int i) {
    return &(c0_vectors[3*i]);
}

float Grid::getC0Vector(int c) {
    return getC0Vector(c);
}

float* Grid::getC1Vector(int i) {
    return &(c0_vectors[3*i]);
}

float Grid::getC1Vector(int, float*) {
    
}