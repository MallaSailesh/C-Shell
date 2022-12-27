#include "headers.h"

struct bg_jobs{
    char * bg_processes;
    int bg_pid;
}bg_jobs[1000];

int cmp(const void * s1 , const void * s2){
    const struct bg_jobs * x = s1;
    const struct bg_jobs * y = s2;
    // if(x->bg_pid == 0 || y-> )
    // return strcmp(x->bg_processes,y->bg_processes) ;
}


int store_bg(int pid , char * arg){
    int index_st ;
    for(int i=0;i<1000;++i){
        if(bg_jobs[i].bg_pid != 0) continue ;
        bg_jobs[i].bg_pid = pid ;
        // printf("a\n");
        bg_jobs[i].bg_processes = (char *)malloc(sizeof(char)*(1+strlen(arg)));
        strcpy(bg_jobs[i].bg_processes , arg);
        index_st = i+1;
        break;
    }
    return index_st;
}

void remove_bg(int pid){
    if(pid == 0) return ;
    for(int i=0;i<1000;++i){
        if(bg_jobs[i].bg_pid == pid) { bg_jobs[i].bg_pid = 0;break; }
    }
}

void jobs1(char * args[]){
    // qsort(bg_jobs,1000,1000,cmp);
    int r_flag = 0 , s_flag = 0;
    for(int i = 1;args[i]!=NULL;++i){
        if(strcmp(args[i],"-r") == 0) r_flag = 1;
        else if(strcmp(args[i],"-s") == 0) s_flag = 1;
    }
    if(r_flag == 0 && s_flag == 0) {r_flag = 1; s_flag = 1;}
    for(int i=0;i<1000;++i){
        if(bg_jobs[i].bg_pid == 0) continue;
        char data_file[1000],state;
        sprintf(data_file,"/proc/%d/stat",bg_jobs[i].bg_pid);
        FILE* file = fopen(data_file, "r");
        if(!file)    {
            perror("Error");
            return ;
        }
        fscanf(file, "%*d %*s %c", &state);

        if(s_flag && state == 'T')
        printf("[%d] %s %s [%d]\n", i+1, "Stopped" ,bg_jobs[i].bg_processes, bg_jobs[i].bg_pid);
        else if(r_flag && state != 'T')
        printf("[%d] %s %s [%d]\n", i+1, "Running" ,bg_jobs[i].bg_processes, bg_jobs[i].bg_pid);
        fclose(file);
    }
}

void sig1(char * args[]){
    int val1 = atoi(args[1]);
    int val2 = atoi(args[2]);
    if(bg_jobs[val1-1].bg_pid == 0){
        perror("No job exist");
        return ;
    }
    kill(bg_jobs[val1-1].bg_pid,val2);
}

void fg1(char * args[]){
    int val1 = atoi(args[1]);
    if(bg_jobs[val1-1].bg_pid == 0){
        perror("No job exist");
        return ;
    }
    val1=bg_jobs[val1-1].bg_pid;
    int status ;
    setpgid(val1,getpgid(0));// getpgid is the group id of the process . getpgid(0) is the group id of the calling process(i.e the process running in terminal)
    // so i am making the group id of val1(pid of some bg process) as that of getpgid(0)
    signal(SIGTTIN, SIG_IGN);// for not writing it into stdin
    signal(SIGTTOU,SIG_IGN);//for not writing it into stdin
    tcsetpgrp(0, val1); // sets the val1 into stdin
    if(kill(val1,SIGCONT)<0) { perror("Error 2.0"); } // now sending a signal if stopped then make it running so SIGCONT used 
    waitpid(val1, &status, WUNTRACED);// wait until the process runs in background 
    tcsetpgrp(0, getpgid(0));// set the stdin to group id of calling process  
    signal(SIGTTIN, SIG_DFL);// disable the above signals
    signal(SIGTTOU, SIG_DFL);

    if (WIFSTOPPED(status)) return ;
    else if(WIFEXITED(status)) remove_bg(val1) ;
    else return remove_bg(-val1);// pid is negative if wait for any child process whse process group id is equal to absolute value of pid
}

void bg1(char  * args[]){
    int val1 = atoi(args[1]);
    if(bg_jobs[val1-1].bg_pid == 0){
        perror("No job exist");
        return ;
    }
    val1=bg_jobs[val1-1].bg_pid;
    if(kill(val1,SIGCONT)<0) { perror("Error 2.0"); }
}

void zombie_check() {
    int status ,pid ;
    pid = waitpid(-1,&status,WNOHANG);// allows the parent process to obtain status information of a child process
    if(pid > 0){
        int index,check_normal = 0 ;
        for(int i=0;i<1000;++i){
            if(bg_jobs[i].bg_pid == pid) {index = i; break;}
        }
        if(WIFSIGNALED(status)) check_normal = 0;
        else if(WIFSTOPPED(status)) return ;
        else if(WIFEXITED(status)) check_normal = 1;
        if(check_normal == 1)   printf("\n%s with pid %d exited normally\n",bg_jobs[index].bg_processes, pid);
        else if(check_normal == 0) printf("\n%s with pid %d exited abnormally\n",bg_jobs[index].bg_processes, pid);
        bg_jobs[index].bg_pid = 0;
        // free(bg_processes[index]);
    }
}

void sys_commands(char * args[],int bg){
	int status ;
	pid_t pid ;
	pid = fork();
    if(pid < 0){
        perror("Error(forking)"); // Error in forking 
    }
    else if(pid == 0){
        // this is a child process
		int child_pid = getpid(); 
		if(bg){
			setpgid(0,0);// it puts the process pid into process group  pgid
		}    
        if(execvp(args[0],args) == -1){
            perror("Invalid Command");
        }
        exit(EXIT_FAILURE);// enters here only if execvp fails otherwise replaced by the command to be executed 
    }
	// parent process
	else {  
		// background process
		if(bg){
			// cnt_bg++;
            int index_pro = store_bg(pid,args[0]);
			printf("[%d]  %d\n",index_pro,pid);
		}
		// foreground process
		else {
            running_fgpid=pid;
			waitpid(pid,NULL,0);
		}
	}
}
