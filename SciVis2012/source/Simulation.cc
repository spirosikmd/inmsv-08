#include "Simulation.h"
#include "Utilities.h"

int Simulation::DIM;

Simulation::Simulation() {
    dt = 0.4; //simulation time step
    visc = 0.001; //fluid viscosity
    frozen = 0; //toggles on/off the animation
    DIM = 40;
}

Simulation::~Simulation() {
    delete[] vx;
    delete[] vy;
    delete[] vx0;
    delete[] vy0;
    delete[] fx;
    delete[] fy;
    delete[] rho;
    delete[] rho0;
}

void Simulation::freeze() {
    frozen = true;
}

void Simulation::unfreeze() {
    frozen = false;
}

void Simulation::toggle_frozen() {
    frozen = !frozen;
}

void Simulation::change_timestep(double change) {
    dt += change;
}

void Simulation::scale_viscosity(float scale) {
    visc *= scale;
}

void Simulation::set_DIM(int dim) {
    DIM = dim;
}

int Simulation::get_DIM() {
    return DIM;
}

//------ SIMULATION CODE STARTS HERE -----------------------------------------------------------------

//init_simulation: Initialize simulation data structures as a function of the grid size 'n'.
//                 Although the simulation takes place on a 2D grid, we allocate all data structures as 1D arrays,
//                 for compatibility with the FFTW numerical library.

void Simulation::init_simulation() {
    //Allocate data structures
    size_t n = DIM;
    size_t dim = n * 2 * (n / 2 + 1);
    vx = new fftw_real[dim];
    vy = new fftw_real[dim];
    vx0 = new fftw_real[dim];
    vy0 = new fftw_real[dim];

    fx = new fftw_real[n * n];
    fy = new fftw_real[n * n];
    rho = new fftw_real[n * n];
    rho0 = new fftw_real[n * n];

    plan_rc = rfftw2d_create_plan(n, n, FFTW_REAL_TO_COMPLEX, FFTW_IN_PLACE);
    plan_cr = rfftw2d_create_plan(n, n, FFTW_COMPLEX_TO_REAL, FFTW_IN_PLACE);

    // Initialize data structures to 0
    for (size_t i = 0; i < n * n; i++) {
        vx[i] = vy[i] = vx0[i] = vy0[i] = fx[i] = fy[i] = rho[i] = rho0[i] = 0.0f;
    }
}


//FFT: Execute the Fast Fourier Transform on the dataset 'vx'.
//     'dirfection' indicates if we do the direct (1) or inverse (-1) Fourier Transform

void Simulation::FFT(int direction, void* vx) {
    if (direction == 1) rfftwnd_one_real_to_complex(plan_rc, (fftw_real*) vx, (fftw_complex*) vx);
    else rfftwnd_one_complex_to_real(plan_cr, (fftw_complex*) vx, (fftw_real*) vx);
}

//solve: Solve (compute) one step of the fluid flow simulation

void Simulation::solve() {
    int n = DIM;
    fftw_real x, y, x0, y0, f, r, U[2], V[2], s, t;
    int i, j, i0, j0, i1, j1;

    for (i = 0; i < n * n; i++) {
        vx[i] += dt * vx0[i];
        vx0[i] = vx[i];
        vy[i] += dt * vy0[i];
        vy0[i] = vy[i];
    }

    for (x = 0.5f / n, i = 0; i < n; i++, x += 1.0f / n)
        for (y = 0.5f / n, j = 0; j < n; j++, y += 1.0f / n) {
            x0 = n * (x - dt * vx0[i + n * j]) - 0.5f;
            y0 = n * (y - dt * vy0[i + n * j]) - 0.5f;
            i0 = clamp(x0);
            s = x0 - i0;
            i0 = (n + (i0 % n)) % n;
            i1 = (i0 + 1) % n;
            j0 = clamp(y0);
            t = y0 - j0;
            j0 = (n + (j0 % n)) % n;
            j1 = (j0 + 1) % n;
            vx[i + n * j] = (1 - s)*((1 - t) * vx0[i0 + n * j0] + t * vx0[i0 + n * j1]) + s * ((1 - t) * vx0[i1 + n * j0] + t * vx0[i1 + n * j1]);
            vy[i + n * j] = (1 - s)*((1 - t) * vy0[i0 + n * j0] + t * vy0[i0 + n * j1]) + s * ((1 - t) * vy0[i1 + n * j0] + t * vy0[i1 + n * j1]);
        }

    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++) {
            vx0[i + (n + 2) * j] = vx[i + n * j];
            vy0[i + (n + 2) * j] = vy[i + n * j];
        }

    FFT(1, vx0);
    FFT(1, vy0);

    for (i = 0; i <= n; i += 2) {
        x = 0.5f * i;
        for (j = 0; j < n; j++) {
            y = j <= n / 2 ? (fftw_real) j : (fftw_real) j - n;
            r = x * x + y*y;
            if (r == 0.0f) continue;
            f = (fftw_real) exp(-r * dt * visc);
            U[0] = vx0[i + (n + 2) * j];
            V[0] = vy0[i + (n + 2) * j];
            U[1] = vx0[i + 1 + (n + 2) * j];
            V[1] = vy0[i + 1 + (n + 2) * j];

            vx0[i + (n + 2) * j] = f * ((1 - x * x / r) * U[0] - x * y / r * V[0]);
            vx0[i + 1 + (n + 2) * j] = f * ((1 - x * x / r) * U[1] - x * y / r * V[1]);
            vy0[i + (n + 2) * j] = f * (-y * x / r * U[0] + (1 - y * y / r) * V[0]);
            vy0[i + 1 + (n + 2) * j] = f * (-y * x / r * U[1] + (1 - y * y / r) * V[1]);
        }
    }

    FFT(-1, vx0);
    FFT(-1, vy0);

    f = 1.0 / (n * n);
    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++) {
            vx[i + n * j] = f * vx0[i + (n + 2) * j];
            vy[i + n * j] = f * vy0[i + (n + 2) * j];
        }
}


// diffuse_matter: This function diffuses matter that has been placed in the velocity field. It's almost identical to the
// velocity diffusion step in the function above. The input matter densities are in rho0 and the result is written into rho.

void Simulation::diffuse_matter() {
    int n = DIM;
    fftw_real x, y, x0, y0, s, t;
    int i, j, i0, j0, i1, j1;

    for (x = 0.5f / n, i = 0; i < n; i++, x += 1.0f / n) {
        for (y = 0.5f / n, j = 0; j < n; j++, y += 1.0f / n) {
            x0 = n * (x - dt * vx[i + n * j]) - 0.5f;
            y0 = n * (y - dt * vy[i + n * j]) - 0.5f;
            i0 = clamp(x0);
            s = x0 - i0;
            i0 = (n + (i0 % n)) % n;
            i1 = (i0 + 1) % n;
            j0 = clamp(y0);
            t = y0 - j0;
            j0 = (n + (j0 % n)) % n;
            j1 = (j0 + 1) % n;
            rho[i + n * j] = (1 - s)*((1 - t) * rho0[i0 + n * j0] + t * rho0[i0 + n * j1]) + s * ((1 - t) * rho0[i1 + n * j0] + t * rho0[i1 + n * j1]);
        }
    }
}

void Simulation::insert_force(int X, int Y, double dx, double dy) {
    fx[Y * DIM + X] += dx;
    fy[Y * DIM + X] += dy;
    rho[Y * DIM + X] = 10.0f;
}

// set_forces: copy user-controlled forces to the force vectors that are sent to the solver.
// Also dampen forces and matter density to get a stable simulation.

void Simulation::set_forces() {
    for (int i = 0; i < DIM * DIM; i++) {
        rho0[i] = 0.995 * rho[i];
        fx[i] *= 0.85;
        fy[i] *= 0.85;
        vx0[i] = fx[i];
        vy0[i] = fy[i];
    }
}

//do_one_simulation_step: Do one complete cycle of the simulation:
//      - set_forces:
//      - solve:            read forces from the user
//      - diffuse_matter:   compute a new set of velocities

void Simulation::do_one_simulation_step() {
    if (!frozen) {
        set_forces();
        solve();
        diffuse_matter();
    }
}