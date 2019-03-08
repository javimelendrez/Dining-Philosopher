//
//  main.cpp
//  semaphores
//
//

//#include <unistd.h>
//#include <sys/types.h>
//#include <errno.h>
//#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <semaphore.h>

#define NUM_THREADS   4
#define NUM_TIMES_RUN 30

sem_t* semaphore = NULL;
const char* semaphore_name = "/semaphore";   // name for semaphore "s"
int sum = 0;


void* runp(void* param) {
  int thisone = (int)(long)param;
  int count = 0;
  while (count++ < NUM_TIMES_RUN) {
    sem_wait(semaphore);
    for (int i = 0; i < thisone; ++i) {
      printf("|\t\t");
    }
    printf("p%d.%d (crit: sum now: %d)\n", thisone, count, ++sum);
    sem_post(semaphore);
  }
  printf("|------------------------------------------------ thread %d terminating... \n", thisone);
  return NULL;
}

void mutex(pthread_t threads[], size_t size, pthread_attr_t* attr, void* (*fp)(void*)) {
  printf("=============== MUTEXes USING SEMAPHORES =================\n");
  
  if ((semaphore = sem_open(semaphore_name, O_CREAT, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED) {
    fprintf(stderr, "sem_open failed\n");
  }
  printf("creating semaphore: \'%s\'... \n\n", semaphore_name);
  
  for (long i = 0; i < NUM_THREADS; ++i) {
    pthread_create(&threads[i], attr, fp, (void*)i);
  }
  
  for (long i = 0; i < NUM_THREADS; ++i) { pthread_join(threads[i], NULL); }
  
  printf("\nunlinking and destroying semaphore\n\n");
  sem_close(semaphore);
  sem_unlink(semaphore_name);
}

int main(int argc, const char * argv[]) {
  pthread_t  threads[NUM_THREADS];
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  
  mutex(threads, NUM_THREADS, &attr, runp);
  
  printf("Sum: Expected: %d, Actual: %d\n", NUM_THREADS * NUM_TIMES_RUN, sum);
  assert(NUM_THREADS * NUM_TIMES_RUN == sum);
  printf("\t\tAll assertions passed!\n\n");
  return 0;
}
