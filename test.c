#include "heap.h"
#include <stdint.h>

void *canary = (void *)69;

uint32_t state=111;

rank_t random_num(){
	state=state*51253+111112;
	return state;
}

Heap range(int lim){
	Heap ans = MAKE_HEAP();
	for(int i=0;i<lim;i++){
		unordered_insert(&ans,i,canary);
	}
	return ans;
}

Heap random_heap(int lim){
	Heap ans = MAKE_HEAP();
	for(int i=0;i<lim;i++){
		unordered_insert(&ans,(random_num()%20)-10,canary);
	}
	return ans;
}

Heap random_ordered_heap(int lim){
	Heap ans = MAKE_HEAP();
	for(int i=0;i<lim;i++){
		//print_heap(ans);
		ordered_insert(&ans,random_num()%10,canary);
	}
	return ans;
}

int main(){
	printf("\n\nchecking printing and basic allocation\n");
	Heap a = MAKE_HEAP();
	for(int i=0;i<3;i++){
		print_heap(a);
		unordered_insert(&a,0,canary);
	}
	free_heap(a);

	printf("\n\nchecking ordered merging\n");
	Heap b=random_ordered_heap(5);
	print_heap(b);

	a=random_ordered_heap(5);
	print_heap(a);

	Heap c=clone_merge_ordered(a,b);
	print_heap(c);
	
	free_heap(a);
	free_heap(b);
	free_heap(c);

	printf("\n\nchecking unordered\n");
	a= random_heap(5);

	rank_t rank;
	data_t data;
	
	print_heap(a);	
	pop_min(&a,&rank,&data);
	printf("our min key we poped is%ld\n",rank);

	print_heap(a);	
	get_min(a,&rank,&data);
	printf("our min key is %ld\n",rank);
	
	free_heap(a);

	printf("\n\ntesting sort\n");
	a= random_heap(16);
	print_heap(a);
	
	printf("\nsorting\n");
	inplace_sort(&a);
	print_heap(a);

	free_heap(a);

	return 0;
}