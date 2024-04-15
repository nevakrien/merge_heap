#include "io.h"

int main() {
    //system ("cd");
    //system("pwd");

    char *file_content;
    const char *filename = "example.txt";
    
    //test print
    Heap test=NULL;
    unordered_insert(&test,sizeof(filename),filename);
    print_heap(test);


    long int result = load_file_to_memory(filename, &file_content);
    if (result == -1) {
        free(file_content);
        return 1;
    }
    printf("File content (size %ld):\n%s\n", result, file_content);

    Heap ans=split_lines_noalloc(file_content,result);
    print_heap(ans);

    return 0;
}

