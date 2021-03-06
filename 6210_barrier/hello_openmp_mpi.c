#include <stdio.h>
#include <sys/utsname.h>
#include <stdlib.h>
#include <omp.h>
#include "mpi.h"
#include "gtmpi.h"
#include "gtmp.h"
#include <time.h>


int main(int argc, char **argv)
{
  struct timespec ts0;
  int req_processes = strtol(argv[1], NULL, 10);
  int num_threads = strtol(argv[2], NULL, 10);
  int my_id, num_processes;
  struct utsname ugnm;
  // printf("%d\n", num_threads);

  MPI_Init(&argc, &argv);
  
  MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_id);

  if(num_processes != req_processes){
    fprintf(stderr, "Wrong number of processes.  Required: %d, Actual: %d\n",
      req_processes, num_processes);
    exit(1);
  }

  uname(&ugnm);

  omp_set_dynamic(0);
  if (omp_get_dynamic()) {printf("Warning: dynamic adjustment of threads has been set\n");}
  omp_set_num_threads(num_threads);

  clock_gettime(CLOCK_REALTIME, &ts0);
  fprintf(stderr, "initial: %lld %ld \n", (long long)ts0.tv_sec, ts0.tv_nsec);
  int i;
  for (i = 0; i < 50; ++i)
  {
    gtmpi_init(req_processes);

    int j;
    for (j = 0; j < 50; ++j)
    {
      gtmp_init(num_threads);
#pragma omp parallel
      {
        // int thread_num = omp_get_thread_num();
        gtmp_barrier();
      }
      gtmp_finalize();
      // fprintf(stderr, "exiting thread %d of process %d, running on %s.\n", thread_num, my_id, ugnm.nodename);
    }
    gtmpi_barrier();
  }

  // fprintf(stderr, "exiting process %d of %d running on %s.\n", my_id, num_processes, ugnm.nodename);
  clock_gettime(CLOCK_REALTIME, &ts0);
  fprintf(stderr, "exit: %lld %ld \n", (long long)ts0.tv_sec, ts0.tv_nsec);
  MPI_Finalize();
  gtmpi_finalize();
  return 0;
}
