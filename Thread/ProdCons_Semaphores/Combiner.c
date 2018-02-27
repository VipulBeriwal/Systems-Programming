#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <semaphore.h>

/**************************************** DATATYPE/FUNCTIONs DECLARATIONS****************************************/
/********************************************************************************************************************/
/********************************************************************************************************************/
int BUFFER_SIZE ; 		
int THREAD_COUNT;		
int* CURRENT_INDEX;			//Saves the current no of elements in buffer of thread - 1 (-1 is for indexing purpose)
char*** BUFF_ADD_TABLE;		//Stores the addresses of respective buffers
int* USERID_TO_THREADID;		//Array stores whihc userID maps to whihc thread_ID
int FLAG_COUNT = 0;			//Ensures that when threads starts all of them puts theri buffer addressess in table and then mapper accesses table
int END = -1;				//Global variable whihc updates when a new UserID comes
int done = 0;				//Done signeal when mapper finds EOF

pthread_mutex_t lock;
pthread_cond_t full;
pthread_cond_t empty;


//Semaphore initialization
sem_t sem_lock, sem_empty, sem_full;
//for sem_lock
unsigned int binary_counter = 1;
//for empty and full
unsigned int empty_counter = 0;
unsigned int full_counter = 0;
int pshared = 0;



//Tuple Structure Declaration
struct Tuple{
	char userID[4];
	int score;
	char Topic[15];
};

//Linked list(REDUCER)
struct  Tuple_Node{
	struct Tuple data;
	struct Tuple_Node* next; 
};

//To convert int to char*(COMBINER)
char* tostring(int num);

//Includes all starting initializations before making threads(COMBINER)
void initializations(int a, int b);

//Takes character array and convert it into Tuple(MAPPER)
struct Tuple  Convert_Tuple(char string[]);

//Converts Tupe to char*(COMBINER)
char* String_Converter(struct Tuple node);

//Insert in a linked list(REDUCER)
struct Tuple_Node* Insert( struct Tuple_Node *HEAD, struct Tuple triplet);

//Reducer prints output string(REDUCER)
void Print_to_Terminal(struct Tuple_Node *HEAD);

//Parsing string and converting to Tuple(REDUCER)
struct Tuple Parse(char* str);

//MAPS which USERID of TUPLE belongs to whihc thread number(COMBINER)
int thread_userID_map(int user_ID);

//For producer thread(main)(COMBINER)
void insert_in_buffer(int thread_index, char *str, int add_count_variable);

//For consumer thread(COMBINER)
char* fetch_from_buffer(int thread_index, int count);

/************************************************FUNCTION INCLUDES CRITICAL SECTION*******************************/
//Utility function for producer(COMBINER) 
void mapper();

/************************************************FUNCTION INCLUDES CRITICAL SECTION*******************************/
//Consumers (COMBINER)
void *Thread_Utility( void* user_index);

//Flushes the linked list
void Flush(struct Tuple_Node *HEAD);




/**********************************************MAIN FUNCTION(PRODUCER)*******************************************/
/********************************************************************************************************************/
/********************************************************************************************************************/
//Producer
int main(int argc, char *argv[]){
	//Initializations
	initializations(atoi(argv[1]), atoi(argv[2]));
	
	//making an array of data type pthread_t
	pthread_t thread_id[THREAD_COUNT];

	//initialization of semaphores
	int lock_return = sem_init(&sem_lock, pshared, binary_counter);
	int empty_return = sem_init(&sem_empty, pshared, empty_counter);
	int full_return = sem_init(&sem_full, pshared, full_counter);

	//creating threads
	for(int user_index = 0; user_index < THREAD_COUNT; user_index++){
		//user_index in index for our understanding
		//pthread_t is the index for kernel understanding
		pthread_create(&thread_id[user_index], NULL, Thread_Utility, (void*)user_index);
	}
	//waiting untill all threads put their respective buffer address in address table so that thread will not access table before initialization
	while(1){
		if (FLAG_COUNT == THREAD_COUNT){
			break;
		}
	}
	//functionality of mapper
	mapper();
	pthread_exit(NULL);
}



/***********************************************FUNCTION DEFINITIONS***********************************************/
/********************************************************************************************************************/
/********************************************************************************************************************/
char* tostring(int num){
    int i, rem, len = 0, n;
    char str[10];
    n = num;
    char* s= (char*)malloc(sizeof(char)*10);
    char* p = s;

    if (num < 0){
    	num = num*(-1);
    	*s = '-';
    	s++;
    }

    while (n != 0)
    {
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';
  
    for(int i = 0;  i <len ; i++){
      *s = str[i];
      s++; 
    }
    *s = '\n';
    s++;	
    return p;
}

void initializations(int a, int b){
	BUFFER_SIZE = a;
	THREAD_COUNT = b;
	BUFF_ADD_TABLE = (char ***)malloc( sizeof(char**)*THREAD_COUNT);
	CURRENT_INDEX = (int*)malloc(sizeof(int)*THREAD_COUNT);
	USERID_TO_THREADID = (int*)malloc(sizeof(int)*THREAD_COUNT);
	//putting -1 in userid table before putting actual userid
	for(int k = 0; k < THREAD_COUNT; k++){
		*USERID_TO_THREADID = -1;
		USERID_TO_THREADID++;
	}
}

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

char* String_Converter(struct Tuple node){
	char *str = (char*)malloc(sizeof(char)*24);
	char *start = str;
	*str = '(';
	str++;
	for(int i = 0; i < 4; i++){ 
		*str = node.userID[i];
		str++;
	}
	*str = ',';
	str++;
	for(int i = 0; i < 15; i++){ 
		*str = node.Topic[i];
		str++;
	}
	*str = ',';
	str++;
	char* s = tostring(node.score);
	while( *s != '\n'){
		*str = *s;
		str++;
		s++;
	}
	*str = ')';
	return start;
}

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

void Print_to_Terminal(struct Tuple_Node *HEAD){
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

struct Tuple Parse(char* str){
	struct Tuple node;
	str++;
	for(int i = 0; i <4; i++) {
		node.userID[i] = *str;
		str++;
	}
	for(int i = 0; i <15; i++) { node.Topic[i] = ' '; }
	int i = 6;
	str++;
	while( *str != ','){
		node.Topic[i-6] = *str; str++; i++;
	}
	int j = i+1;
	str++;
	char s[20];
	while(*str != ')'){
		s[j-i-1] = *str; str++; j++;
	}
	node.score = atoi(s);
	return node;
}

int thread_userID_map(int user_ID){
	int i = 0;
	int thread_number = -1;
	int *needle = USERID_TO_THREADID;
	for( i = 0; i <= END; i++){
		if (*needle == user_ID) { return i; }
		else{ *needle++; }
	}
	if (thread_number == -1){
		END ++;
		*needle = user_ID;
		return END;
	}
}

void insert_in_buffer(int thread_index, char *str, int add_count_variable){
	char** my_buffer_address = BUFF_ADD_TABLE[thread_index];
	//putting string at that index of buffer
	my_buffer_address[add_count_variable] = str;
}

char* fetch_from_buffer(int thread_index, int count){
	char** my_buffer_address = BUFF_ADD_TABLE[thread_index];
	return my_buffer_address[count];
}

void mapper(){
	//Mapper is taking input from standard input
	FILE* fptr = stdin;						//File object pointer 
	if (fptr == NULL){						//Checking for file
		printf("Cannot open file");
		exit(0);
	}
	char character = fgetc(fptr);					//Reads each character of file and increments the pointer internally
	
	while (1){	
		//CRITICAL SECTION STARTS : Mapper is editing on "done" and other threads are reading this value "done"
		//*************************************************************************************************

		//pthread_mutex_lock(&lock);
		sem_wait(&sem_lock);
		if((character == '\n') || (character == EOF)){
			done = 1;
			sem_post(&sem_empty);
			sem_post(&sem_lock);
			break;
		}
		sem_post(&sem_lock);
		//CRITICAL SECTION ENDS
		//*************************************************************************************************
		
		//USUAL WORK: Usual Work of Mapper(NO LOCK)
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
		//conersion of user id
		char user_id[4];
		for(int m = 0; m < 4; m++){
			user_id[m] = node.userID[m];
		}
		int user_ID = atoi(user_id);
		char *s = String_Converter(node);

		//CRITICAL SECTION STARTS
		//*************************************************************************************************
		//pthread_mutex_lock(&lock);
		sem_wait(&sem_lock);
		int thread_number = thread_userID_map(user_ID);
		//fetching the number of entries in buffer(actually gives index, if entry = 1 then it gives 0)
		int *needle = &CURRENT_INDEX[thread_number];
		//buffer is full
		while( *needle == BUFFER_SIZE - 1 ){
			sem_post(&sem_lock);
			//prevents reducer to wait for empty
			sem_post(&sem_empty);
			sem_wait(&sem_full);
			sem_wait(&sem_lock);
		}
		//before inserting incrementing index, as previously in starting -1(no entry)
		*needle = *needle + 1;
		//we have to pass by pointer to get an update
		insert_in_buffer(thread_number, s, *needle);
		sem_post(&sem_lock);
		sem_post(&sem_empty);
    		//CRITICAL SECTION ENDS
		//*************************************************************************************************

		//USUAL WORK
		character = fgetc(fptr);  				// As the while loop read untill ')' it increments pointer 
		character = fgetc(fptr); 					// checks for the EOF
	}
}

void *Thread_Utility( void* user_index){
	//casting the user id into int type
	int thread_index = (int)user_index;
	setbuf(stdout, NULL);
	
	
	//CRITICAL SECTION STARTS
	//*************************************************************************************************
	sem_wait(&sem_lock);
	char** mybuffer = (char**)malloc( sizeof(char *)*BUFFER_SIZE);
	int x = thread_index;
	char*** temp = BUFF_ADD_TABLE;
	while (x != 0){
		temp++;
		x--;
	}
	//storing address
	*temp = mybuffer;
	int* pointer = CURRENT_INDEX;
	int y = thread_index;
	while(y != 0){
		pointer++;
		y --;
	}
	*pointer = -1;
	//Each thread will increase FLAG_COUNT untill reaches total count
	FLAG_COUNT ++;
	sem_post(&sem_lock);
	//CRITICAL SECTIONS ENDS
	//*************************************************************************************************

	//linked list initializations
	struct Tuple_Node *HEAD = NULL;

	
	//reading count
	int *needle = &CURRENT_INDEX[thread_index];
	
	while(1){
		//CRITICAL SECTION STARTS
		//*************************************************************************************************			
		sem_wait(&sem_lock);
		while( (*needle == -1 ) && (!done) ){
			sem_post(&sem_lock);
			sem_post(&sem_full);
			sem_wait(&sem_empty);
			sem_wait(&sem_lock);
		}

		if( (*needle == -1 ) && (done) ){
			Print_to_Terminal(HEAD);
			//up leave empty
			sem_post(&sem_empty);
			sem_post(&sem_lock);
			break;
		}
		//now i have to come out of waiting list of sem_empty
		//We can take out this from buffer
		char* str = fetch_from_buffer(thread_index, *needle);
		//After fetching  reducing the count of that buffer 
		*needle = *needle -1;
		struct Tuple node = Parse(str);
		//Sending string in the linked list
		HEAD = Insert(HEAD, node);
		sem_post(&sem_lock);
		sem_post(&sem_full);
    		//CRITICAL SECTION ENDS
    		//*************************************************************************************************
	}
	pthread_exit(NULL);
}
