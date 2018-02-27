#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	int Pipe_FD[2];					//Pipe fd
	pipe(Pipe_FD); 					//Creatinf new pipe
	pid_t Mapper_PID = fork();			//Creating new child process as a "mapper"

	if (Mapper_PID == 0){				//For child-mapper
		close(Pipe_FD[0]);			//As child-mapper read from file, closing reading end of child-mapper
		dup2(Pipe_FD[1], 1);			//Redirecting standard output of child-mapper to write end of pipe for reducer-child to read
		execvp(argv[1], argv);			//Running mapper
	}
	else{
		close(Pipe_FD[1]);			//This will true for parent. As child-mapper uses write end of pipe. Parent should be disconnected from it
		pid_t Reducer_PID = fork();		//This will reate another child that is reducer child
		if (Reducer_PID == 0){			//It will be run in Reducer only
			close(Pipe_FD[1]);		//As Reducer does not write in pipe. Closing write pipe end for the user
			dup2(Pipe_FD[0], 0);		//Redirecting reducer to read from the pipe as compared to standard input
			execvp(argv[2], argv);		//Running Reducer
		}
		else{
			close(Pipe_FD[0]);		//Only in parent, as reducer reads from read end of the pipe. Parent should be disconnected from this end too
		}
	}
	
	return 0;
}
