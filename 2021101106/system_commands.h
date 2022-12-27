#ifndef SYS_H
#define SYS_H
extern int running_fgpid;
extern int running ;
void jobs1(char * args[]);
int store_bg(int pid , char * arg);
void sig1(char * args[]);
void fg1(char * args[]);
void bg1(char * args[]);
void zombie_check();
void sys_commands(char * tokens[],int bg);

#endif