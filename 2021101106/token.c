#include "headers.h"

int running = 0;
int running_fgpid;
char * fg_comm ;

void ctrlc_handler(){

}
void ctrlz_handler(){
    // if(running == 1){
            store_bg(running_fgpid,fg_comm);
            // running = 0;
			// printf("[%d]  %d\n",index_pro,pid);
}

char ** piping(char * text){
    char ** commands = malloc(1024 * sizeof(char *)); // 1024 is used because while allocating memory boundaries are clearly maintained 
    if(!commands)
	{
        fprintf(stderr, "Allocation Error \n");// printing the error to stderr 
        exit(EXIT_FAILURE);
    }
    int i ;
    char * line ;
    line = strtok(text , "|"); // for seperating commands with delimiter "|"
    for(i = 0;line != NULL ; i++){
        commands[i] = line ;
        line = strtok(NULL,"|");
    }
    commands[i] = line;
    free(line);

    return commands ;
}

void tokeniseamp(char * text, char * dir , char * prevdir , char * homedir){
    
    char temp[1000]; strcpy(temp,text);
    char * tokens[1000];
    int ampi = 0;
    tokens[0] = strtok(text,"&");

    for(ampi = 0;;ampi++){
        if(tokens[ampi] == NULL)
        break;
        else tokens[ampi+1] = strtok(NULL,"&");
    }
    if(ampi <= 1){
        int len = strlen(temp);
        while(temp[len-1]==' ') len--;
        if( temp[len-1] =='&' )     tokenise_sc(tokens[0],1,dir,prevdir,homedir);
        else    tokenise_sc(tokens[0],0,dir,prevdir,homedir);
    }
    else{
        for(int i=0;i<ampi;++i){
            if( (i+1)!=ampi )   tokenise_sc(tokens[i],1,dir,prevdir,homedir);
            else if(temp[strlen(temp)-1] == '&')  tokenise_sc(tokens[i],1,dir,prevdir,homedir);
            else tokenise_sc(tokens[i],0,dir,prevdir,homedir);
        }
    }
}

void tokenise_sc(char * text ,int bg, char * dir , char * prevdir, char * homedir)
{
    char * tokens[10000];
    int si=0;
    tokens[0] = strtok(text,";");    
    for(si=0;;++si){
        if(tokens[si]==NULL){
            break;
        }
        else tokens[si+1] = strtok(NULL,";");  
    }
    for(int i=0;i<si;++i)
        tokenise_space(tokens[i],bg,dir,prevdir,homedir);
}

void tokenise_space(char * text ,int bg, char * dir , char * prevdir, char * homedir)
{
    char ** comm_af_piping = piping(text);
    int piping_args = 0;
    for(piping_args=0;comm_af_piping[piping_args]!=NULL;piping_args++);
    int pipefds[2][2];
    for(int j = 0;j<piping_args;++j)
    {
        char * tokens[10000]; 
        char * args[1000];
        int si=0 , no_arg = 0;
        int stdinclone_fd , stdoutclone_fd  ;
        int ip_redir = 0 , op_redir = 0 ;// flags opredir == 1 for write == 2 for append
        char * read_file = NULL;
        char * write_file = NULL;
        int fd_r = 0, fd_w =0 ;
        tokens[0] = strtok(comm_af_piping[j]," \t");    
        for(si=0;;++si){
            if(tokens[si]==NULL){
                break;
            }
            else tokens[si+1] = strtok(NULL," \t");  
        }
        if(piping_args > 1){
            if(pipe(pipefds[j%2])< 0){
                perror("Error");
                break;
            }
            if(j > 0) fd_r = pipefds[1-j%2][0];//read from the previous one 
            if(j<piping_args-1) fd_w = pipefds[j%2][1];// write into the present one 
        }
        args[0] = NULL;
        for(int i = 0 ; tokens[i]!=NULL;++i){
            if(ip_redir){
                fd_r = open(read_file,O_RDONLY);
                ip_redir = 0;
            }
            else if(op_redir){
                fd_w = open(write_file,O_WRONLY|O_CREAT|(op_redir == 1 ? O_TRUNC:O_APPEND),0644);
                op_redir = 0;
            }
            else if(strcmp(tokens[i],"<") == 0) {
                ip_redir = 1;
                if(tokens[i+1] == NULL){
                    perror("Syntax Error");
                    exit(0);
                }
                else read_file = tokens[i+1];
            }
            else if(strcmp(tokens[i],">") == 0) {
                op_redir = 1;
                if(tokens[i+1] == NULL){
                    perror("Syntax Error");
                    exit(0);
                }
                else write_file = tokens[i+1];
            }
            else if(strcmp(tokens[i],">>") == 0) {
                op_redir = 2;
                if(tokens[i+1] == NULL){
                    perror("Syntax Error");
                    exit(0);
                }
                else write_file = tokens[i+1];
            }
            else if(tokens[i][0] != '\0'){
                args[no_arg] = malloc(sizeof(char)*(strlen(tokens[i])+1));
                strcpy(args[no_arg++],tokens[i]);
                args[no_arg] = NULL;
            }
        }
        
        if(fd_r < 0 || fd_w < 0){
            perror("Error\n");
            continue;
        }

        if(fd_r){
            stdinclone_fd = dup(STDIN_FILENO);
            dup2(fd_r,STDIN_FILENO);
        }
        if(fd_w){
            stdoutclone_fd = dup(STDOUT_FILENO);
            dup2(fd_w,STDOUT_FILENO);
        }
        running = 1;
        if(strcmp(args[0],"cd") == 0){
            char store[10000] = "";
            strcpy(store,dir);
            cd1(prevdir,dir,args,no_arg,homedir);
            strcpy(prevdir,store);
        }
        else if(strcmp(args[0],"jobs") == 0){
            jobs1(args);
        }
        else if(strcmp(args[0],"sig") == 0){
            if(no_arg != 3){
                perror("Invalid number of arguments");
                continue ;
            }
            sig1(args);
        }
        else if(strcmp(args[0],"fg") == 0){
            if(no_arg != 2 ){
                perror("Invalid number of arguments");
                continue;
            }
            fg1(args);
        }
        else if(strcmp(args[0],"bg") == 0){ 
            if(no_arg != 2 ){
                perror("Invalid number of arguments");
                continue;
            }
            bg1(args);
        }
        else if(strcmp(args[0],"pwd") == 0){
            pwd1();
        }
        else if(strcmp(args[0],"clear") == 0){
            printf("\e[1;1H\e[2J");
        }
        else if(strcmp(args[0],"echo") == 0){
            echo1(args);
        }
        else if(strcmp(args[0],"ls") == 0){
            ls1(args,no_arg);
        }
        else if(strcmp(args[0],"pinfo") == 0){
            pinfo1(args,no_arg,homedir);
        }
        else if(strcmp(args[0],"history") == 0){
            history1(homedir);
        }
        else if(strcmp(args[0],"discover") == 0){
            discover1(args,no_arg);
        }
        else {
            running = 1;
            fg_comm = malloc(sizeof(char)*1000);
            strcpy(fg_comm,args[0]);
            // running_pid = getpid();
            sys_commands(args,bg);
            running = 0;
        }
        // running = 0;
        if(fd_r){
            close(fd_r);
            dup2(stdinclone_fd,STDIN_FILENO);
            close(stdinclone_fd);
        }
        if(fd_w){
            close(fd_w);
            dup2(stdoutclone_fd,STDOUT_FILENO);
            close(stdoutclone_fd);
        }
        if(piping_args > 1){
            if(j > 0)  close(pipefds[1-j%2][0]);
            if(j<piping_args-1) close(pipefds[j%2][1]);
        }
    }
    return ;
}