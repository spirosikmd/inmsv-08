/* 
 * File:   DataBuffer.h
 * Author: cm
 *
 * Created on January 31, 2013, 11:29 AM
 */

#ifndef DATABUFFER_H
#define	DATABUFFER_H

#include <rfftw.h> 
#include <deque>
#include <vector>



class DataBuffer {
public:
    DataBuffer(int, int);
    DataBuffer(const DataBuffer& orig);
    virtual ~DataBuffer();
    
    enum DATATYPE {
        RHO, VX,VY,FX,FY,DATATYPE_SIZE
    };
    
    void addTimeslice(fftw_real rho[], fftw_real vx[],fftw_real vy[],fftw_real fx[],fftw_real fy[]);
    void changeDimension(int);
    
    
    fftw_real getValue(int time, DATATYPE type, int idx);
    const int getCapacity();
    
private:
    int capacity;
    int DIM;
    std::deque<std::vector<std::vector<fftw_real> > > buffer;
};

#endif	/* DATABUFFER_H */

