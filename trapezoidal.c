#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#define FUNC cube
#define NUM 100000000. // number of fractions
#define MIN_X 0. // left most interval of function
#define MAX_X 1. // right most interval of function

double cube(double x){
  return x*x*x;
}

int main(int argc, char** argv){
  MPI_Init(&argc, &argv); // initial mpi

  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size); // get world size of mpi comm

  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank); // get world rank of this process

  struct timeval start_time, end_time; // define timeval for collect time

  if(world_rank == 0){
    gettimeofday(&start_time, NULL); // get start time
  }

  double fr = (MAX_X - MIN_X)/NUM;
  double area = 0, sum_area = 0; // area under the function

  for(long i = world_rank; i < NUM; i += world_size){
    area += (FUNC((fr * i)) + FUNC((fr * (i+1))))/2 * fr;
  }

  MPI_Reduce(&area, &sum_area, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  if(world_rank == 0){
    gettimeofday(&end_time, NULL); // get end time

    long duration = ((end_time.tv_sec - start_time.tv_sec)*1000000)\
                    + (end_time.tv_usec - start_time.tv_usec); // compute time in micro second
    printf("duration (msec): %ld\n", duration);
    printf("Approximated: %.30f\nErrors: %g\n", sum_area, sum_area-0.25);
  }
  MPI_Finalize();
}
