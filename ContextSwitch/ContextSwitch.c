#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <time.h>


pthread_mutex_t *lock;                                  //for shared lock
pthread_cond_t *cond_parent, *cond_child;  //Two condition variables both can access of both of theirs
pthread_mutexattr_t attr_lock;		      //Attributes of lock to make it shared
pthread_condattr_t attr_parent, attr_child;   //Attributes of cond variables to make it shared


int *Buffer_parent, *Buffer_child, *count, *iterations, *sum;
//Buffer_parent and Buffer_child is the place where their tokens are saved

void initialization(){
	//MMapping
	lock = (pthread_mutex_t*)mmap(NULL, sizeof(pthread_mutex_t), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
	cond_parent = (pthread_cond_t*)mmap(NULL, sizeof(pthread_cond_t), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
	cond_child = (pthread_cond_t*)mmap(NULL, sizeof(pthread_cond_t), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
	Buffer_parent = (int*)mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
	Buffer_child = (int*)mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
	count = (int*)mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
	iterations = (int*)mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
	sum = (int*)mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
	
	//Giving virtual token to Parent initially
	*Buffer_parent = 1;
	*Buffer_child = 0;
	*count = 0;
	//This value can be adjusted for the average
	*iterations = 100000;
	//A shared variable whihc we give the average 
	*sum = 0;

	//Making attribtes shared
	pthread_mutexattr_setpshared(&attr_lock, PTHREAD_PROCESS_SHARED);
	pthread_condattr_setpshared(&attr_parent, PTHREAD_PROCESS_SHARED);
	pthread_condattr_setpshared(&attr_child, PTHREAD_PROCESS_SHARED);

	//Initializations for sync variables
	pthread_mutex_init(lock, &attr_lock);
	pthread_cond_init(cond_parent, &attr_parent);
	pthread_cond_init(cond_child, &attr_child);	
}

int main(){
	//initializing
	initialization();
	//Creating child
	pid_t pid = fork();

	if (pid == 0){
		//Child
		while(1){
			clock_t start, end;

			pthread_mutex_lock(lock);
			while(*Buffer_child == 0 ){
				pthread_cond_wait(cond_child, lock);
			}
			
			//Timestamp for recieving token by the parent
			start = clock(); 

			if (*count == (*iterations -1)){
				(*count)++;
				*Buffer_parent = 1;
				pthread_mutex_unlock(lock);
				pthread_cond_signal(cond_parent);
				exit(0);
			}

			//Sends the token Again to Parent
			*Buffer_child = 0;
			*Buffer_parent = 1;
			(*count)++;
			pthread_mutex_unlock(lock);
			pthread_cond_signal(cond_parent);
			//Timestamp for time taken till token sent and time taken by signal to make parent alive.
			end = clock();
			//child will keep on adding its timestamps
			*sum += ((end - start)*1000000000)/CLOCKS_PER_SEC;
			printf("Child time difference  %ld\n\n", ((end - start)*1000000000)/CLOCKS_PER_SEC);
		}
	}
	else{
		//Parent
		//Sum of parents timestamps for the average
		int parent_sum = 0;
		while(1){
			clock_t start, end;

			pthread_mutex_lock(lock);
			//Token delivery starts
			*Buffer_parent = 0;
			*Buffer_child = 1;
			(*count)++;
			pthread_cond_signal(cond_child);
			//PArent has sent the token to child. Child is waiting to acquire lock

			while(*Buffer_parent == 0 ){
				//Timestamp at which parent goes to waiting state.
				start = clock();  
				pthread_cond_wait(cond_parent, lock);  
				//When it comes here, context switches happen as parent blocks. Child acquires the lock
			}
			//Parent is scheduled again and recives the token back
			end = clock();
			pthread_mutex_unlock(lock);
	 		
	 		parent_sum += ((end - start)*1000000000)/CLOCKS_PER_SEC;
			printf("Parent time difference  %ld\n\n", ((end - start)*1000000000)/CLOCKS_PER_SEC);
			
			//Last condition check to exit 
			pthread_mutex_lock(lock);
			if (*count  == *iterations){
				pthread_mutex_unlock(lock);
				//Taking average of the total sum.
				//Timestamp = sending and recieving token back again, therefore context switches happens twice for A so division by 2
				//Last time stamp of parent = ((end - start)*1000000)/CLOCKS_PER_SEC)
				//Taking average by dividing with number of iterations
				//Parent adds one last extra difference to total sum, therefore subtracting form the equation the last timestamp
				*sum = (parent_sum - *sum - (((end - start)*1000000000)/CLOCKS_PER_SEC)) / (*iterations*2);
				printf("The average time of context switch is %d microseconds\n", *sum);
				exit(0);
			}
			pthread_mutex_unlock(lock);
		}
	}
}
