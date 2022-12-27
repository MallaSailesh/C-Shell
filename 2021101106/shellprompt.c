#include "headers.h"


char * shellprompt(){
    char * value = (char *)malloc(sizeof(char));
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid(uid);
    value = pw->pw_name ;
    strcat(value,"@");
    char name[100]  = {};
    gethostname(name,100);
    strcat(value,name);
    return value ;
}

char * dirctry(){
    char * dir = (char *)malloc(sizeof(char)*1000);
    dir = "~";
    return dir;
}

void exitshell(){
    printf("\t\t\t\t\033[01;91m-----------\n");
    printf("\t\t\t\t\033[01;91m--- Bye ---\n");
    printf("\t\t\t\t\033[01;91m-----------\n");
}

void entershell(){
    printf("\t\t\t\033[01;91m----------------------------\n");
    printf("\t\t\t\033[01;91m--- Malla Sailesh's Shell --\n");
    printf("\t\t\t\033[01;91m------- 2021101106 ---------\n");
    printf("\t\t\t\033[01;91m----------------------------\033[01;00m\n");
}