/*
desgin choice:
1. we are using a single header mainly for simplicity. the code is so short


points on style:

1. the code uses macros and conditional compilation in order to compile for all 3 cases
   simply define the case correctly and using the defined names in your c code would call the functions

2. static functions should not be used by the users of this code. they are only and implementation detail

3. the code would some times leave undfined memory temporerily and use unreachble 
   in all cases this should be viewed as code comments and assertions of correctnes.

*/

#ifndef HEAP_H
#define HEAP_H

#include <stdio.h>
#include <stdlib.h>

//key point UNREACHABLE and the if statments runing it are actually never excuted... the compiler
//specifcly knows the brench is impossible in realease builds and would optimize it away.
//its used as both a code comment and a way to optimize since the compiler can now plan for it

#define CHECK_UNREACHABLE

#if defined(CHECK_UNREACHABLE)
#include <assert.h>
#define UNREACHABLE() assert(0 && "Unreachable code reached")
#elif defined(__GNUC__) || defined(__clang__)
#define UNREACHABLE() __builtin_unreachable()
#elif defined(_MSC_VER)
#define UNREACHABLE() __assume(0)
#else
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

typedef struct Node* Heap;


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
	//no need for setting next thats on the caller
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
			//this code excutes at most once
			free_heap(ans);//O(n) or 0 if never excuted
			return (Heap){NULL};//O(1) or 0 if never excuted
		} 
	}
	//now we do need to set that undfined memory. 
	cur->next=NULL; //O(1)
	
	return ans;//O(1)
}



#define UNION clone_merge_ordered //clone_merge_unordered
#define INSERT unordered_insert //ordered_insert
#define MNIMUM peak //get_min
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
	//this will stay the case as we keep poping the smallest element
	//this is pretty much the merge function from merge sort


	//O(a+b)
	while(1){//O(a+b) times
		
		if(cur==b || cur==a){
			//no self refrence for either
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
			exit(1);//O(1)
		}
	}
	if(b){//O(1)
		b=unsafe_clone_heap(b);//O(b)
		if(!b){//O(1)
			exit(1);//O(1)
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
	
	if(a==b){//no self refrence
		UNREACHABLE();
	}

	return ans;//O(1)
}

//O(a+b)
Heap clone_merge_unordered(Heap a,Heap b){
	//returns a newly alocated Heap with the last element of a pointing to the first of b


	if(a){//O(1)
		a=unsafe_clone_heap(a);//O(a)
		if(!a){//O(1)
			exit(1);//O(1)
		}
	}
	if(b){//O(1)
		b=unsafe_clone_heap(b);//O(b)
		if(!b){//O(1)
			exit(1);//O(1)
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
//because the data storage here fundementaly can not be in the heap
//it is an external buffer the caller alocated
//you might think that if rank==data this introduces new db
//however having 2 pointers of diffrent type pointing to the same object is allways ub
//so as long as the caller didnt introduce ub we are not introducing ub

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
	
	//freeing and modifiying
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
//this can be paralalized if I took the time but it introduces a lot of syncing overhead so I elected to not do that

// Heap get_end(Heap x,int size){
// 	for(int i=0;i<size;i++){
// 		if(x==NULL){
// 			return NULL;
// 		}
// 		x=x->next;
// 	}
// }

// //I am avoiding making internal calls here because its very easy to get log(n) memory with these like mergesort does
// //we have a unique opretunaty here to be constant memory which I am gona go for
// void inplace_sort(Heap *h){
// 	int step=1;
// 	Heap* cur_tail=h;
	
// 	Heap a=*h;
// 	Heap end_a=a->next;
	
// 	Heap b=a_end;
// 	Heap end_b=get_end(b,step);
	
// 	while(end_a!=NULL){
// 		//merge all ordered sublists of length step (last sublist may be shorter)
// 		while(b){
// 			//this block reorders *cur_tail->a..->b->end_b 	
// 			//to *cur_tail->c0..->cn>end_b
// 			while(1){
				
// 				if(a->rank <= b->rank){
// 					*cur_tail=a;
// 					cur_tail=&((*cur_tail)->next); //steping so our tail reflects the end of the sorted stack
// 					a=a->next;
					
// 					if(a==end_a){
// 						(*cur_tail)->next=b;
// 						//wrong //cur_tail=&((*cur_tail)->next);
// 						//a=end_b;
// 						while(b->next!=end_b){
// 							b=b->next;
// 						}
// 						//we have end_b as our end
// 						cur_tail=&(b->next);
// 						break;
// 					}
// 				}
// 				else{
// 					*cur_tail=b;
// 					cur_tail=&((*cur_tail)->next);
// 					b=b->next;

// 					if(b==end_b){
// 						(*cur_tail)->next=a;
// 						//wrong //cur_tail=&((*cur_tail)->next);
						
// 						//since end_a was in the b heap we put it into out stack
// 						//we have an inner loop prev_tail->...->end_a->..->a->..end_a
// 						while(a->next!=end_a){
// 							a=a->next;
// 						}
// 						a->next=end_b;
// 						//now we have reordered entry->a0..->am->an->b0...->bk->end_b 
// 						//to entry->c0..->bk->am->...->an->end_b 
						
// 						cur_tail=&(a->next);
// 						//a=end_b;
// 						break;
// 					}
// 				}
// 			}

// 			a=end_b;
// 			end_a=get_end(a,step);
// 			b=end_a;
// 			end_b=get_end(b,step);
// 		}

// 		step*=2;
// 		cur_tail=h;
// 	}

// }
#endif //HEAP_H