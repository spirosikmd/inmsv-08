/* 
 * File:   UniformGrid.cpp
 * Author: spyros
 * 
 * Created on January 20, 2013, 3:03 PM
 */

#include "UniformGrid.h"
#include <iostream>

using namespace std;

UniformGrid::UniformGrid(int N1_, int N2_, float m1_, float m2_, float M1, float M2) :
        N1(N1_), N2(N2_), m1(m1_), m2(m2_), d1((M1-m1)/N1_), d2((M2-m2)/N2_) {
}

int UniformGrid::numPoints() {
    return N1*N2;
}

int UniformGrid::numCells() {
    return (N1 - 1) * (N2 - 1);
}

void UniformGrid::getPoint(int i, float *p) {
    p[0] = m1 + (i % N1) * d1;
    p[1] = m2 + (i / N1) * d2;
}

int UniformGrid::getCell(int c, int *v) {
    int C[2];
    int P = N1;
    
    // compute cell coordinates C[0], C[1]
    C[1] = c / P;
    c -= C[1] * P;
    C[0] = c;
    
    // now go from cell coordinates to vertex coordinates
    int i[2];
    int j = 0;
    for (i[0] = 0; i[0] < 2; i[0]++)
        for (i[1] = 0; i[1] < 2; i[1]++) {
            cout << C[0] + i[0] << ' ' << C[1] + i[1] << endl;
            v[j++] = lex(C[0] + i[0], C[1] + i[1]);
        }
    
    return 2;
}

int UniformGrid::findCell(float *p) {
    int C[2];
    
    // compute cell coordinates C[0], C[1]
    C[0] = int((p[0] - m1) * N1/d1);
    C[1] = int((p[1] - m2) * N2/d2);
    
    cout << "C[0]: " << C[0] << "  C[1]: " << C[1] << endl;
    
    // test if p is inside the dataset
    if (C[0] < 0 || C[0] >= N1-1 || C[1] < 0 || C[1] >= N2-1)
        return -1;
    
    // go from cell coordinates to cell index
    return C[0] + C[1]*N1;
}

int UniformGrid::getDimension1() {
    return N1;
}

int UniformGrid::getDimension2() {
    return N2;
}

int UniformGrid::lex(int n1, int n2) {
    return n1 + n2*N1;
}