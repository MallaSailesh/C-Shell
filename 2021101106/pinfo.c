#include "headers.h"

void pinfo1(char * tokens[],int tokenscnt,char * homedir)
{
    if(tokenscnt == 1){
        tokens[1] = (char *)malloc(sizeof(char)*10);
        sprintf(tokens[1],"%d",getpid());
    }
    char data_file[1000];
    int pid = -1, memory = -1 , pgrp = -1,tpgid = -1;
    sprintf(data_file,"/proc/%s/stat",tokens[1]); // "/proc/%s" is a folder 
    char status = '-';
    

    FILE * f1 = fopen(data_file,"r");
    if(f1 == NULL){
        perror("No such process exists");
        return ;
    }
    fscanf(f1,
           "%d %*s %c %*s %d %*s %*s %d %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %d",
           &pid, &status, &pgrp, &tpgid, &memory); // %*s is also used for ignoring the string 

    printf("pid : %d\n",pid);
    if(pgrp == tpgid) printf("Process Status : {%c+}\n",status); //'+' :- running in foreground
    else printf("Process Status : {%c}\n",status);
    printf("memory : %d {Virtual Memory}\n",memory);

    fclose(f1);
    
    char filename[1000];
    char * path  = (char *)malloc(sizeof(char)*1001);
    char * path_actual = (char *)malloc(sizeof(char)*1001);
    sprintf(filename,"/proc/%s/exe",tokens[1]);
    int pathlen = readlink(filename,path,1000);
    // path[pathlen] = '\0';
    if(pathlen < 0) {
        perror("Error");
        return ;
    }
    path[pathlen] = '\0';
    int isRelative = 0;
    int  i;
    for (i=0;; i++) {
      if (i!=strlen(homedir) && i != pathlen  && path[i] == homedir[i]) continue;
      else break;
    }
    if(i == pathlen){
        printf("Executable Path : %s\n","~");
        // path += strlen(homedir) - 1;
        // for(int j=i;j)
    }
    else if(i == strlen(homedir)){
        path_actual[0] = '~';
        int cnt = 1;
        for(int j=i;j<pathlen;++j) path_actual[cnt++] = path[j];
        path_actual[cnt] = '\0';
        printf("Executable Path : %s\n", path_actual);
    }
    else printf("Executable Path : %s\n", path);
}