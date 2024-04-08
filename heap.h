/*
desgin choice:
1. we are using a single header mainly for simplicity. the code is so short t


points on style:

1. the code uses macros and conditional compilation in order to compile for all 3 cases
   simply define the case correctly and using the defined names in your c code would call the functions

2. static functions should not be used by the users of this code. they are only and implementation detail

3. the code would some times leave undfined memory temporerily and use unreachble 
   in all cases this should be viewed as code comments and assertions of correctnes.

   there is 1 case where we are explicitly told that 2 heaps have no overlap this means we can call functions on them
   with assuming no overlap.if a user breaks this promice we get UB thats the ONLY case of undefined behivior in the code
*/

#ifndef HEAP_H
#define HEAP_H

#include <stdio.h>
#include <stdlib.h>

#if defined(CHECK_UNREACHABLE)
#define UNREACHABLE() assert(0 && "Unreachable code reached")
#elif defined(__GNUC__) || defined(__clang__)
#define UNREACHABLE() __builtin_unreachable()
#elif defined(_MSC_VER)
#define UNREACHABLE() __assume(0)
#else
#include <assert.h>
//null pointer derefrence to signal unreachbilety
#define UNREACHABLE() (*(volatile int*)0 = 0)
#endif



typedef void* data_t ;
typedef int  rank_t;

void print_rank(rank_t x){printf("%d",x);}
void print_data(data_t x){printf("%p",x);}

struct Node {
	rank_t rank;
	data_t data;
	struct Node* next;
};

struct Heap{
	struct Node* head;
};

void print_heap(struct Heap x){
	while(x.head != NULL){
		print_data(x.head->data);
		printf("  ");
		print_rank(x.head->rank);
		printf("->");

		x.head=x.head->next;
	}
	printf("\n");
}

//O(1)
struct Heap MAKE_HEAP(){
	return (struct Heap){0};//O(1)
}

//O(1)
int unordered_insert(struct Heap* h,rank_t rank,data_t data){
	//puts the node as the new head
	
	struct Node* new_head =malloc(sizeof(struct Node));//O(1)
	if(!new_head){//O(1)
		return 1;//O(1)
	}

	*new_head=(struct Node){rank,data,h->head};//O(1)
	h->head=new_head;//O(1)
	return 0;//O(1)
}

//O(1)
static struct Node* unsafe_clone_node(struct Node* node){
	//clones the data if a Node leaving the next undefined 

	struct Node* cur =malloc(sizeof(struct Node)); //O(1)
	if(!cur){//O(1)
		return NULL;//O(1)
	} 

	cur->rank=node->rank;//O(1)
	cur->data=node->data;//O(1)
	//no need for setting next thats on the caller
	return cur;
}

//O(n)
void free_heap(struct Heap x){
	while(x.head){//loops n times
		struct Node* temp =x.head->next;//O(1)*n
		free(x.head);//O(1)*n
		x.head=temp;//O(1)*n
	}
}

//O(n)
static struct Heap unsafe_clone_heap(struct Heap x){
	//deep clones a non empty heap

	//never put in null
	if(x.head==NULL){
	 	UNREACHABLE();
	}
	//rest of the code should be safe

	struct Node* cur =unsafe_clone_node(x.head); //O(1)
	if(!cur){//O(1)
		return (struct Heap){NULL};//O(1)
	} 
	struct Heap ans={cur};//O(1)
	
	//O(n)
	//the whole loop is L(n)=O(n)(no free needed)+O(n)(free operation)=O(n)
	while(x.head->next){//loops n-1 times
		x.head=x.head->next;//O(1)*(n-1)
		cur->next=unsafe_clone_node(x.head);//O(1)*(n-1)
		cur=cur->next;//O(1)*(n-1)
		if(!cur){//O(1)*(n-1)
			//this code excutes at most once
			free_heap(ans);//O(n) or 0 if never excuted
			return (struct Heap){NULL};//O(1) or 0 if never excuted
		} 
	}
	//now we do need to set that undfined memory. 
	cur->next=NULL; //O(1)
	
	return ans;//O(1)
}

#define MERGER(a,b)(a)//for testing only

#define UNION clone_merge
//#define clone_merge UNION 
/*also works and TECHNICALLY more correct since it makes the function named union in the binary
its a bad practice and I would argue it makes the code less readble
since this is a header only project having this define here allways works*/ 


struct Heap unsafe_merge( struct Heap a, struct Heap b){
	//reorder both lists to be 1 long list. 
	//we are assuming that the lists have no overlap
	if(!a.head){
		return b;
	}
	if(!b.head){
		return a;
	}

	if(a.head==b.head){
		UNREACHABLE();
	}

	return MERGER(a,b);
}

struct Heap clone_merge(struct Heap a,struct Heap b){
	if(a.head){
		a=unsafe_clone_heap(a);
		if(!a.head){
			exit(1);
		}
	}
	if(b.head){
		b=unsafe_clone_heap(b);
		if(!b.head){
			exit(1);
		}
	}
	//since we JUST cloned both lists we know for a fact the clones have no overlap
	return unsafe_merge(a,b);
}

#endif //HEAP_H