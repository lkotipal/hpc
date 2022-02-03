#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define N 15000
int a[N][N];

void main() {
  int i,j;
  clock_t t1,t2;


  /* Begin measurement */
  t1 = clock();
  for (i=0;i<N;i++) 
    for (j=0;j<N;j++) 
      a[i][j]=(i+j)/2;
  t2 = clock();
  /* End measurement */


  for (i=0;i<N;i+=10000) for (j=0;j<N;j+=10000) printf("%d ",a[i][j]);
  printf("\n");
  printf("%f s elapsed \n", ((double) (t2 - t1)) / CLOCKS_PER_SEC);

}


  
  
    
