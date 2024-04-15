#include <stdio.h>
#include "io.h"


int main () {
   FILE *fp;
   int c;
  
   const char *filename = "example.txt";
   fp=fopen(filename,"r");

   //moved a part here would be commented out if I tested load_file_to_memory
    if (fp == NULL) {
        perror("Error opening file main\n");
        return 1;
    }

   rewind(fp);
   //this works fine
   while(1) {
      c = fgetc(fp);
      if( feof(fp) ) { 
         break ;
      }
      printf("%c", c);
   }
   fclose(fp);
   
   return 0 ;
}