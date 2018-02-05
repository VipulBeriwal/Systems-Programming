#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

/*This program creates n threads given in the input terminal and print their respective ids. One can see the out of order
    execution of different threads in the program */

//Ignore the warnings, they are beacuse of casting int to void pointer

//Usage : compile gcc -pthread create_threads.c -o create_threads
// Usage run: ./create_threads n

void *Display( void* user_index){
	//casting the user id into int type
	int thread_index = (int)user_index;
	//taking care of local buffer of the printf
	setbuf(stdout, NULL);
	printf("Hi, I am thread %d\n", thread_index);
	pthread_exit(NULL);
}

int main(int argc, char *argv[]){
	//making an array of data type pthread_t
	pthread_t thread_id[atoi(argv[1])];
	//creating threads
	for(int user_index = 1; user_index <= 5; user_index++){
		//user_index in index for our understanding
		//pthread_t is the index for kernel understanding
		pthread_create(&thread_id[user_index], NULL, Display, (void*)user_index);
	}
	pthread_exit(NULL);
}