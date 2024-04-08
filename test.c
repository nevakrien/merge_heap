#include "heap.h"

int main(){
	struct Heap x = MAKE_HEAP();
	for(int i=0;i<10;i++){
		print_heap(x);
		unordered_insert(&x,0,&x);
	}
	free_heap(x);
	return 0;
}