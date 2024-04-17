#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "io.h"

//command IO
#define MAX_INPUT_LENGTH 4096
char input[MAX_INPUT_LENGTH];
char *tok1;
char *tok2;
char *tok3;
char *tok4;

const char *delim = " "; 

int sanitize(char *str) {
    int count=0;
    while (*str != '\0') {
        if (*str == '\n' || *str == '\r') {
            *str = '\0';  // Replace with null terminator
            
            if (*str == '\n'){
            	count++;
            	if(count>1){
            		return 1;
            	}
            }
        }
        str++;
    }

    return 0;
}

//internal structure
struct UserHeap{
	Heap h;
	bool sorted;
	char name[];
};

struct HeapList{
	struct HeapList* next;
	struct UserHeap self;
};

struct HeapList* Heaps=NULL; 

void show_state(){
	struct HeapList* head=Heaps;

	printf("current heaps: ");
	while(head){
		printf("<%s> ",head->self.name);
		head=head->next;
	}
	printf("\n");
}

void addHeap(Heap h,bool sorted,char* name){
	size_t l=strlen(name)+1;
	struct HeapList* to_add=malloc(sizeof(struct HeapList)+l);
	if(to_add==NULL){
		printf("ran out of memory\n");
		exit(1);
	}
	to_add->self.h=h;
	to_add->self.sorted=sorted;
	memcpy(to_add->self.name,name,l);

	to_add->next=Heaps;
	Heaps=to_add;
}


struct UserHeap* get_heap(char* name){
	struct HeapList* head=Heaps;
	while(head!=NULL){
		if(strcmp(head->self.name,name)==0){
			return &head->self;
		}
		head=head->next;
	}
	return NULL;
}

int remove_heap(char* name){
	if(Heaps==NULL){
		return 1;
	}

	if(strcmp(Heaps->self.name,name)==0){
		struct HeapList* temp=Heaps->next;
		free(Heaps);
		Heaps=temp;
		return 0;
	}

	struct HeapList* prev=Heaps;
	struct HeapList* tail=Heaps->next;

	while(tail!=NULL){
		if(strcmp(tail->self.name,name)==0){
			prev->next=tail->next;
			free(tail);
			return 0;
		}
		prev=tail;
		tail=tail->next;
	}
	return 1;
}

int main() {
	while(1){
		show_state();
		printf("Enter command: \n");
    
	    if (fgets(input, MAX_INPUT_LENGTH, stdin) == NULL) {
	        printf("Error reading input.\n");
	        continue;
	    }
	    
	    if(sanitize(input)){
	    	printf("newlines are not alowed.\n");
	    	continue;
	    }

	    // Get the first token (command)
	    tok1 = strtok(input, delim);

	    if (tok1 == NULL) {
	        printf("empty string...\n");
	        continue;
	    }

	    //printf("\nDebug: tok1 = '%s'\n", tok1);

	    if(strcmp(tok1,"show")==0){//prints heap
	    	tok2 = strtok(NULL, delim); //name
	    	if(tok2==NULL){
	    		goto badargs;
	    	}

	    	struct UserHeap* uh=get_heap(tok2);
	    	if(uh==NULL){
	    		printf("heap dosent exists\n");
	    		continue;
	    	}

	    	if(uh->sorted){
	    		printf("[sorted]\n");
	    	}
	    	else{
	    		printf("[not sorted]\n");
	    	}
	    	print_heap(uh->h);
	    }

	    else if(strcmp(tok1,"load")==0){//loads file into heap
	    
	    }

	    else if(strcmp(tok1,"dump")==0){//dumpts a heap

	    }

	    else if(strcmp(tok1,"make")==0){//makes an empty heap
	    	tok2 = strtok(NULL, delim); //name
	    	tok3 = strtok(NULL, delim); //sorted

	    	if(tok2==NULL){
	    		goto badargs;
	    	}

	    	if(get_heap(tok2)!=NULL){
	    		printf("heap by that name already exists\n");
	    		continue;
	    	}

	    	int sorted;
	    	if(tok3==NULL){
	    		sorted=1;
	    	}

	    	else{
	    		if (strcmp(tok3, "1") == 0 || strcmp(tok3, "y") == 0 || strcmp(tok3, "yes") == 0) {
	    		sorted=1;
		    	}

		    	else if (strcmp(tok3, "0") == 0 || strcmp(tok3, "n") == 0 || strcmp(tok3, "no") == 0){
		    	 	sorted=0;
		    	}

		    	else{
		    		goto badargs;
		    	}
	    	}
	    	
	    	addHeap(NULL,sorted,tok2);
	    }

	    else if(strcmp(tok1,"remove")==0){//makes an empty heap
	    	tok2 = strtok(NULL, delim); //name
	    	if(tok2==NULL){
	    		goto badargs;
	    	}
	    	//printf("\nDebug: tok2 = '%s'\n", tok2);

	    	if(remove_heap(tok2)){
	    		printf("no such Heap\n");
	    	}

	    }

	    else if(strcmp(tok1,"sort")==0){//sorts a heap and changes its type to sorted

	    }

	    else if(strcmp(tok1,"to_unsort")==0){//changes the type to sorted

	    }

	    else if(strcmp(tok1,"merge")==0){//merges two heaps of the same type (if not same type errors)

	    }

	    else if(strcmp(tok1,"clone")==0){//clones a heap naming the clone a user provided thing

	    }

	    else if(strcmp(tok1,"insert")==0){//inserts to a heap (if sorted keeps it sorted)
	    	tok2 = strtok(NULL, delim); //name
	    	tok3 = strtok(NULL, delim); //rank
	    	tok4 = strtok(NULL, delim); //data

	    	if(tok2==NULL || tok3==NULL || tok4==NULL){
	    		goto badargs;
	    	}

	    	rank_t rank;
	    	if(parse_int(tok3,&rank)){
	    		printf("3d not an int\n");
	    		goto badargs;
	    	}

	    	struct UserHeap* uh=get_heap(tok2);
	    	if(uh==NULL){
	    		printf("heap dosent exists\n");
	    		continue;
	    	}

	    	if(uh->sorted){
	    		if(ordered_insert(&uh->h,rank,tok4)){goto memory_error;};
	    	}
	    	else{
	    		if(unordered_insert(&uh->h,rank,tok4)){goto memory_error;};
	    	}


	    }

	    else if(strcmp(tok1,"get_min")==0){//returns the min

	    }

	    else if(strcmp(tok1,"pop_min")==0){//pops the min

	    }

	    else if(strcmp(tok1,"exit")==0){//pops the min
	    	return 0;
	    }

	    else{
	    	printf("no such command\n");
	    }

	    continue;

	    badargs:
	    	printf("command miss shaped\n");
	    	continue;

	    memory_error:
	    	printf("could not alocate memory\n");
	    	return 1;

	}
	

	return 1;
}


