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
  int j,k;
  double dxl,dxr;
      
  j=i-1; if (j<0) j=nat-1;
  k=i+1; if (k>=nat) k=0;
  
  dxl=x[i]-x[j];
  dxr=x[k]-x[i];
  if (dxl<-box/2.0) dxl+=box;
  if (dxl>=box/2.0) dxl-=box;
  if (dxr<-box/2.0) dxr+=box;
  if (dxr>=box/2.0) dxr-=box;
  dxl-=d;
  dxr-=d;

  *u=(k1*(dxl*dxl+dxr*dxr)+k2*(dxl*dxl*dxl+dxr*dxr*dxr))/2.0;
  *a=-(2.0*k1*(dxl-dxr)+3.0*k2*(dxl*dxl-dxr*dxr));
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

  double epsum,eksum;    // system energies
  double dt;             // time step 
  double vsc;            // mean initial velocity
  double box;            // system size
  int nat;               // number of atoms
  int maxt;              // number of time steps simulated
  int eout;              // energy output interval
  int cout;              // coordinate output interval (lot of data, beware!)
  
  int i,n;
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
    return(-1);
  }
  
  cout=0;
  eout=1;
  nat=atoi(*++argv);
  dt=atof(*++argv);
  maxt=atoi(*++argv);
  vsc=atof(*++argv);
  
  if (argc>5) eout=atoi(*++argv);
  if (argc>6) cout=atoi(*++argv);
  
  x=(double *)malloc((size_t)nat*sizeof(double));
  v=(double *)malloc((size_t)nat*sizeof(double));
  v0=(double *)malloc((size_t)nat*sizeof(double));
  a=(double *)malloc((size_t)nat*sizeof(double));
  ep=(double *)malloc((size_t)nat*sizeof(double));
  ek=(double *)malloc((size_t)nat*sizeof(double));
  
  // Initialize atoms positions and give them random velocities
  box=nat;
  srand(time(NULL));
  for (i=0;i<nat;i++) {
    x[i]=i;
    v[i]=vsc*(double)rand()/RAND_MAX;
  }

  if (cout>0) fd=fopen("coords.dat","w");
  
  // Remove center of mass velocity
  vsum=0.0;
  for (i=0;i<nat;i++) vsum+=v[i];
  vsum/=nat;
  for (i=0;i<nat;i++) v[i]-=vsum;
  
  n=0;
  
  // If the user wants calculate initial energy and print initial coords
  if (cout>0) {
    for (i=0;i<nat;i++) accel(nat,i,&ep[i],&a[i],box,x);
    printcoords(nat,n,x,ep,box);
  }
  
  
  // Simulation proper
  
  for (n=0;n<maxt;n++) {
    
    for (i=0;i<nat;i++) v0[i]=v[i];
    
    for (i=0;i<nat;i++)
      // New potential energy and acceleration
      accel(nat,i,&ep[i],&a[i],box,x);
    
    for (i=0;i<nat;i++) {
      
      // Leap frog integration algorithm: update position and velocity
      v[i]=v[i]+dt*a[i];
      x[i]=x[i]+dt*v[i];
      
      // Check periodic boundary conditions
      if (x[i]<0.0 ) x[i]=x[i]+box;
      if (x[i]>=box) x[i]=x[i]-box;
      
      // Calculate kinetic energy (note: mass=1)
      vave=(v0[i]+v[i])/2.0;
      ek[i]=1.0/2.0*vave*vave;
      
    }


     // Calculate and print total potential end kinetic energies
     // and their sum that should be conserved.
    epsum=eksum=0.0;
    for (i=0;i<nat;i++) epsum+=ep[i];
    for (i=0;i<nat;i++) eksum+=ek[i];
    if (eout>0)
      if (n%eout==0)
	printf("%20.10g %20.10g %20.10g %20.10g\n",dt*n,epsum+eksum,epsum,eksum);
    if (cout>0) 
        if (n%cout==0)
	  printcoords(nat,n,x,ep,box);

  }

  if (cout>0) fclose(fd);

  MPI_Finalize();
  return(0);
}

