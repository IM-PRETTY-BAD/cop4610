#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

//The reporter currently asking a question to the speaker.
int current_reporter=-1;
//Whether or not the news conference is in session.
int in_session = 0;
//The maximum number of reporters in the conference.
int max_reporters;

//Mutex used to prevent multiple reporters from asking questions at once.
pthread_mutex_t question_m;

//Semaphore used to limit the number of reporters that can be
//in the conference room at any given moment.
sem_t room_capacity;

//Mutex used to prevent the current reporter and the speaker from
//speaking out of turn.
pthread_mutex_t answer_m;

//A condition to ensure that the speaker will not begin answering
//questions until the conference is in session.
pthread_cond_t  session_c;

int Speaker();
void SpeakerThread();
int Reporter(int id);
void ReporterThread(void *args);
void AnswerStart();
void AnswerDone();
void EnterConferenceRoom(int id);
void LeaveConferenceRoom(int id);
void QuestionStart(int id);
void QuestionDone(int id);

int main(int argc, char *argv[]) {
 if(argc != 3) {
  printf("usage: %s number_of_reporters room_capacity \n", argv[0]);
  return 0;
 }
 int total_reporters=atoi(argv[1]);
 //Lock initialization.
 sem_init(&room_capacity, 0, atoi(argv[2]);
 pthread_mutex_init(&question_m, NULL);
 pthread_mutex_init(&answer_m, NULL);
 pthread_cond_init(&session_c, NULL);
 pthread_t reporters[total_reporters];
 //Lock answer mutex and set condition to block until it is unlocked.
 pthread_mutex_lock(&answer_m);
 Speaker();
 pthread_cond_wait(&session_c, &answer_m);
 //Begin the conference session.
 in_session = 1;
 pthread_mutex_unlock(&answer_m);
 int i;
 for(i=0; i<total_reporters; i++)
  reporters[i] = Reporter(i);
 for(i=0; i<total_reporters; i++)
  pthread_join(reporters[i], NULL);
 return 0;
}

int Speaker(){
pthread_t thread;
pthread_create(&thread, NULL, (void *)&SpeakerThread, NULL);
return thread;
}

void SpeakerThread(){
pthread_mutex_lock(&answer_m);
while(1){
AnswerStart();
AnswerDone();
}
}

int Reporter(int id){
pthread_t thread;
pthread_create(&thread, NULL, (void *)&ReporterThread, (void *)id);
return thread;
}

void ReporterThread(void *args){
int id = (int)args;
int questions = id % 4 + 2;
EnterConferenceRoom(id);
while(questions > 0) {
QuestionStart(id);
QuestionDone(id);
questions--;
}
LeaveConferenceRoom(id);
}

void AnswerStart(){
if(!in_session)
pthread_cond_signal(&session_c);
//The speaker does not begin answering until the conference is in
//session and it is their turn to speak.
pthread_cond_wait(&session_c, &answer_m);
printf("Speaker starts to answer question for reporter %d.\n",
current_reporter);
}

void AnswerDone(){
printf("Speaker is done with answer for reporter %d.\n", current_reporter);
pthread_cond_signal(&session_c);
}

void EnterConferenceRoom(int id){
//Block if the room is at max capacity.
sem_wait(&room_capacity);
printf("Reporter %d enters the conference room.\n", id);
}

void LeaveConferenceRoom(int id){
printf("Reporter %d leaves the conference room.\n", id);
sem_post(&room_capacity);
//Free up a room slot.
}

void QuestionStart(int id){
//Lock to ensure only the given reporter can ask a question at a time.
pthread_mutex_lock(&question_m);
printf("Reporter %d asks a question.\n", id);
current_reporter = id;
//Lock to ensure the speaker only answers the current reporter.
pthread_mutex_lock(&answer_m);
pthread_cond_signal(&session_c);
pthread_cond_wait(&session_c, &answer_m);
}

void QuestionDone(int id){
pthread_mutex_unlock(&answer_m);
printf("Reporter %d is satisfied.\n", id);
current_reporter = -1;
pthread_mutex_unlock(&question_m);
}