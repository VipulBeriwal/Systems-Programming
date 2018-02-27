#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>


void Signal_Handler(int sig){
	char* Buffer = "\nTom : I am Immortal! Better luck next time.\n";
	sleep(1);
	write(STDOUT_FILENO, Buffer, strlen(Buffer));
}



int main(){
	//Welcome messages
	char *Buffer;
	printf("$$$$$$$$$__My second child is alive__$$$$$$$$$$\n");
	printf("Tom   : %d\n", getpid());
	printf("Tom   : Hello World\n");
	sleep(2);


	//Before Handling
	printf("\nTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT\n");
	printf("Tom : I am MORTAL right now. (SIGINT is not handled)\n");
	printf("Tom : If you press Cntrl+c, I will be killed silently. But my parent Mary and my sibling Sandy will not be killed.\n");
	Buffer = "Tom : Sandy is rightnow ignoring the SIGINT and no matter if you kill me or not, Sandy will definitely see you after sometime!\n";
	write(STDOUT_FILENO, Buffer, strlen(Buffer));
	sleep(15);
	int time = 15;
	while(time > 0){
		write(STDOUT_FILENO, ". ", strlen(". "));
		sleep(1);
		time--;
	}

	//handling Interrpt
	signal(SIGINT, Signal_Handler);
	Buffer = "\n\nTom: **********__I am IMMORTAL now)__**********. (SIGINT is handled. \nTom : If you press Cntrl+c now, I will not be killed! Sandy is coming!\n";
	write(STDOUT_FILENO, Buffer, strlen(Buffer));
	sleep(5);
	

	for(;;);
}
