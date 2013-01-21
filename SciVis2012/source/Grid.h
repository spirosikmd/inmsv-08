/* 
 * File:   Grid.h
 * Author: spyros
 *
 * Created on January 20, 2013, 2:59 PM
 */

#ifndef GRID_H
#define	GRID_H

#include <vector>

using namespace std;

class Grid {
public:
    virtual ~Grid();
    virtual int numPoints() = 0;
    virtual int numCells() = 0;
    virtual void getPoint(int, float *) = 0;
    virtual int getCell(int, int *) = 0;
    virtual int findCell(float *) = 0;
    void world2cell(int, float *, float *);
    float& getC0Scalar(int);
    float getC0Scalar(int, float, float);
    float& getC1Scalar(int);
    float getC1Scalar(int, float *);
    float* getC0Vector(int);
    float getC0Vector(int, float, float);
    float* getC1Vector(int);
    float getC1Vector(int, float *);
    float Phi(int, float float);
    
protected:
    vector<float> c0_scalars;
    vector<float> c1_scalars;
    vector<float> c0_vectors;
    vector<float> c1_vectors;
    
private:
    const int MAX_CELL_SIZE = 8;
};

#endif	/* GRID_H */

