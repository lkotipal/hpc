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
  int j = i ? i - 1 : nat;
  int k = i < nat - 1 ? i + 1 : nat + 1;
  
  double dxl = x[i] - x[j];
  double dxr = x[k] - x[i];
  if (dxl < -box / 2.0) 
    dxl+=box;
  if (dxl >= box / 2.0) 
    dxl-=box;
  if (dxr < -box / 2.0) 
    dxr+=box;
  if (dxr >= box / 2.0) 
    dxr-=box;
  dxl -= d;
  dxr -= d;

  *u = (k1 * (dxl * dxl + dxr * dxr) + k2 * (dxl * dxl * dxl + dxr * dxr * dxr)) / 2.0;
  *a = -(2.0 * k1 * (dxl - dxr) + 3.0 * k2 * (dxl * dxl - dxr * dxr));
}

void printcoords(int nat, int n, double *x, double *ep, double box) {
  fprintf(fd,"%d\n",nat);
  fprintf(fd," Frame number %d %d fs boxsize %f 10.0 10.0\n",n,n,box);
  for (int i = 0; i < nat; i++) 
    fprintf(fd,"Fe %20.10g %20.10g %20.10g %20.10g\n",xsc*x[i],0.0,0.0,ep[i]);
}


int main(int argc, char **argv)
{
	const int tag=50;
	int id, ntasks, rc;
	MPI_Status status;

	if (MPI_Init(&argc,&argv) != MPI_SUCCESS) {
		fprintf(stderr, "MPI initialization failed\n");
		exit(1);
	}
	MPI_Comm_size(MPI_COMM_WORLD,&ntasks);
	MPI_Comm_rank(MPI_COMM_WORLD,&id);

  int prev_id = id ? id - 1 : ntasks - 1;
  int next_id = id < ntasks - 1 ? id + 1 : 0;

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
  
  int total_nat = atoi(argv[1]);

  if (total_nat % ntasks) {
    fprintf(stderr,"nat must be divisble by ntasks\n");
    MPI_Finalize();
    return -1;
  }
  int my_nat = total_nat / ntasks;
  
  double dt = atof(argv[2]);
  int maxt = atoi(argv[3]);
  double vsc = atof(argv[4]);
  
  int eout = argc > 5 ? atoi(argv[5]) : 1;
  int cout = argc > 6 ? atoi(argv[6]) : 0;
  
  // x[my_nat] and x[my_nat + 1] left and right neighbors respectively
  double* x = (double*) malloc((size_t) (my_nat + 2) * sizeof(double));
  double* v = (double*) malloc((size_t) my_nat * sizeof(double));
  double* v0 = (double*) malloc((size_t) my_nat * sizeof(double));
  double* a = (double*) malloc((size_t) my_nat * sizeof(double));
  double* ep = (double*) malloc((size_t) my_nat * sizeof(double));
  double* ek = (double*) malloc((size_t) my_nat * sizeof(double));

  double* ep_sums = id ? NULL : (double*) malloc((size_t) ntasks * sizeof(double));
  double* ek_sums = id ? NULL : (double*) malloc((size_t) ntasks * sizeof(double));

  double* all_x = id ? NULL : (double*) malloc((size_t) total_nat * sizeof(double));
  double* all_ep = id ? NULL : (double*) malloc((size_t) total_nat * sizeof(double));
  
  // Initialize atoms positions and give them random velocities
  double box = total_nat;
  //srand(time(NULL));
  // Use fixed seed according to task id
  srand(id);
  for (int i = 0; i < my_nat; i++) {
    x[i] = i + id * my_nat;
    v[i] = vsc * (double) rand() / RAND_MAX;
  }

  // Send left, receive from right
  MPI_Send(&x[0], 1, MPI_DOUBLE, prev_id, tag, MPI_COMM_WORLD);
  MPI_Recv(&x[my_nat + 1], 1, MPI_DOUBLE, next_id, tag, MPI_COMM_WORLD, &status);

  // Send right, receive from left
  MPI_Send(&x[my_nat - 1], 1, MPI_DOUBLE, next_id, tag, MPI_COMM_WORLD);
  MPI_Recv(&x[my_nat], 1, MPI_DOUBLE, prev_id, tag, MPI_COMM_WORLD, &status);

  if (cout>0) 
    fd=fopen("coords.dat","w");
  
  // Remove center of mass velocity
  double vsum = 0.0;
  for (int i = 0; i < my_nat; i++) 
    vsum += v[i];
  vsum /= my_nat;
  for (int i = 0; i < my_nat; i++) 
    v[i] -= vsum;
  
  // If the user wants calculate initial energy and print initial coords
  if (cout > 0) {
    for (int i = 0; i < my_nat; i++) 
      accel(my_nat, i, &ep[i], &a[i], box, x);

    MPI_Gather(x, my_nat, MPI_DOUBLE, all_x, my_nat, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Gather(ep, my_nat, MPI_DOUBLE, all_ep, my_nat, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    if (id == 0)
      printcoords(my_nat, 0, x, ep, box);
  }
  
  
  // Simulation proper
  
  for (int n = 0; n < maxt; n++) {
    // Send left, receive from right
    MPI_Send(&x[0], 1, MPI_DOUBLE, prev_id, tag, MPI_COMM_WORLD);
    MPI_Recv(&x[my_nat + 1], 1, MPI_DOUBLE, next_id, tag, MPI_COMM_WORLD, &status);

    // Send right, receive from left
    MPI_Send(&x[my_nat - 1], 1, MPI_DOUBLE, next_id, tag, MPI_COMM_WORLD);
    MPI_Recv(&x[my_nat], 1, MPI_DOUBLE, prev_id, tag, MPI_COMM_WORLD, &status);
    
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
      double vave = (v0[i] + v[i]) / 2.0;
      ek[i] = 1.0 / 2.0 * vave * vave;
    }


     // Calculate and print total potential end kinetic energies
     // and their sum that should be conserved.

    if (eout && n % eout == 0) {
      double epsum = 0.0;
      double eksum = 0.0;
      for (int i = 0; i < my_nat; i++) 
        epsum+=ep[i];
      for (int i = 0; i < my_nat; i++) 
        eksum += ek[i];
      
      MPI_Gather(&epsum, 1, MPI_DOUBLE, ep_sums, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
      MPI_Gather(&eksum, 1, MPI_DOUBLE, ek_sums, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

      if (id == 0) {
        for (int i = 1; i < ntasks; ++i) {
          epsum += ep_sums[i];
          eksum += ek_sums[i];
        }

        printf("%20.10g %20.10g %20.10g %20.10g\n",dt*n,epsum+eksum,epsum,eksum);
      }
    }

    if (cout && n % cout == 0) {
      MPI_Gather(x, my_nat, MPI_DOUBLE, all_x, my_nat, MPI_DOUBLE, 0, MPI_COMM_WORLD);
      MPI_Gather(ep, my_nat, MPI_DOUBLE, all_ep, my_nat, MPI_DOUBLE, 0, MPI_COMM_WORLD);
      
      if (id == 0)
        printcoords(total_nat, n, all_x, all_ep, box);
    }

  }

  if (cout>0) 
    fclose(fd);

  MPI_Finalize();
  return 0;
}

