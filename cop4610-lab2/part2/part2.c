/*
Michael Garrett, Winicius Siqueira, Michael Weiland
COP 4610L - Lab 2
Part 2: News Conference Multithreaded Programming
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <ctype.h>

pthread_t * Speaker();
pthread_t * Reporter(int id);
void AnswerStart();
void AnswerDone();
void EnterConferenceRoom(int id);
void LeaveConferenceRoom(int id);
void QuestionStart(int id);
void QuestionDone(int id);

void * SpeakerThread(void *args);
void * ReporterThread(void *args);

bool isNumber(char number[]);

//shared variable that holds the id of the reporter asking the question
int question_asker_id = -1;

int started = 0;
int nconference;

//semaphore variable that sets max amount of reporters allowed in conference room
sem_t conference_room_sem;

//allows one reporter to talk at a time.
pthread_mutex_t speech_mutex;

//allows a speaker or reporter to take turns talking
pthread_mutex_t floor_mutex;
pthread_cond_t  floor_cond;


int main(int argc, char *argv[]) {
  int nthreads=-1;
  int nconference=-1;
  if(argc != 3){printf("Need two positive numeric arguments.\n The first one is the amount of reporters in the conference room, and the second is the max amount of reporters allowed in the conference room.\nE.g. %s 2\n", argv[0]);return -1;}
  else if(isNumber(argv[1]) && isNumber(argv[2])){
    nthreads = atoi(argv[1]);
    nconference = atoi(argv[2]);
    if(nthreads <= 0) {
      printf("Please enter a positive integer.\n");
      return -1;
    }
    if(nconference <= 0) {
      printf("Please enter a positive integer.\n");
      return -1;
    }
  } else{
    printf("Need two positive numeric arguments. E.g. %s 2\n", argv[0]);
    return -1;
  }

    //Initialize semaphore, mutex, and mutex lock conditions.
    if(sem_init(&conference_room_sem, 0, nconference) ||
       pthread_mutex_init(&speech_mutex, NULL) ||
       pthread_mutex_init(&floor_mutex, NULL) ||
       pthread_cond_init(&floor_cond, NULL)) {
      printf("Error: Could not initialize semaphore, mutex, or mutex lock condition.\n");
      return -1;
    }

    int i;
    pthread_t * speaker;
    pthread_t *  reporters[nthreads];

    //floor is locked until all threads are created
    pthread_mutex_lock(&floor_mutex);

    speaker = Speaker();
    if(speaker == 0) {
      printf("Error: Could not create Speaker thread.\n");
      return -1;
    }

    //Wait on the condition, this is to allow the speaker to be created and then the speaker will signal and wait
    pthread_cond_wait(&floor_cond, &floor_mutex);

    //speaker is done being created and is currently waiting.
    //started var represents that we have started. */
    started = 1;
    pthread_mutex_unlock(&floor_mutex);

    //create reporter threads
    for(i = 0; i < nthreads; i++) {
      reporters[i] = Reporter(i);
      if(*reporters[i] == 0) {
        printf("Error: Could not create Reporter(%d) thread.\n", i);
        return -1;
      }
    }

    //join reporter threads
    for(i = 0; i < nthreads; i++) {
      if(pthread_join(*reporters[i], NULL)) {
        printf("Error: Could not join Reporter(%d) thread.\n", i);
        return -1;
      }
    }

  //}
  return 0;
}

//creates the speaker thread and returns pointer to thread
pthread_t * Speaker() {
  pthread_t *thread = (pthread_t *)malloc(sizeof(pthread_t));
  if(pthread_create(thread, NULL, SpeakerThread, NULL)) {
    return 0;
  }
  return thread;
}

//creates a reported thread and returns a pointer to thread
pthread_t * Reporter(int id) {
  int *a = (int *)malloc(sizeof(int));
  *a = id;

  pthread_t *thread = (pthread_t *)malloc(sizeof(pthread_t));
  if(pthread_create(thread, NULL, ReporterThread, (void *)a)) {
    return 0;
  }
  return thread;
}

//
void AnswerStart() {
  if(!started) {
    pthread_cond_signal(&floor_cond);
  }
  pthread_cond_wait(&floor_cond, &floor_mutex);

  printf("Speaker starts to answer question for reporter %d.\n", question_asker_id);
}

//
void AnswerDone() {
  printf("Speaker is done with answer for reporter %d.\n", question_asker_id);

  pthread_cond_signal(&floor_cond);
}

//sem_wait decrements sempahore integer
void EnterConferenceRoom(int id) {
  sem_wait(&conference_room_sem);
  printf("Reporter %d enters the conference room.\n", id);
}

//sem_post increments semaphore integer
void LeaveConferenceRoom(int id) {
  printf("Reporter %d leaves the conference room.\n", id);
  sem_post(&conference_room_sem);
}

//
void QuestionStart(int id) {
  pthread_mutex_lock(&speech_mutex);

  printf("Reporter %d asks a question.\n", id);
  question_asker_id = id;

  pthread_mutex_lock(&floor_mutex);
  pthread_cond_signal(&floor_cond);
  pthread_cond_wait(&floor_cond, &floor_mutex);
}

//
void QuestionDone(int id) {
  pthread_mutex_unlock(&floor_mutex);

  printf("Reporter %d is satisfied.\n", id);
  question_asker_id = -1;

  pthread_mutex_unlock(&speech_mutex);
}

//
void * SpeakerThread(void *args) {
  pthread_mutex_lock(&floor_mutex);
  while(1) {
    AnswerStart();
    AnswerDone();
  }
  return 0;
}

//
void * ReporterThread(void *args) {
  int id = *((int *)args);
  int questions = id % 4 + 2;

  EnterConferenceRoom(id);

  while(questions > 0) {
    QuestionStart(id);
    QuestionDone(id);

    questions--;
  }

  LeaveConferenceRoom(id);

  return 0;
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
