#include <stdlib.h>
#include <stdio.h>
#include <string.h>


//Tuple Structure
struct Tuple{
	char userID[4];
	int score;
	char Topic[15];
};

//Linked List structure
struct  Tuple_Node{
	struct Tuple data;
	struct Tuple_Node* next; 
};

// Always returns HEAD of Linked List
struct Tuple_Node* Insert( struct Tuple_Node *HEAD, struct Tuple triplet){
	struct Tuple_Node *ptr , *prev;
	prev = HEAD;
	ptr = HEAD;
	if (ptr == NULL){
		struct Tuple_Node* node = (struct Tuple_Node*)malloc(sizeof(struct Tuple_Node));			//Runs everytime when new userID comes
		node->data = triplet;											//Linked list first node
		node->next = NULL;
		HEAD = node;
	}
	else{														//Pointer traverses to check for same Topic 
		while(ptr != NULL){
			if ( !strcmp(ptr->data.Topic, triplet.Topic)) {
				ptr->data.score += triplet.score;
				break;											//If found, adds score
			}
			else{  	
				prev = ptr;
				ptr = ptr->next;
			}
		}
		if(ptr == NULL){											//If not, make new node
			struct Tuple_Node* node = (struct Tuple_Node*)malloc(sizeof(struct Tuple_Node));
			node->data = triplet;
			node->next = NULL;
			prev->next = node;
		}
	}
	return  HEAD;
}

//Print the Linked List
void Display(struct Tuple_Node *HEAD){
	while(HEAD != NULL){
		printf("("); 
		for(int i = 0; i <4; i++) { printf("%c", HEAD->data.userID[i]); }
		printf(",");
		for(int i = 0; i<15; i++ ) {printf("%c", HEAD->data.Topic[i]) ; }
		printf(",");
		printf("%d", HEAD->data.score); 
		printf(")\n");
		HEAD = HEAD->next;
	}
}

//Free Unused Linked List Deallocation of Nodes
void Flush(struct Tuple_Node *HEAD){
	struct Tuple_Node *forward = HEAD; 
	while(forward != NULL){
		forward = HEAD->

		next;
		free(HEAD);
		HEAD = forward;
	}
}

//Parsing function to convert string into Tuple structure
struct Tuple Parse(char string[]){
	struct Tuple node;
	for(int i = 0; i <4; i++) {node.userID[i] = string[i+1]; }
	for(int i = 0; i <15; i++) { node.Topic[i] = ' '; }
	int i = 6;
	while(string[i] != ','){
		node.Topic[i-6] = string[i]; i++;
	}
	int j = i+1;
	char s[20];
	while(string[j] != ')'){
		s[j-i-1] = string[j]; j++;
	}
	node.score = atoi(s);
	return node;
}


int main(){
	char character = '0';
	char string_buffer[100];  
	for(int i = 0; i < 100; i++) { string_buffer[i] = '0'; } 		
	char prev_userID[4];
	
	printf("\n");
	struct Tuple_Node *HEAD = NULL;
	fgets(string_buffer, 100, stdin);						//Reading tuple and storing in string_buffer
	for(int i = 0; i < 4; i++) { prev_userID[i] = string_buffer[i+1]; } 	//Updating prev ID first time
	struct Tuple node = Parse(string_buffer);				//String is parsed and returned as node Tuple structure
	HEAD = Insert(HEAD, node);						//Node is inserted in the linked List
	character = string_buffer[0];						//Checks for \n character as first character of Line

	while(character != '\n'){
		fgets(string_buffer, 100, stdin);
		struct Tuple node = Parse(string_buffer);
		if ((string_buffer[1] == prev_userID[0]) && (string_buffer[2] == prev_userID[1]) && (string_buffer[3] == prev_userID[2])  && (string_buffer[4] == prev_userID[3])){
			HEAD = Insert(HEAD, node);				//If same ID then Insert Node
		}
		else{
			Display(HEAD);						//Else, print linked List
			Flush(HEAD);						//Flushed
			HEAD = NULL;
			for(int i = 0; i < 4; i++) { prev_userID[i] = string_buffer[i+1]; } // UserID is updated
			HEAD = Insert(HEAD, node);				//That node is treated as first node of Linked list and inserted
		}
		character = string_buffer[0];					//Cheks first character as \n
	}
	return 0;                                                                                                                                 
}

