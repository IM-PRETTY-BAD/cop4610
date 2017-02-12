#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int SharedVariable = 0;
void *runner(void *param);
void SimpleThread(void *arg);

int main(int argc, char *argv[]){
 if(argc!=2){
  fprintf(stderr,"usage: a.out <integer value>\n");
  return -1;
 }
 if(atoi(argv[1])<1){
  fprintf(stderr,"The number of threads must be at least 1\n");
  return -1;
 }
 int NUM_THREADS=atoi(argv[1]);
 pthread_t workers[NUM_THREADS];
 pthread_attr_t attrs[NUM_THREADS];
 int i;
 for(i=0;i<NUM_THREADS;i++){
  pthread_attr_init(&attrs[i]);
  pthread_create(&workers[i],&attrs[i],(void *)&SimpleThread,(void *)i);
 }

 for(i=0;i<NUM_THREADS;i++)
  pthread_join(workers[i],NULL);
 return 0;
}

void SimpleThread(void *arg){
 int threadID = (int)arg;
 int num, val;

 for(num=0;num<20;num++){
  if(random()>RAND_MAX/2)
   usleep(10);

  val=SharedVariable;
  printf("*** thread %d sees value %d\n", threadID, val);
  SharedVariable=val+1;
 }
 val=SharedVariable;
 printf("Thread %d sees final value %d\n",threadID,val);
 pthread_exit(0);
}