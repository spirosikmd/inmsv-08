/* 
 * File:   UniformGrid.h
 * Author: spyros
 *
 * Created on January 20, 2013, 3:03 PM
 */

#ifndef UNIFORMGRID_H
#define	UNIFORMGRID_H

#include "Grid.h"

class UniformGrid : public Grid {
public:
    UniformGrid(int, int, float, float, float, float);
    int numPoints();
    int numCells();
    void getPoint(int, float *);
    int getCell(int, int *);
    int findCell(float *);
    int getDimension1();
    int getDimension2();
    int lex(int, int);
protected:
    int N1, N2;
    float m1, m2;
private:
    float d1, d2;
};

#endif	/* UNIFORMGRID_H */

