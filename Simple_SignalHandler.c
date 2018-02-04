#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

/*This program shows how to handle interrupt and change the default nature of the signal. If not handled
   the signal sigint terminates the program but after handling it is customized to perform specific task! */
 
//It will run interrupt, making the main program to be paused
//Main program will start executing again when it comes from interrupt
void Signal_Handler(int sig){
	printf("\nAhh!!!....I am an interrpt!!\nChange in the environment. Running the interrpt routine... \n");
	for(int i = 0; i < 5; i++) {
		//use printf to check the unpredictable behaviour of buffer
		//printf(". ");
		write(STDOUT_FILENO, ". ", strlen(". "));
		sleep(1);
	}
	printf("\nExecution completes. Going back to main... \n");
}


int main(){
	//if ctrl+c comes before handling the signal it terminates
	printf("Press 'Ctrl + c' before 10 seconds for not to handle the SIGINT.\n");
	for(int j = 1; j <=10; j++){
		printf("%d seconds\n", j);
		sleep(1);
	}

	printf("Signal is ready to be handled\n");
	//installing the signal handler and after this point if interrupt comes, it will be handled
	signal(SIGINT, Signal_Handler);

	//running the loop infinitely so that when you press (ctrl + c) it calls interrpt and then return to main program 
	for(int i = 1; ; i ++){
		printf("%d seconds\n", i);
		sleep(1);
	}
	return 0;
}
