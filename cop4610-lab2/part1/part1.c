#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <stdint.h>

#ifdef PTHREAD_INSPECT
/* system call code */
#include<linux/unistd.h>

#define __NR_winicius_siqueira 341
#define __NR_garrett_siqueira_weiland 342

extern long int syscall (long int __sysno, ...) __THROW;
/* end of system call code */
#endif

bool isNumber(char number[]);

#ifdef PTHREAD_SYNC
void SimpleThread(void *);
#else
void SimpleThread(int);
#endif

//shared varaible
int shared_variable = 0;

//locks shared variable
pthread_mutex_t   sharedvar_mutex;
pthread_barrier_t barr;

int main(int argc, char *argv[]) {
  int nthreads=-1;
  if(argc != 2){printf("Need one numeric argument. E.g. %s 2\n", argv[0]);return -1;}
  else if(isNumber(argv[1])){
    nthreads = atoi(argv[1]);

    if(nthreads <= 0) {
      printf("Please enter a positive integer.\n");
      return -1;
    }
  } else{
    printf("Need one numeric argument. E.g. %s 2\n", argv[0]);return -1;
  }

      int i;
      pthread_t th_id[nthreads+1];

#ifdef PTHREAD_SYNC
      // this barrier waits for all threads to finish before outputting final value
      if(pthread_barrier_init(&barr, NULL, nthreads)) {
        printf("Error: Could not create a barrier\n");
        return -1;
      }

      // initialize mutex to lock and unlock critical section.
      if(pthread_mutex_init(&sharedvar_mutex, NULL)) {
        printf("Error: Unable to initialize a mutex\n");
        return -1;
      }
#endif

      //create the threads.
      for(i = 1; i <= nthreads; i++) {
        if(pthread_create(&th_id[i], NULL, (void *)&SimpleThread, (void *)(intptr_t)i)) {
          printf("Error: Could not create thread %d\n", i);
          return -1;
        }
      }
      #ifdef PTHREAD_INSPECT
	//enter system call to inspect thread
	printf("%lu\n", syscall(__NR_garrett_siqueira_weiland));
      #endif

      //join all the threads created
      for(i = 1; i <= nthreads; i++) {
        if(pthread_join(th_id[i], NULL)) {
          printf("Error: Could not join thread %d\n", i);
          return -1;
        }
      }

  return 0;
}

#ifdef PTHREAD_SYNC
void SimpleThread(void *args) {
  int which = (int)(intptr_t)args;
#else
void SimpleThread(int which) {
#endif
  int num, val;

  for(num = 0; num < 20; num++) {
    if(random() > RAND_MAX / 2) {
      usleep(10);
    }

#ifdef PTHREAD_SYNC
    pthread_mutex_lock(&sharedvar_mutex);
#endif

    val = shared_variable;
    printf("*** thread %d sees value %d\n", which, val);
    shared_variable = val + 1;

#ifdef PTHREAD_SYNC
    pthread_mutex_unlock(&sharedvar_mutex);
#endif

  }

#ifdef PTHREAD_SYNC
  int rc = pthread_barrier_wait(&barr);
  if(rc != 0 && rc != PTHREAD_BARRIER_SERIAL_THREAD) {
    printf("Error: Could not wait on barrier\n");
    exit(-1);
  }
#endif
  val = shared_variable;
  printf("Thread %d sees final value %d\n", which, val);

  //return 0;
}

bool isNumber(char number[])
{
    int i = 0;

    //checking for negative numbers
    if (number[0] == '-')
        i = 1;
    for (; number[i] != 0; i++)
    {
        //if (number[i] > '9' || number[i] < '0')
        if (!isdigit(number[i]))
            return false;
    }
    return true;
}
