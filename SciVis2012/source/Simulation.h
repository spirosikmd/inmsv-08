#ifndef SIMULATION_H_INCLUDED
#define	SIMULATION_H_INCLUDED

#include <rfftw.h>              //the numerical simulation FFTW library
#include <math.h>               //for various math functions
		

class Parameters;		
		
class Simulation
{
	friend class Visualization;		
	
	public:
		
		static const int DIM = 40;				//size of simulation grid
				
		Simulation();		
		virtual ~Simulation();
		
		void do_one_simulation_step();
		void init_simulation(size_t n);		
		void insert_force(int X, int Y, double dx, double dy);
		void set_forces();
		
		void freeze();
		void unfreeze();
		void toggle_frozen();
		
		void change_timestep(double change);
		void scale_viscosity(float scale);
				
	private:
		
		void FFT(int direction, void* vx);		
		void solve();
		void diffuse_matter();
		
		//--- SIMULATION PARAMETERS ------------------------------------------------------------------------
		
		double dt;				//simulation time step
		float visc;				//fluid viscosity
		
		fftw_real *vx, *vy;             //(vx,vy)   = velocity field at the current moment
		fftw_real *vx0, *vy0;           //(vx0,vy0) = velocity field at the previous moment
		fftw_real *fx, *fy;	            //(fx,fy)   = user-controlled simulation forces, steered with the mouse
		fftw_real *rho, *rho0;			//smoke density at the current (rho) and previous (rho0) moment
		rfftwnd_plan plan_rc, plan_cr;  //simulation domain discretization			
		
		bool frozen;               //toggles on/off the animation
};

#endif