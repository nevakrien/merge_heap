#include "io.h"

int main() {
    //system ("cd");
    //system("pwd");

    char *file_content;
    const char *filename = "example.txt";


    long int result = load_file_to_memory(filename, &file_content);
    if (result == -1) {
        free(file_content);
        return 1;
    }
    printf("File content (size %ld):\n%s\n", result, file_content);

    Heap ans=split_lines_noalloc(file_content,result);
    if(self_alocate_data_t(ans)){
        printf("self alocation failed");
        return 1;
    }
    free(file_content);

    print_heap(ans);
    free_heap_full(ans);

    return 0;
}

