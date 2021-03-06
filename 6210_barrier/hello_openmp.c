#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <sys/utsname.h>
#include "gtmp.h"
#include <time.h>


int main(int argc, char **argv)
{
  int num_threads;
  struct timespec ts0;
  // Serial code
  if (argc < 2){
    fprintf(stderr, "Usage: ./hello_world [NUM_THREADS]\n");
    exit(1);
  }

  num_threads = strtol(argv[1], NULL, 10);

  omp_set_dynamic(0);
  if (omp_get_dynamic()) {printf("Warning: dynamic adjustment of threads has been set\n");}
  omp_set_num_threads(num_threads);
  clock_gettime(CLOCK_REALTIME, &ts0);
  fprintf(stderr, "initial: %lld %ld \n", (long long)ts0.tv_sec, ts0.tv_nsec);
  int i;
  for (i = 0; i < 10000; ++i)
  {
    gtmp_init(num_threads);
#pragma omp parallel
    {
      // int thread_num = omp_get_thread_num();
      struct utsname ugnm;
      
      uname(&ugnm);

      // fprintf(stderr, "Hello World from thread %d of %d, running on %s.\n", thread_num, num_threads, ugnm.nodename);
      gtmp_barrier();
      // fprintf(stderr, "%d is exiting barrier\n", thread_num);
    } // implied barrier
  }

  clock_gettime(CLOCK_REALTIME, &ts0);
  fprintf(stderr, "exit: %lld %ld \n", (long long)ts0.tv_sec, ts0.tv_nsec);
  // Resume serial code
  // printf("Back in the serial section again\n");
  gtmp_finalize();
  return 0;
}

