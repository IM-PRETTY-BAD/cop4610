#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

#define __NR_get_slob_amt_claimed 343
#define __NR_get_slob_amt_free 344

#define MAXN  1
int main(void){
  char *buffer[MAXN];
  int i=0;
  for(; i<MAXN; ++i){
    buffer[i]=(char*)malloc((i%200)+50);
  }
  printf("Amount claimed: %lu bytes\n", syscall(__NR_get_slob_amt_claimed);
  printf("Amount free: %lu bytes\n", syscall(__NR_get_slob_amt_free));
  for(i=0; i<MAXN; ++i){
    free(buffer[i]);
  }
}
