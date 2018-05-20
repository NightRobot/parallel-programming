#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#define FUNC cube
#define SIZE 600 // number of fractions

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
  srand(0);
  float A[SIZE][SIZE], B[SIZE][SIZE], C[SIZE][SIZE], D[SIZE][SIZE]; // define 4 arrays D for receive reducing
  for(int j = 0; j < SIZE; j++){
    for(int k = 0; k < SIZE; k++){
      C[j][k] = 0.;
    }
  }

  if(world_rank == 0){
    for(int i = 0; i < SIZE; i++){
      for(int j = 0; j < SIZE; j++){
        A[i][j] = (float)rand()/RAND_MAX*2.0-1.0; // random value in matrix
        B[i][j] = (float)rand()/RAND_MAX*2.0-1.0; // random value in matrix
      }
    }
    for(int i = 1; i < world_size; i++){
      MPI_Send(A, SIZE*SIZE, MPI_FLOAT, i, 0, MPI_COMM_WORLD); // send A to another
      MPI_Send(B, SIZE*SIZE, MPI_FLOAT, i, 1, MPI_COMM_WORLD); // send B to another
    }
  }else{
    MPI_Recv(A, SIZE*SIZE, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // receive A from root
    MPI_Recv(B, SIZE*SIZE, MPI_FLOAT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // receive B from root
  }
  MPI_Barrier(MPI_COMM_WORLD); // wait all nodes

  for(int i = world_rank; i < SIZE; i += world_size){
    for(int j = 0; j < SIZE; j++){
      for(int k = 0; k < SIZE; k++){
        C[i][j] += A[i][k] * B[k][j]; // compute matrix muliplication
      }
    }
  }

  for(int j = 0; j < SIZE; j++){
    for(int k = 0; k < SIZE; k++){
      D[j][k] = 0.;
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Reduce(&C, &D, SIZE*SIZE, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD); // reduce the matrix from all node by sum

  if(world_rank == 0){
    gettimeofday(&end_time, NULL); // get end time

    long duration = ((end_time.tv_sec - start_time.tv_sec)*1000000)\
                    + (end_time.tv_usec - start_time.tv_usec); // compute time in micro second
    printf("duration (msec): %ld\n", duration);
  }
  MPI_Finalize();
}
