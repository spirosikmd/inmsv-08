/* 
 * File:   DataBuffer.cpp
 * Author: cm
 * 
 * Created on January 31, 2013, 11:29 AM
 */

#include "DataBuffer.h"
#include <iostream>
using namespace std;

DataBuffer::DataBuffer(int c, int d) {
    capacity = c;
    DIM = d;

    int numberOfValues = DIM*DIM;

    buffer.resize(capacity);
    for (int i = 0; i < c; ++i) {
        buffer[i].resize(DATATYPE_SIZE);
        for (int j = 0; j < DATATYPE_SIZE; ++j)
            buffer[i][j].resize(numberOfValues);
    }
}

DataBuffer::DataBuffer(const DataBuffer& orig) {
}

DataBuffer::~DataBuffer() {
}

void DataBuffer::changeDimension(int d) {
    DIM = d;
    int numberOfValues = DIM*DIM;
    for (int i = 0; i < capacity; ++i) {
        buffer[i].resize(DATATYPE_SIZE);
        for (int j = 0; j < DATATYPE_SIZE; ++j)
            buffer[i][j].resize(numberOfValues);
    }
}

fftw_real DataBuffer::getValue(int time, DATATYPE type, int idx) {
    return buffer[time][type][idx];
}

void DataBuffer::addTimeslice(fftw_real rho[], fftw_real vx[], fftw_real vy[], fftw_real fx[], fftw_real fy[]) {
    vector<vector<fftw_real> > timeslice;       
    int numberOfValues = DIM*DIM;
    timeslice.resize(DATATYPE_SIZE);
    for (int j = 0; j < DATATYPE_SIZE; ++j) {
        timeslice[j].resize(numberOfValues);
    }
    buffer.push_front(timeslice);
    if (buffer.size() > 20) {
        buffer.pop_back();
    }

    for (int i = 0; i < numberOfValues; i++) {
        buffer[0][RHO][i] = rho[i];
        buffer[0][VX][i] = vx[i];
        buffer[0][VY][i] = vy[i];
        buffer[0][FX][i] = fx[i];
        buffer[0][FY][i] = fy[i];
    }
}

const int DataBuffer::getCapacity() {
    return capacity;
}