#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "io.h"

/* style guide:

	this code exit on memory fail. it also fundemntaly uses global state
	this is because this is essentially an intepeter for a gced languge...

	we prefer deep cloning here over effishency so that the code can stay simple.

	duplicate names are handeled by ignoring the old version (it would still show up)

*/

//command IO
#define MAX_INPUT_LENGTH 4096
char input[MAX_INPUT_LENGTH];
char *tok1;
char *tok2;
char *tok3;
char *tok4;

const char *delim = " "; 

rank_t rank1;
data_t data1;

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

void show_result_node(){
	print_data(data1);
	printf("  ");
	print_rank(rank1);
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

void freeHeapNode(struct HeapList* h){
	free_heap_full(h->self.h);
	free(h);
}

int remove_heap(char* name){
	if(Heaps==NULL){
		return 1;
	}

	if(strcmp(Heaps->self.name,name)==0){
		struct HeapList* temp=Heaps->next;
		freeHeapNode(Heaps);
		Heaps=temp;
		return 0;
	}

	struct HeapList* prev=Heaps;
	struct HeapList* tail=Heaps->next;

	while(tail!=NULL){
		if(strcmp(tail->self.name,name)==0){
			prev->next=tail->next;
			freeHeapNode(tail);
			return 0;
		}
		prev=tail;
		tail=tail->next;
	}
	return 1;
}

Heap load_from_file(const char* filename){
	char* file_content;

	long int result = load_file_to_memory(filename, &file_content);
    if(result==-1){
    	perror("Error opening file\n");
    	return &ERROR_NODE;
    }
    if (result == -2) {
        //free(file_content);
        printf("file too large!!!\n");
        exit(1);
    }

    Heap ans=split_lines_noalloc(file_content,result);

    if(self_alocate_data_t(ans)){
        printf("file too large!!!\n");
        exit(1);
    }

    return ans;
}

void print_help(){
	printf("Available commands:\n");
	printf("  help                 Displays this help message.\n");
	printf("  show [heap_name]     Displays details about a specified heap.\n");
	printf("\n");
	printf("  load [file_name] [heap_name]  Loads a file's lines into a heap ranking them by byte length.\n");
	printf("  dump [heap_name] [file_name]  Dumps the specified heap's text into a file.\n");
	printf("  make [heap_name] [sorted]     Creates an empty heap. 'sorted' can be 1 (true) or 0 (false).\n");
	printf("  remove [heap_name]            Removes the specified heap.\n");
	printf("\n");
	printf("  sort [heap_name]      Sorts the specified heap and changes its type to sorted.\n");
	printf("  to_unsort [heap_name] Changes a sorted heap's type to unsorted.\n");
	printf("\n");
	printf("  merge [heap_name1] [heap_name2] [output_heap_name]  Merges two heaps into a new heap with the given name.\n");
	printf("  clone [source_heap_name] [new_heap_name]            Clones a heap with a new name.\n");
	printf("\n");
	printf("  insert [heap_name] [rank] [data]  Inserts a new element into the specified heap.\n");
	printf("  get_min [heap_name]   Returns the minimum element from the specified heap without removing it.\n");
	printf("  pop_min [heap_name]   Removes and returns the minimum element from the specified heap.\n");
	printf("\n");
	printf("  exit                  Exits the program.\n");
	printf("\n");

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

	    //big us "switch" case for all possible commands
	    //printf("\nDebug: tok1 = '%s'\n", tok1);

	    if(strcmp(tok1,"help")==0){
	    	print_help();
	    }
	    else if(strcmp(tok1,"show")==0){//prints heap
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
	    	tok2 = strtok(NULL, delim); //file name
	    	tok3 = strtok(NULL, delim); //name (optional)
	    	if(tok2==NULL){
	    		goto badargs;
	    	}

	    	Heap ans=load_from_file(tok2);
	    	if(ans==&ERROR_NODE){
	    		continue;
	    	}

	    	if(tok3==NULL){
	    		tok3=tok2;
	    	}

	    	addHeap(ans,0,tok3);
	    }

	    else if(strcmp(tok1,"dump")==0){//dumpts a heap
	    	tok2 = strtok(NULL, delim); //heap name
	    	tok3 = strtok(NULL, delim); //filename 
	    	if(tok2==NULL || tok3==NULL){
	    		goto badargs;
	    	}

	    	struct UserHeap* uh=get_heap(tok2);
	    	if(uh==NULL){
	    		printf("heap dosent exists\n");
	    		continue;
	    	}

	    	if(dump_heap(uh->h,tok3)){
	    		printf("dump failed\n");
	    	}
	    	else printf("dumped file\n");
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
	    	
	    	//addHeap(NULL,sorted,tok2);
	    	addHeap(MAKE_HEAP(),sorted,tok2); //MAKE_HEAP just gives null...
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
	    	tok2 = strtok(NULL, delim); //name
	    	if(tok2==NULL){
	    		goto badargs;
	    	}

	    	struct UserHeap* uh=get_heap(tok2);
	    	if(uh==NULL){
	    		printf("heap dosent exists\n");
	    		continue;
	    	}

	    	inplace_sort(&uh->h);
	    	uh->sorted=1;

	    }

	    else if(strcmp(tok1,"to_unsort")==0){//changes the type to sorted
			tok2 = strtok(NULL, delim); //name
	    	if(tok2==NULL){
	    		goto badargs;
	    	}

	    	struct UserHeap* uh=get_heap(tok2);
	    	if(uh==NULL){
	    		printf("heap dosent exists\n");
	    		continue;
	    	}

	    	uh->sorted=0;
	    }

	    else if(strcmp(tok1,"merge")==0){//merges two heaps of the same type (if not same type errors)
	    	tok2 = strtok(NULL, delim); //source1
	    	tok3 = strtok(NULL, delim); //source2
	    	tok4 = strtok(NULL, delim); //target

	    	if(tok2==NULL || tok3==NULL || tok4==NULL){
	    		goto badargs;
	    	}

	    	struct UserHeap* a=get_heap(tok2);
	    	if(a==NULL){
	    		printf("source 1 dosent exist\n");
	    		continue;
	    	}
	    	struct UserHeap* b=get_heap(tok3);
	    	if(b==NULL){
	    		printf("source 2 dosent exist\n");
	    		continue;
	    	}

	    	int sorted=0;
	    	if(a->sorted!=b->sorted){
	    		printf("incompatible types using unsorted\n");
	    	}
	    	else if(a->sorted==1){
	    		sorted=1;
	    	}

	    	Heap ans;
	    	if(sorted){
	    		ans=clone_merge_ordered(a->h,b->h);
	    	}
	    	else{
	    		ans=clone_merge_unordered(a->h,b->h);
	    	}

	    	if(ans==&ERROR_NODE){
	    		goto memory_error;
	    	}

	    	addHeap(ans,sorted,tok4);
	    }

	    else if(strcmp(tok1,"clone")==0){//clones a heap naming the clone a user provided thing
	    	tok2 = strtok(NULL, delim); //source
	    	tok3 = strtok(NULL, delim); //target

	    	if(tok2==NULL || tok3==NULL){
	    		goto badargs;
	    	}

	    	struct UserHeap* original=get_heap(tok2);
	    	if(original==NULL){
	    		printf("no such Heap\n");
	    		continue;
	    	}

	    	Heap clone = original->h;
	    	if(clone!=NULL){
	    		clone=unsafe_clone_heap(clone);
	    		if(clone==NULL){
	    			goto memory_error;
	    		}
	    	}

	    	addHeap(clone,original->sorted,tok3);
	    }

	    else if(strcmp(tok1,"insert")==0){//inserts to a heap (if sorted keeps it sorted)
	    	tok2 = strtok(NULL, delim); //name
	    	tok3 = strtok(NULL, delim); //rank
	    	tok4 = strtok(NULL, delim); //data

	    	if(tok2==NULL || tok3==NULL || tok4==NULL){
	    		goto badargs;
	    	}

	    	//rank_t rank;
	    	if(parse_int(tok3,&rank1)){
	    		printf("3d not an int\n");
	    		goto badargs;
	    	}

	    	struct UserHeap* uh=get_heap(tok2);
	    	if(uh==NULL){
	    		printf("heap dosent exists\n");
	    		continue;
	    	}

	    	size_t size=strlen(tok4)+1;
	    	char* new_str= malloc(size);
	    	if(!new_str){
	    		goto memory_error;
	    	}
	    	memcpy(new_str,tok4,size);


	    	if(uh->sorted){
	    		if(ordered_insert(&uh->h,rank1,new_str)){goto memory_error;};
	    	}
	    	else{
	    		if(unordered_insert(&uh->h,rank1,new_str)){goto memory_error;};
	    	}


	    }

	    else if(strcmp(tok1,"get_min")==0){//returns the min
	    	tok2 = strtok(NULL, delim); //name

	    	if(tok2==NULL){
	    		goto badargs;
	    	}

	    	struct UserHeap* u=get_heap(tok2);
	    	if(u==NULL){
	    		printf("no such heap\n");
	    		continue;
	    	}

	    	if(u->sorted){
	    		if(peak(u->h,&rank1,&data1)){
	    			printf("!!!error empty heap!!!\n");
	    			continue;
	    		}
	    	}

	    	else if(get_min(u->h,&rank1,&data1)){
	    		printf("!!!error empty heap!!!\n");
	    		continue;
	    	}

	    	show_result_node();
	    	continue;

	    }

	    else if(strcmp(tok1,"pop_min")==0){//pops the min
			tok2 = strtok(NULL, delim); //name

	    	if(tok2==NULL){
	    		goto badargs;
	    	}

	    	struct UserHeap* u=get_heap(tok2);
	    	if(u==NULL){
	    		printf("no such heap\n");
	    		continue;
	    	}

	    	if(u->sorted){
	    		if(pop(&u->h,&rank1,&data1)){
	    			printf("!!!error empty heap!!!\n");
	    			continue;
	    		}
	    	}

	    	else if(pop_min(&u->h,&rank1,&data1)){
	    		printf("!!!error empty heap!!!\n");
	    		continue;
	    	}

	    	show_result_node();
	    	free(data1); //we got back a string that is no longer in use
	    	continue;
	    }

	    else if(strcmp(tok1,"exit")==0){//pops the min
	    	return 0;
	    }

	    else{
	    	printf("no such command try typing \"help\"\n");
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


