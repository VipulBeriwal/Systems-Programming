#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>


void Signal_Handler(int sig){
	char* Buffer = "\nSandy : I am Immortal! Better luck next time.\n";
	sleep(1);
	write(STDOUT_FILENO, Buffer, strlen(Buffer));
}


int main(){
	//Welcome messages
	printf("$$$$$$$$$__My first child is alive__$$$$$$$$$$\n");
	printf("Sandy : %d\n", getpid());
	printf("Sandy : Hello World\n\n");

	signal(SIGINT, SIG_IGN);
	sleep(37);
	signal(SIGINT, SIG_DFL);
	
	//Before Handling
	sleep(2);
	printf("\nSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS");
	printf("\nSandy : Yes!! I am back. \nSandy : I am MORTAL right now. (SIGINT is not handled)\n");
	printf("Sandy : If you press Cntrl+c, I will be killed silently\n");
	int time = 15;
	while(time > 0){
		write(STDOUT_FILENO, ". ", strlen(". "));
		sleep(1);
		time--;
	}
	
	//handling Interrpt
	signal(SIGINT, Signal_Handler);
	char* Buffer = "\n\nSandy: **********__I am IMMORTAL now__**********. (SIGINT is handled). \nSandy : If you press Cntrl+c now, I will not be killed!\n\n";
	write(STDOUT_FILENO, Buffer, strlen(Buffer));

	for(;;);
}