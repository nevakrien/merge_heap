#ifndef IO_H
#define IO_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


#define TYPES_FOR_DATA
typedef char* data_t ;
typedef long  rank_t;

void print_rank(rank_t x){printf("%ld",x);}
void print_data(data_t x){printf("%s",x);}

#include "heap.h"




long int load_file_to_memory(const char *filename, char **ans) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file\n");
        return -1;
    }

    // Move to the last character of the file to check if it's empty
    if (fseek(file, -1, SEEK_END) != 0) {
        fclose(file);

        //giving an empty string
        *ans = malloc(1); 
        if (*ans == NULL) {
            fputs("Memory allocation failed\n", stderr);
            return -1;
        }
        **ans = '\0'; 
        return 0;  
    }

    //check if we need a null terminator
    int last_char = fgetc(file);
    char need_extra_byte = (last_char != '\0'); //if we need extra stuff

    long int size = ftell(file);

    *ans = malloc(size + need_extra_byte);
    if (*ans == NULL) {
        fclose(file);
        fputs("Memory allocation failed\n", stderr);
        return -1;
    }

    // Read the file from the beginning
    rewind(file);

    if (fread(*ans, 1, size, file) != size) {
        free(*ans);
        fclose(file);
        fputs("Error reading file\n", stderr);
        return -1;
    }

    // Set the null terminator if needed
    if (need_extra_byte) {
        (*ans)[size] = '\0';
    }
    fclose(file);//close the file
    return size + need_extra_byte;  // Return the size of the file

}

//modifies the input
Heap split_lines_noalloc(char* str,long int size){
    Heap head=NULL;
    //printf("string is: %s\n",str);
    while(size>0){
        //printf("pre loop substirng %s\n",str);
        char *end=str;
        while(*end!='\n' && *end!='\0'){
            //printf("seeing: <%c>\n",*end);
            #ifdef _WIN32
            if(*end=='\r'){
                *end='\0';
            }
            #endif
            end++;
        }
        
        *end='\0'; //making a null terminator
        //printf("substirng %s\n",str);
        long int len=1+(end-str);

        //inserting
        if(unordered_insert(&head,len,str)){
            free_heap(head);
            return &ERROR_NODE;
        }
        size-=len;
        //printf("len is:%ld and size is %ld\n",len,size);
        str=end+1;
    }

    return head;
}

//frees head on fail
int self_alocate_data_t(Heap head){
    Heap tail=head;
    while(tail){
        char* clone_data=malloc(tail->rank);
        if(!clone_data){
            goto exit_error;
        }

        //strcpy(clone_data,tail->data);
        memcpy(clone_data,tail->data,tail->rank);
        tail->data=clone_data;
        tail=tail->next;
    }

    return 0;

exit_error:
    while(head!=tail){
        free(head->data);
        head=head->next;
    }

    free_heap(head);
    return 1;
}

void free_heap_full(Heap x){
    while(x){
        free(x->data);
        Heap temp =x->next;
        free(x);
        x=temp;
    }
}

#ifdef _WIN32
const char* terminator="\r\n";
const char terlen=2;
#else
const char* terminator="\n";
const char terlen=1;
#endif

int dump_heap(Heap h,const char *filename){
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Error opening file\n");
        return 1;
    }

    while(h){
        long size=h->rank-terlen;
        if(fwrite(h->data,1,size,file)!=size){
            goto exit_write_error;
        }
        if(fwrite(terminator,1,terlen,file)!=terlen){
            goto exit_write_error;
        }
        h=h->next;
    }

    fclose(file);
    return 0;

    exit_write_error:
        perror("Error writing to file\n");
        fclose(file);
        remove(filename);
        return 1;
}

// /**
//  * Parses a string into an integer.
//  * @param str The string to parse.
//  * @param out A pointer to an integer where the parsed value will be stored.
//  * @return 0 on success, 1 on error.
//  */
// int parse_int(const char *str, int *out) {
//     char *end;
//     if (str == NULL) {
//         return 1; // Null string pointer is an error
//     }

//     // Handle empty string
//     if (*str == '\0') {
//         return 1;
//     }

//     // Convert string to long using strtol
//     long result = strtol(str, &end, 10); // Base 10

//     // Check if the conversion was successful
//     if (end == str) {
//         return 1; // No digits were found
//     }

//     // Check for any remaining characters after the number
//     while (*end != '\0') {
//         if (!isspace((unsigned char)*end)) {
//             return 1; // Additional non-whitespace characters found
//         }
//         end++;
//     }

//     // Check if the value is out of the int range
//     if (result > INT_MAX || result < INT_MIN) {
//         return 1; // Result is out of the range of an int
//     }

//     // Successful conversion
//     *out = (int)result;
//     return 0;
// }

#endif //IO_H