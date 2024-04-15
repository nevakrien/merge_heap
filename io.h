#ifndef IO_H
#define IO_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef _WIN32
#include <errno.h>
#endif

#define TYPES_FOR_DATA
typedef char* data_t ;
typedef long  rank_t;

void print_rank(rank_t x){printf("%ld",x);}
void print_data(data_t x){printf("%s",x);}

#include "heap.h"




long int load_file_to_memory(const char *filename, char **ans) {
    FILE *file = fopen(filename, "r");
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

    #ifdef _WIN32 //windows does file sizes super weird
    errno=0;
    long int r=fread(*ans, 1, size, file);
    if (errno != 0) {
        if (errno == EIO) {
            printf("An I/O error occurred.\n");
        } else if (errno == ENOMEM) {
            printf("Not enough memory.\n");
        } else if (errno == EINVAL) {
            printf("Invalid argument.\n");
        } else {
            printf("An unspecified error occurred.\n");
        }

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
    return r;

    #else
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

    #endif//_WIN32
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


#endif //IO_H