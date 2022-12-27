#include "headers.h"

void pwd1(){
    char cwd[10000];
    getcwd(cwd, sizeof(cwd));
    printf("%s\n", cwd);
}
