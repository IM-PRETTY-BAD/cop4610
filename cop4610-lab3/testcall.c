#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

#define __NR_get_slob_amt_claimed 343
#define __NR_get_slob_amt_free 344

#define MAXN	10000
int main(void){
	char *buffer[MAXN];
	int i = 0;
	for(; i < MAXN; ++i){
		//sigma from 0 to 200 n+50 = 30150
		//10 000/200 = 50
		// 50*30 150 = 1 507 500
		//buffer[i] = (char *)malloc((i%200)+50);
		buffer[i] = (char *)malloc(((MAXN-(i+1))%200)+50);
	}


	for(i=0 ; i < MAXN; i+=2){
		free(buffer[i]);
	}
	
	for(i=0 ; i < MAXN; i+=2){
		buffer[i] = (char *)malloc((i%200)+50);
	}

	printf("Amount claimed: %lu bytes\n", syscall(__NR_get_slob_amt_claimed));
	printf("Amount free: %lu bytes\n", syscall(__NR_get_slob_amt_free));
	
	for(i=0 ; i < MAXN; ++i){
		free(buffer[i]);
	}

	return 0;
}
