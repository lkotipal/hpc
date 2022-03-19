/*
 Try e.g.
./a.out 10000 0.001 100000 1 100 0 | \
 gawk '{print $1,$2 > "etot";print $1,$3 > "epot"; print $1,$4 > "ekin"}'; xgraph etot epot ekin
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

#define d  1.0
#define k1 1.0
#define k2 0.1
#define xsc 2.35

FILE *fd;


void accel(int nat, int i, double *u, double *a, double box, double *x) {
  // Calculate the potential energy u 
  // and acceleration a of atom i.
  int j, k;
  double dxl, dxr;
      
  j = i ? i - 1 : nat - 1;
  k = i < nat - 1 ? i + 1 : 0;
  
  dxl=x[i]-x[j];
  dxr=x[k]-x[i];
  if (dxl <- box / 2.0) 
    dxl+=box;
  if (dxl >= box / 2.0) 
    dxl-=box;
  if (dxr <- box / 2.0) 
    dxr+=box;
  if (dxr >= box / 2.0) 
    dxr-=box;
  dxl -= d;
  dxr -= d;

  *u = (k1 * (dxl * dxl + dxr * dxr) + k2 * (dxl * dxl * dxl + dxr * dxr * dxr)) / 2.0;
  *a = -(2.0 * k1 * (dxl - dxr) + 3.0 * k2 * (dxl * dxl - dxr * dxr));
}

void printcoords(int nat, int n, double *x, double *ep, double box) {
  int i;
  fprintf(fd,"%d\n",nat);
  fprintf(fd," Frame number %d %d fs boxsize %f 10.0 10.0\n",n,n,box);
  for (i=0;i<nat;i++) 
    fprintf(fd,"Fe %20.10g %20.10g %20.10g %20.10g\n",xsc*x[i],0.0,0.0,ep[i]);
}


int main(int argc, char **argv)
{

  double *x;             // atom positions
  double *v;             //      velocities
  double *v0;            //      previous veloocities (leap frog needs them)
  double *a;             //      accelerations
  double *ep;            //      potential energies
  double *ek;            //      kinetic energies

  double dt;             // time step 
  double vsc;            // mean initial velocity
  double box;            // system size
  int total_nat;         // number of atoms
  int my_nat;            // number of atoms in process
  int maxt;              // number of time steps simulated
  int eout;              // energy output interval
  int cout;              // coordinate output interval (lot of data, beware!)
  
  double vsum,vave;

	const int tag=50;
	int id, ntasks, rc;
	MPI_Status status;

	if (MPI_Init(&argc,&argv) != MPI_SUCCESS) {
		fprintf(stderr, "MPI initialization failed\n");
		exit(1);
	}
	MPI_Comm_size(MPI_COMM_WORLD,&ntasks);
	MPI_Comm_rank(MPI_COMM_WORLD,&id);

  // Get number of atoms, time step and simulation length from command line
  if (argc<5 || argc>7) {
    fprintf(stderr,"usage: %s nat dt maxt vsc [eout [cout]]\n",argv[0]);
    fprintf(stderr,"    nat  = number of atoms\n");
    fprintf(stderr,"    dt   = time step\n");
    fprintf(stderr,"    maxt = number of time steps in simulation\n");
    fprintf(stderr,"    vsc = mean velocity of atoms in the beginning ('temperature')\n");
    fprintf(stderr,"    eout = interval for printing energies to stdout\n");
    fprintf(stderr,"    cout = interval for printing coordinates to 'coords.dat'\n");

    MPI_Finalize();
    return -1;
  }
  
  total_nat = atoi(argv[1]);

  if (total_nat % ntasks) {
    fprintf(stderr,"nat must be divisble by ntasks\n");
    MPI_Finalize();
    return -1;
  }
  my_nat = total_nat / ntasks;
  
  dt = atof(argv[2]);
  maxt = atoi(argv[3]);
  vsc = atof(argv[4]);
  
  eout = argc > 5 ? atoi(argv[5]) : 1;
  cout = argc > 6 ? atoi(argv[6]) : 0;
  
  x = (double*) malloc((size_t) my_nat * sizeof(double));
  v = (double*) malloc((size_t) my_nat * sizeof(double));
  v0 = (double*) malloc((size_t) my_nat * sizeof(double));
  a = (double*) malloc((size_t) my_nat * sizeof(double));
  ep = (double*) malloc((size_t) my_nat * sizeof(double));
  ek = (double*) malloc((size_t) my_nat * sizeof(double));
  
  // Initialize atoms positions and give them random velocities
  box=my_nat;
  srand(time(NULL));
  for (int i = 0; i<my_nat; i++) {
    x[i] = i;
    v[i] = vsc * (double) rand() / RAND_MAX;
  }

  if (cout>0) 
    fd=fopen("coords.dat","w");
  
  // Remove center of mass velocity
  vsum = 0.0;
  for (int i = 0; i < my_nat; i++) 
    vsum += v[i];
  vsum/=my_nat;
  for (int i = 0; i < my_nat; i++) 
    v[i] -= vsum;
  
  n = 0;
  
  // If the user wants calculate initial energy and print initial coords
  if (cout > 0) {
    for (i = 0; i < my_nat; i++) 
      accel(my_nat, i, &ep[i], &a[i], box, x);
    printcoords(my_nat, n, x, ep, box);
  }
  
  
  // Simulation proper
  
  for (int n=0; n < maxt; n++) {
    
    for (int i = 0; i < my_nat; i++) 
      v0[i]=v[i];
    
    for (int i = 0; i < my_nat; i++)
      accel(my_nat,i,&ep[i],&a[i],box,x); // New potential energy and acceleration
    
    for (int i = 0; i < my_nat; i++) {
      // Leap frog integration algorithm: update position and velocity
      v[i] = v[i] + dt * a[i];
      x[i] = x[i] + dt * v[i];
      
      // Check periodic boundary conditions
      if (x[i] < 0.0) 
        x[i]=x[i]+box;
      if (x[i] >= box) 
        x[i]=x[i]-box;
      
      // Calculate kinetic energy (note: mass=1)
      vave = (v0[i] + v[i]) / 2.0;
      ek[i] = 1.0 / 2.0 * vave * vave;
    }


     // Calculate and print total potential end kinetic energies
     // and their sum that should be conserved.

    if (eout && n % eout == 0) {
      double epsum = 0.0;
      double eksum = 0.0;
      for (i = 0;i < my_nat; i++) 
        epsum+=ep[i];
      for (i = 0; i < my_nat; i++) 
        eksum += ek[i];

      printf("%20.10g %20.10g %20.10g %20.10g\n",dt*n,epsum+eksum,epsum,eksum);
    }
    if (cout && n % cout == 0) 
      printcoords(my_nat, n, x, ep, box);

  }

  if (cout>0) 
    fclose(fd);

  MPI_Finalize();
  return 0;
}

