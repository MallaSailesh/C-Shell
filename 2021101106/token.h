#ifndef _TOKEN_H
#define _TOKEN_H

char ** piping(char * text);
void tokeniseamp(char * text , char *dir , char * prevdir , char * homedir );
void ctrlc_handler();
void ctrlz_handler();
void tokenise_sc(char * text,int bg , char * dir , char * prevdir , char * homedir);
void tokenise_space(char * text ,int bg ,char * dir , char * prevdir,char * homedir);
#endif