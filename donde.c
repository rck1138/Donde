/* donde.c
 * A hybrid MPI / OpenMP program that reports the CPU where each thread
 * of each rank is executing. Used to assist in determining correct 
 * binding behavior.
 * Rory Kelly
 * 3 May 2017
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <mpi.h>
#include <omp.h>

int main(int argc, char **argv){
  int mpi_id;         // MPI Task ID
  int n_mpi;          // Number of MPI Tasks
  int omp_id;         // OpenMP Thread ID
  int n_omp;          // Number of OpenMP threads
  int my_cpu;         // CPU # where task/thread is executing
  int mpi_tsup_lev;   // provided level of MPI thread support   

  char thrd_str[80];  // the per-thread output string
  char node_name[80]; // The node where process / thread is executing
  int length;         // Length of returned string

  MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &mpi_tsup_lev);
  MPI_Comm_size(MPI_COMM_WORLD, &n_mpi);
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_id);
  MPI_Get_processor_name(node_name, &length);
   
  // Print MPI Rank and OpenMP thread info in-order for readability
  for(int j=0; j<n_mpi; j++){
    if(j == mpi_id){
      #pragma omp parallel private(omp_id, n_omp, my_cpu, thrd_str)
      {
        omp_id = omp_get_thread_num();
        n_omp = omp_get_num_threads();
	      my_cpu = sched_getcpu();

        if (omp_id == 0){
          sprintf(thrd_str, "MPI Task %2d, OpenMP thread %d of %d (cpu %d)", mpi_id, omp_id, n_omp, my_cpu);
		    } else {
		      sprintf(thrd_str, "             OpenMP thread %d of %d (cpu %d)", omp_id, n_omp, my_cpu);
        }

        #pragma omp for ordered schedule(static, 1)
        for(int i=0; i<n_omp; i++){
          #pragma omp ordered
          {	
            puts(thrd_str);
          }
        }

      }
	 }
	 MPI_Barrier(MPI_COMM_WORLD);
  }
  return MPI_Finalize();
}
    
