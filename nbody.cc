/* Copyright (c) 2013-2015, Colfax International. All Right Reserved.
This file, labs/4/4.01-overview-nbody/nbody.cc,
is a part of Supplementary Code for Practical Exercises for the handbook
"Parallel Programming and Optimization with Intel Xeon Phi Coprocessors",
2nd Edition -- 2015, Colfax International,
ISBN 9780988523401 (paper), 9780988523425 (PDF), 9780988523432 (Kindle).
Redistribution or commercial usage without written permission 
from Colfax International is prohibited.
Contact information can be found at http://colfax-intl.com/     */


#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <mkl_vsl.h>
#include <omp.h>
#define tileSize 16

struct ParticleSet { 
  float *x, *y, *z;
  float *vx, *vy, *vz; 
};

void MoveParticles(const int nParticles, ParticleSet* const particle, const float dt) {
#pragma omp parallel for schedule(guided)
  // Loop over particles that experience force
  for (int ii = 0; ii < nParticles; ii+=tileSize) { 

    // Components of the gravity force on particle i
    float Fx[tileSize], Fy[tileSize], Fz[tileSize];
	Fx[:] = Fy[:] = Fy[:] = 0;
#pragma unroll(tileSize)      
    // Loop over particles that exert force: vectorization expected here
    for (int j = 0; j < nParticles; j++) { 
	for (int i = ii; i < ii + tileSize; i++){      
      // Avoid singularity and interaction with self
      const float softening = 1e-20;

      // Newton's law of universal gravity
      const float dx = particle->x[j] - particle->x[i];
      const float dy = particle->y[j] - particle->y[i];
      const float dz = particle->z[j] - particle->z[i];
      const float drRecip  = 1.0f/sqrtf(dx*dx + dy*dy + dz*dz + 1e-20f);
      const float drPowerN32  = drRecip*drRecip*drRecip;
	
      // Calculate the net force
      Fx[i-ii] += dx * drPowerN32;  
      Fy[i-ii] += dy * drPowerN32;  
      Fz[i-ii] += dz * drPowerN32;

    }
}
for(int i=ii;i<tileSize+ii;i++){
    // Accelerate particles in response to the gravitational force
    particle->vx[i-ii] += dt*Fx[i-ii]; 
    particle->vy[i-ii] += dt*Fy[i-ii]; 
    particle->vz[i-ii] += dt*Fz[i-ii];
}
  }

  // Move particles according to their velocities
  // O(N) work, so using a serial loop
 
  for (int i = 0 ; i < nParticles; i++) { 
    particle->x[i]  += particle->vx[i]*dt;
    particle->y[i]  += particle->vy[i]*dt;
    particle->z[i]  += particle->vz[i]*dt;
  }
}

int main(const int argc, const char** argv) {

  // Problem size and other parameters
  const int nParticles = (argc > 1 ? atoi(argv[1]) : 16384);
  const int nSteps = 10;  // Duration of test
  const float dt = 0.01f; // Particle propagation time step

  // Particle data stored as an Array of Structures (AoS)
  // this is good object-oriented programming style,
  // but inefficient for the purposes of vectorization
  ParticleSet* particle = new ParticleSet[1];
  particle->x = new float[nParticles];
  particle->y = new float[nParticles];
  particle->z = new float[nParticles];
  particle->vx = new float[nParticles];
  particle->vy = new float[nParticles];
  particle->vz = new float[nParticles];

  // Initialize random number generator and particles
  VSLStreamStatePtr rnStream;  
  vslNewStream( &rnStream, VSL_BRNG_MT19937, 1 );
  //vsRngUniform(VSL_RNG_METHOD_UNIFORM_STD, 
	      //rnStream, 6*nParticles, (float*)particle, -1.0f, 1.0f);
  
  // Perform benchmark
  printf("\n\033[1mNBODY Version 00\033[0m\n");
  printf("\nPropagating %d particles using 1 thread on %s...\n\n", 
	 nParticles,
#ifndef __MIC__
	 "CPU"
#else
	 "MIC"
#endif
	 );
  double rate = 0, dRate = 0; // Benchmarking data
  const int skipSteps = 3; // Skip first iteration is warm-up on Xeon Phi coprocessor
  printf("\033[1m%5s %10s %10s %8s\033[0m\n", "Step", "Time, s", "Interact/s", "GFLOP/s"); fflush(stdout);
  for (int step = 1; step <= nSteps; step++) {

    const double tStart = omp_get_wtime(); // Start timing
    MoveParticles(nParticles, particle, dt);
    const double tEnd = omp_get_wtime(); // End timing

    const float HztoInts   = float(nParticles)*float(nParticles-1) ;
    const float HztoGFLOPs = 20.0*1e-9*float(nParticles)*float(nParticles-1);

    if (step > skipSteps) { // Collect statistics
      rate  += HztoGFLOPs/(tEnd - tStart); 
      dRate += HztoGFLOPs*HztoGFLOPs/((tEnd - tStart)*(tEnd-tStart)); 
    }

    printf("%5d %10.3e %10.3e %8.1f %s\n", 
	   step, (tEnd-tStart), HztoInts/(tEnd-tStart), HztoGFLOPs/(tEnd-tStart), (step<=skipSteps?"*":""));
    fflush(stdout);
  }
  rate/=(double)(nSteps-skipSteps); 
  dRate=sqrt(dRate/(double)(nSteps-skipSteps)-rate*rate);
  printf("-----------------------------------------------------\n");
  printf("\033[1m%s %4s \033[42m%10.1f +- %.1f GFLOP/s\033[0m\n",
	 "Average performance:", "", rate, dRate);
  printf("-----------------------------------------------------\n");
  printf("* - warm-up, not included in average\n\n");
  delete particle;
}


