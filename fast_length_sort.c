#include <stdio.h>
#include "io.h"

//we are not doing cleanup for speed.
int main(int argc, char *argv[]) {
    // Check if the correct number of arguments is passed
    if (argc != 3) {
        printf("Usage: %s inputfile outputfile\n", argv[0]);
        return 1;
    }

    char *file_content;
    long int result = load_file_to_memory(argv[1], &file_content);
    if (result == -1) {
        perror("Error opening file\n");
        return 1;
    }

    if(result<0){
        return 1;
    }

    Heap ans=split_lines_noalloc(file_content,result);
    inplace_sort(&ans);

    if(dump_heap_trusted(ans,argv[2])){
        printf("error dumping heap\n");
        return 1;
    }


    return 0;
}
