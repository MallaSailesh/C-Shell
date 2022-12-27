#include "headers.h"

int echo1(char * tokens[]) {
  for(int i=1;tokens[i]!=NULL;++i)
  printf("%s ",tokens[i]);
  printf("\n");
  return 0;
}
