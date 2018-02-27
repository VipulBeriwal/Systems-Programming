#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
//getpid is the function of unistd
//sleep is the function of unistd
#include <sys/types.h>
#include <signal.h>
#include <string.h>

void Signal_Handler(int sig){
	char* Buffer = "\nMarry : I am Immortal! Better luck next time.\n";
	sleep(1);
	write(STDOUT_FILENO, Buffer, strlen(Buffer));
}


int main(int argc, char* argv[]){
	char* Buffer;
	//Welcome messages
	printf("Mary : I am Alive\n");
	printf("Mary : My pid is %d\n", getpid());
	printf("Mary : Hello World!\n");
	sleep(4);

	//Before Handling
	printf("\nMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM");
	printf("\nMary : I am MORTAL right now. (SIGINT is not handled)\n");
	printf("Mary : If you press Cntrl+c now , I will be killed silently and cannot produce my children Tom and Sandy.\n");
	
	int time = 15;
	while(time > 0){
		write(STDOUT_FILENO, ". ", strlen(". "));
		sleep(1);
		time--;
	}

	//Signal Handler Installed
	signal(SIGINT, Signal_Handler);
	Buffer = "\n\nMary: ***********__I am IMMORTAL now__**********. (SIGINT is handled). \nMary : If you press Cntrl+c now, I will not be killed!\n";
	write(STDOUT_FILENO, Buffer, strlen(Buffer));
	sleep(5);
	printf("\nMarry : I should expand now. I am creating my children.\n\n");
	sleep(10);
	

	//Created Tom
	pid_t Tom = fork();

	if (Tom > 0){
		//Created Sandy
		pid_t Sandy = fork();

		if (Sandy == 0){
			execv(argv[2], argv);
		}
		else{
			//Only Parent
			for(;;);
		}

	}
	else if (Tom == 0){
		sleep(1);
		execv(argv[1], argv);
	}
}
