#include <stdio.h>
#include <stdlib.h>

//Tuple Structure Declaration
struct Tuple{
	char userID[4];
	int score;
	char Topic[15];
};

//Reading string from the file and converting into the tuple structure
struct Tuple  Convert_Tuple(char string[]){
	struct Tuple node;
	//for userID
	for(int i = 0; i < 4; i++){
		node.userID[i] = string[i+1];
	}
	//for score
	switch(string[6]){
		case 'P' : {node.score = 50; break;}
		case 'L' : {node.score = 20; break; }
		case 'D': {node.score = -10; break;}
		case 'C': {node.score = 30; break;}
		case 'S' : {node.score = 40; break;}
	}
	//for Topic
	for(int i = 0; i < 15; i++){ node.Topic[i] = ' '; }
	int i = 8;
	while(string[i] != ')'){
		node.Topic[i-8] = string[i];
		i++;
	}
	return node;
}

//To print
void Display(struct Tuple node){
	printf("(");
	for(int i = 0; i < 4; i++){ printf("%c", node.userID[i]); }
	printf(",");
	for(int i = 0; i < 15; i++){ printf("%c", node.Topic[i]); }
	printf(",");
	printf("%d", node.score);
	printf(")\n");
}


int main(){
	FILE* fptr = fopen("input.txt", "r");				//File object pointer 
	if (fptr == NULL){						//Checking for file
		printf("Cannot open file");
		exit(0);
	}
	char character = fgetc(fptr);					//Reads each character of file and increments the pointer internally
	while (character != EOF ){					//Picks the 24 characters
		char string[24];
		int i = 0;
		for(int i = 0; i <24; i++) { string[i] = ' '; }
		while( character != ')' && character != EOF ){
			string[i] = character;
			character = fgetc(fptr);
			i++;
		}
		string[i] = character;					//making last character of string ')'
		struct Tuple node = Convert_Tuple(string);
		Display(node);
		character = fgetc(fptr);  				// As the while loop read untill ')' it increments pointer 
		character = fgetc(fptr); 					// checks for the EOF
	}
	printf("\n");							// printf so that reducer will detect end of the file								
	return 0;
}
