/*
design choice:
1. we are using a single header mainly for simplicity. the code is so short


points on style:

1. the code uses macros and conditional compilation in order to compile for all 3 cases
   simply define the case correctly and using the defined names in your c code would call the functions

2. static functions should not be used by the users of this code. they are only an implementation detail

3. the code would some times leave undefined memory temporarily and use restrict and unreachable 
   in all cases this should be viewed as code comments and assertions of correctness. 

4. the code attempts to be as efficient as possible. there isnt really a place for SIMD or multi core work here
   but what we can do is avoid allocating unnecessary memory. this is especially relevant for the inplace_sort function

*/

#ifndef HEAP_H
#define HEAP_H

#include <stdio.h>
#include <stdlib.h>

//key point UNREACHABLE and the if statements running it are actually never executed... the compiler
//specifically knows the branch is impossible in release builds and would optimize it away.
//its used as both a code comment and a way to optimize since the compiler can now plan for it

// #define CHECK_UNREACHABLE

#if defined(CHECK_UNREACHABLE)
#include <assert.h>
#define UNREACHABLE() assert(0 && "Unreachable code reached")
#elif defined(__GNUC__) || defined(__clang__)
#define UNREACHABLE() __builtin_unreachable()
#elif defined(_MSC_VER)
#define UNREACHABLE() __assume(0)
#else
//null pointer dereference to signal unreachability
#define UNREACHABLE() (*(int*)0 = 0)
#endif




//allowing the user the option to have their own types
#ifndef TYPES_FOR_DATA
#define TYPES_FOR_DATA

typedef void* data_t ;
typedef long  rank_t;

void print_rank(rank_t x){printf("%ld",x);}
void print_data(data_t x){printf("%p",x);}

#endif //TYPES_FOR_DATA

struct Node {
	rank_t rank;
	data_t data;
	struct Node* next;
};

typedef struct Node* Heap;

struct Node ERROR_NODE=(struct Node){0}; //we use this address to signal errors

//O(n)
void print_heap(Heap x){
	//loop is O(n)
	while(x != NULL){//n
		print_data(x->data);//O(1)
		printf("  ");//O(1)
		print_rank(x->rank);//O(1)
		printf("->");//O(1)

		x=x->next;//O(1)
	}
	printf("\n");//O(1)
}

//O(1)
Heap MAKE_HEAP(){
	return (Heap){0};//O(1)
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
	//no need for setting next that's on the caller
	return cur;
}

//O(n)
void free_heap(Heap x){
	while(x){//loops n times
		struct Node* temp =x->next;//O(1)*n
		free(x);//O(1)*n
		x=temp;//O(1)*n
	}
}

//O(n)
static Heap unsafe_clone_heap(Heap x){
	//deep clones a non empty heap

	//never put in null
	if(x==NULL){
	 	UNREACHABLE();
	}
	//rest of the code should be safe

	struct Node* cur =unsafe_clone_node(x); //O(1)
	if(!cur){//O(1)
		return (Heap){NULL};//O(1)
	} 
	Heap ans=cur;//O(1)
	
	//O(n)
	//the whole loop is L(n)=O(n)(no free needed)+O(n)(free operation)=O(n)
	while(x->next){//O(n)
		x=x->next;//O(1)*(n-1)
		cur->next=unsafe_clone_node(x);//O(1)*(n-1)
		cur=cur->next;//O(1)*(n-1)
		if(!cur){//O(1)*(n-1)
			//this code executes at most once
			free_heap(ans);//O(n) or 0 if never executed
			return (Heap){NULL};//O(1) or 0 if never executed
		} 
	}
	//now we do need to set that undefined memory. 
	cur->next=NULL; //O(1)
	
	return ans;//O(1)
}



#define UNION clone_merge_ordered //clone_merge_unordered
#define INSERT unordered_insert //ordered_insert
#define MINIMUM peak //get_min
#define EXTRACT_MIN pop //pop_min

//O(a+b)
Heap unsafe_merge_ordered(Heap a, Heap b){
	//reorder both lists to be 1 long list. 
	//we are assuming that the lists have no overlap and end in NULL
	if(!a){//O(1)
		return b;//O(1)
	}
	if(!b){//O(1)
		return a;//O(1)
	}

	if(a==b){
		UNREACHABLE();
	}

	Heap ans;//O(1)
	if(a->rank <=  b->rank){//O(1)
		ans=a;//O(1)
		a=a->next;//O(1)
		if(!a){//O(1)
			ans->next=b;//O(1)
			return ans;//O(1)
		}

	}
	else{
		ans=b;//O(1)
		b=b->next;//O(1)
		if(!b){//O(1)
			ans->next=a;//O(1)
			return ans;//O(1)
		}
	}
	Heap cur=ans;//O(1)
	//we will have ans->x->...->cur->random_stuff where ans..cur is ordered
	//right now we have ans->random_stuff and its the smallest
	//this will stay the case as we keep popping the smallest element
	//this is pretty much the merge function from merge sort


	//O(a+b)
	while(1){//O(a+b) times
		
		if(cur==b || cur==a){
			//no self reference for either
			UNREACHABLE();
		}

		//printf("a: ");print_heap(a);
		//printf("b: ");print_heap(b);

		if(a->rank <= b->rank){//(a+b)*O(1)
			cur->next=a;//(a)*O(1)
			a=a->next;//(a)*O(1)
			cur=cur->next;//(a)*O(1)

			if(!a){//(a)*O(1)
				cur->next=b;//O(1)
				return ans;//O(1)
			}
		}
		else{
			cur->next=b;//(b)*O(1)
			b=b->next;//(b)*O(1)
			cur=cur->next;//(b)*O(1)

			if(!b){//(b)*O(1)
				cur->next=a;//O(1)
				return ans;//O(1)
			}
		}


	}
	UNREACHABLE();
}

//O(a+b)
Heap clone_merge_ordered(Heap a,Heap b){
	//takes 2 ordered Heaps and returns an ordered Heap with the elements of both
	if(a){//O(1)
		a=unsafe_clone_heap(a);//O(a)
		if(!a){//O(1)
			return &ERROR_NODE;//exit(1);//O(1)
		}
	}
	if(b){//O(1)
		b=unsafe_clone_heap(b);//O(b)
		if(!b){//O(1)
			if(a){//O(1)
				free_heap(a);//O(a)
			}
			return &ERROR_NODE;//exit(1);//O(1)
		}
	}
	//since we JUST cloned both lists we know for a fact the clones have no overlap
	return unsafe_merge_ordered(a,b);//O(a+b)
}

//O(a)
Heap unsafe_merge_unordered(Heap a, Heap b){
	//reorder both lists to be 1 long list. 
	//we are assuming that the lists have no overlap
	if(!a){//O(1)
		return b;//O(1)
	}
	if(!b){//O(1)
		return a;//O(1)
	}

	Heap ans=a;//O(1)
	while(a->next){//O(a)
		a=a->next;//O(a)
	}
	a->next=b;//O(1)
	
	if(a==b){//no self reference
		UNREACHABLE();
	}

	return ans;//O(1)
}

//O(a+b)
Heap clone_merge_unordered(Heap a,Heap b){
	//returns a newly allocated Heap with the last element of a pointing to the first of b


	if(a){//O(1)
		a=unsafe_clone_heap(a);//O(a)
		if(!a){//O(1)
			return &ERROR_NODE;//exit(1);//O(1)
		}
	}
	if(b){//O(1)
		b=unsafe_clone_heap(b);//O(b)
		if(!b){//O(1)
			if(a){//O(1)
				free_heap(a);//O(a)
			}
			return &ERROR_NODE;//exit(1);//O(1)
		}
	}
	//since we JUST cloned both lists we know for a fact the clones have no overlap
	return unsafe_merge_unordered(a,b);//O(a)
}

//O(1)
int unordered_insert(Heap* h,rank_t rank,data_t data){
	//puts the node as the new head
	
	struct Node* new_head =malloc(sizeof(struct Node));//O(1)
	if(!new_head){//O(1)
		return 1;//O(1)
	}

	*new_head=(struct Node){rank,data,*h};//O(1)
	*h=new_head;//O(1)
	return 0;//O(1)
}


//O(n)
int ordered_insert(Heap* h,rank_t rank,data_t data){
	//puts the node as the new head
	
	Heap new_node =malloc(sizeof(struct Node));//O(1)
	if(!new_node){//O(1)
		return 1;//O(1)
	}
	*new_node=(struct Node){rank,data,NULL};//O(q)
	*h=unsafe_merge_ordered(new_node,*h);//O(n+1)=O(n)
	return 0;//O(1)
}

//we have restrict on everything here 
//because the data storage here fundamentally CANNOT be in the h heap
//it is an external buffer the caller allocated
//you might think that if rank==data this introduces new db
//however having 2 pointers of different type pointing to the same object is always ub
//so as long as the caller didn't introduce ub we are not introducing ub

//O(1)
int peak(Heap h,rank_t* restrict rank,data_t* restrict data){
	//looks at the first element

	if(!h){//O(1)
		return 1;//O(1)
	}
	*rank=h->rank;//O(1)
	*data=h->data;//O(1)
	return 0;//O(1)
}

//O(1)
int pop(Heap* h,rank_t* restrict rank,data_t* restrict data){
	//pops the first element leaving the heap 1 element shorter

	if(!h){//O(1)
		return 1;//O(1)
	}
	*rank=(*h)->rank;//O(1)
	*data=(*h)->data;//O(1)
	
	//freeing and modifying
	Heap temp=(*h)->next;//O(1)
	free(*h);//O(1)
	*h=temp;//O(1)
	return 0;//O(1)
}

//O(n)
int get_min(Heap h,rank_t* restrict rank, data_t* restrict data){
	//finds the min in an unordered heap

	if(!h){//O(1)
		return 1;//O(1)
	}//O(1)
	*rank=h->rank;//O(1)
	*data=h->data;//O(1)
	
	while(h->next){//O(n)
		h=h->next;//n*O(1)
		if(h->rank < *rank){//n*O(1)
			*rank=h->rank;//n*O(1)
			*data=h->data;//n*O(1)
		}
	}
	return 0;//O(1)
}

//O(n)
int pop_min(Heap* h,rank_t* restrict rank,data_t* restrict data){
	//finds the min in an unordered heap and removes that Node

	if(!h){//O(1)
		return 1;//O(1)
	}
	*rank=(*h)->rank;//O(1)
	*data=(*h)->data;//O(1)
	
	Heap premin=NULL;//O(1)
	Heap cur=*h;//O(1)

	while(cur->next){//O(n)
		if(cur->next->rank < *rank){//n*O(1)
			*rank=cur->rank;//n*O(1)
			*data=cur->data;//n*O(1)
			premin=cur;//n*O(1)
		}
		cur=cur->next;//n*O(1)
	}

	if(premin==NULL){//O(1)
		cur=(*h)->next;//O(1)
		free(*h);//O(1)
		*h=cur;//O(1)
		return 0;//O(1)
	}

	Heap to_free =premin->next;//O(1)
	cur=to_free->next;//O(1)
	free(to_free);//O(1)
	premin->next=cur;//O(1)

	return 0;//O(1)
}

//sorting is a bounce but it seems fun so I am doing it anyway.
//this can be parallelized if I took the time but it introduces a lot of syncing overhead so I elected to not do that

//O(size)
Heap get_end(Heap x,int size){
	for(int i=0;i<size;i++){//O(size)
		if(x==NULL){//size*O(1)
			return NULL;//O(1)
		}
		x=x->next;//size*O(1)
	}
	return x;//O(1)
}

//O(size)
Heap* get_end_ref(Heap x,int size){
	if(x==NULL || size==0){
		UNREACHABLE();
	}

	while(1){//O(size)
		size-=1;//size*O(1)
		if(size==0){//size*O(1)
			return &(x->next);//O(1)
		}
		if(x->next==NULL){//size*O(1)
			return &(x->next);//O(1)
		}
		x=x->next;//size*O(1)
	}
}

//this algorithm is essentially merge sort with 1 major difference its constant memory
//since we have linkedlists as our data structure we can merge in place without needing to allocate a new array

//to put this in perspective quicksort is the leading sorting algorithm because its O(log(n)) memory
//so this algorithm is even more memory efficient and doesn't have the worse case O(n^2) problem

//I am avoiding making internal calls here because its very easy to get log(n) memory with these like quicksort does
//you can verify its constant memory since we only allocate memory for each named stack variable (a constant size set)

//the goto pattern I made is very similar to internal function calls its in the codeblock itself to avoid passing around stack pointers
//its purely for making the code more readable. putting everything in a triple while loop would give the same result

//O(nlog(n))
void inplace_sort(Heap *h){
	if(*h==NULL){
		return;
	}
	//1<=step<=2n => O(step)=O(n) O(1/step)=O(1)
	int step=1;//O(1)

	//cur_tail is the current end of the sorted stack.
	Heap* cur_tail=h;//O(1)
	//you will notice we use *cur_tail every time we push an item
	
	//a and b refer to the SUBLISTS 
	Heap a=*h;//O(1)
	Heap end_a=get_end(a,step);//O(1)
	Heap b=end_a;//O(1)

	Heap* end_b_ref=get_end_ref(b,step); //passed by reference so we can update the tail cheaper //O(1)
	Heap  end_b=*end_b_ref;//O(1)
	
	//loops until the first sublist is the whole list (end_a=null)
	
	while(1){//O(log2(n))
		goto sort_for_step;//log2(n)*O(n)
		end_sort_for_step:

		step*=2;//log2(n)*O(1)

		//get the first 2 sublists of the array
		cur_tail=h;//log2(n)*O(1)
		a=*h;//log2(n)*O(1)
		end_a=get_end(a,step);//log2(n)*O(step)
		if(end_a==NULL){//log2(n)*O(step)
			break;//O(1)
		}
		b=end_a;//log2(n)*O(1)
		end_b_ref=get_end_ref(b,step);//log2(n)*O(step)=O(nlog2(n))
		end_b=*end_b_ref;//log2(n)*O(1)
	}
	return;

//O(n)=O(n/2step)*O(step)
sort_for_step:
	//merge all ordered sublists of length step (last sublist may be shorter)
	
	//loops until the "a" sublist has no list to its left (ie end_a=null)
	while(1){//O(n/2step)
		goto merge_heaps;//O(n)=(n/2step)*O(step+step) 
		end_merge_heaps:

		//get the next sublist
		a=end_b;//(n/2step)*O(1)=O(n)
		end_a=get_end(a,step);//(n/2step)*O(1)=O(n)
		if(end_a==NULL){//(n/2step)*O(1)=O(n)
			break;//O(1)
		}

		b=end_a;//(n/2step)*O(1)=O(n)
		end_b_ref=get_end_ref(b,step);//(n/2step)*O(step)=O(n)
		end_b=*end_b_ref;//(n/2step)*O(1)=O(n)

		if(a==NULL || b==NULL || end_b_ref==NULL){
			UNREACHABLE();
		}
	}

	goto end_sort_for_step;//O(1)

//O(a+b)
merge_heaps:
	//this block reorders *cur_tail->a..->b->end_b 	
	//to *cur_tail->c0..->cn->end_b
	//it also updates the cur_tail to point where the current tail is
	
	//loops until the a or b sublist is empty (ie a==end_a or b==end_b)
	while(1){//O(a+b)
		
		if(a==b || cur_tail==NULL){
			UNREACHABLE();
		}

		if(a->rank <= b->rank){//(a+b)*O(1)

			//pop from "a" into our tail
			*cur_tail=a;//a*O(1)
			cur_tail=&((*cur_tail)->next); //a*O(1)
			a=a->next;//a*O(1)
			
			if(a==end_a){//a*O(1)
				//put the rest of b in our tail

				*cur_tail=b;//O(1)
				//we saved the end of b already
				cur_tail=end_b_ref;//O(1)
				//we have prev_tail->...->end_a->..->last b [end_b_ref]->end_b
				break;//O(1)
			}
		}
		else{
			//pop from b into our tail
			*cur_tail=b;//b*O(1)
			cur_tail=&((*cur_tail)->next);//b*O(1)
			b=b->next;//b*O(1)

			if(b==end_b){//b*O(1)
				//put the rest of "a" in our tail

				*cur_tail=a;//O(1)
				
				//since end_a was in the b heap we have put it into our sublist
				//we have an inner loop prev_tail->...->end_a->..->a->..end_a
				while(a->next!=end_a){//O(a)
					a=a->next;//a*O(1)
				}
				a->next=end_b;//O(1)
				//now we have reordered entry->a0..->am->an->b0...->bk->end_b 
				//to entry->c0..->bk->am->...->an->end_b 
				
				cur_tail=&(a->next);//O(1)
				break;//O(1)
			}
		}
	}

	goto end_merge_heaps;//O(1)
}
#endif //HEAP_H