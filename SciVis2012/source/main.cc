// Usage: Drag with the mouse to add smoke to the fluid. This will also move a "rotor" that disturbs
//        the velocity field at the mouse location. Press the indicated keys to change options
//--------------------------------------------------------------------------------------------------

#include <iostream>
#include "Application.h"
#include "UniformGrid.h"

using namespace std;

int main(int argc, char** argv)
{	
//    UniformGrid *ug = new UniformGrid(40, 40, 0, 0, 1000, 800);
//    cout << "points: " << ug->numPoints() << '\n';
//    cout << "cells: " << ug->numCells() << '\n';
//    float *p = new float[2];
//    ug->getPoint(1599, p);
//    cout << "p0: " << p[0] << "  p1: " << p[1] << endl;
//    cout << "new point here" << endl;
//    p[0] = 1;
//    p[1] = 1;
//    int c = ug->findCell(p);
//    cout << "cell id: " << c << endl;
//    int *v = new int[4];
//    ug->getCell(c, v);
//    cout << v[0] << ' ' << v[1] << ' ' << v[2] << ' ' << v[3] << endl;
    try
    {		
        Application::outputUsage();
        Application::initialize(&argc, argv);
    }
    catch(exception &ex)
    {
        cerr << "an exception occurred: " << ex.what() << '\n';		
    }
    catch(...)
    {
        cerr << "an exception occurred.\n";		
    }
}