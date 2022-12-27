#include "changedir.h"
#include "headers.h"

struct stat datastorage ;

void cd1(char * prevdir , char * dir , char* tokens[],int tokenscnt, char * homedir)
{
    if(tokenscnt > 2){ // if there are more than two arguments 
        printf("Too many arguments!!\n");
        return ;
    }
    else if( tokenscnt == 1 || (tokens[1][0] == '~' && strlen(tokens[1]) == 1) ) { // if it is cd or cd ~
        strcpy(dir,"~");
        int check = chdir(homedir); // then chnaging the directory to ~
        if(check < 0){
            perror("Shell ");
        }
        return ;
    }
    else if(tokens[1][0] == '~'){ // if it is in form ~/help etc ( where help is a directory )
        char store[10000] = "";
        strcpy(store,homedir);
        strcat(store,&tokens[1][1]);
        int check = chdir(store);
        if(check < 0){
            perror("Shell ");
        }
        else{
            char cwd[10000];
            getcwd(cwd, sizeof(cwd));
            if(strlen(cwd) >= strlen(homedir) ){
                int len = strlen(homedir);
                char store[10000]="~";
                strcat(store,&cwd[len]);
                strcpy(dir,store);
            }
            else {
                strcpy(dir,cwd);
            }
        }
        return ;
    }
    else if(strcmp(tokens[1],"-") == 0){
        int check ;
        if(prevdir[0] == '~'){
            char store[10000] ;
            strcpy(store,homedir);
            if(strlen(prevdir) > 1)  strcat(store,&prevdir[1]);
            check = chdir(store);
        }
        else check = chdir(prevdir);
        if(check < 0){
            perror("Shell ");
        }
        else {
            strcpy(dir,prevdir);
            printf("%s\n",prevdir);
        }
        return ;
    }
    int check = chdir(tokens[1]);
    if(check < 0){
        perror("Shell ");
    }
    else{
        char cwd[10000];
        getcwd(cwd, sizeof(cwd));
        if(strlen(cwd) >= strlen(homedir) ){
            int len = strlen(homedir);
            char store[10000]="~";
            strcat(store,&cwd[len]);
            strcpy(dir,store);
        }
        else {
            strcpy(dir,cwd);
        }
    }
}