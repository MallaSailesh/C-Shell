#include "headers.h"

int discover_exe(char target_dir[] , char search_file[] , int f_flag , int d_flag, char homedir[], char path[]){
    
    struct stat data_storage;
    struct dirent *file ;

    DIR * navigate_dir = opendir(target_dir); // DIR object for navigation through files 
    if(!navigate_dir){
        return 0;
    }
    while ((file = readdir(navigate_dir)) != NULL) {
        char temp3[1000] ;
        sprintf(temp3,"%s/%s",target_dir,file->d_name);
        int check = stat(temp3, &data_storage);
        if(file->d_name[0] == '.') continue;

        if(check == 0 & S_ISREG(data_storage.st_mode)){
            if(strcmp("",search_file) == 0){
                if(f_flag){
                    printf("%s/%s\n",path,file->d_name);
                }
            }
            else{
                if(strcmp(search_file,file->d_name) == 0 && f_flag){
                    printf("%s/%s\n",path,file->d_name);
                }
            }
            continue;
        }
        char temp[1000];
        strcpy(temp,path);
        strcat(temp,"/");
        strcat(temp,file->d_name);
        if(strcmp(search_file,file->d_name) == 0 || d_flag)
        printf("%s/%s\n",path,file->d_name);
        char temp2[1000];
        strcpy(temp2,target_dir);
        strcat(temp2,"/");
        strcat(temp2,file->d_name);
        discover_exe(temp2,search_file,f_flag,d_flag,homedir,temp);

    }
    return 0;
}

void discover1(char * tokens[],int tokenscnt){
    struct stat data_storage;
    int f_flag = 0,  d_flag = 0,check = 0;
    char search_file[1000] = "",target_dir[1000] = "",dir_pre[1000] = "";
    char path[1000];
    
    getcwd(dir_pre,sizeof(dir_pre));

    for(int i=1;i<tokenscnt;++i){
        if(tokens[i][0] == '-'){
            if(strcmp(tokens[i],"-f") == 0) f_flag = 1;
            else if(strcmp(tokens[i],"-d") == 0) d_flag = 1;
            else {
                printf("Error! Invalid flag!!\n");
                return ;
            }
        }
        else if(tokens[i][0] == '"'){
            for(int j=1;j<strlen(tokens[i])-1;++j){
                search_file[j-1] = tokens[i][j];
            }
        }
        // else if(strcmp(tokens[i],"~") == 0){
        //     check = 1;
        //     strcpy(target_dir,dir_pre);
        //     strcpy(path,".");
        // }
        else if(tokens[i][0] == '~'){
            check = 1;
            strcpy(target_dir,dir_pre);
            strcpy(path,".");
            if(strlen(tokens[i]) > 1)
            {
                strcat(target_dir,&tokens[i][1]);
                strcat(path,&tokens[i][1]);

            }
        }
        else{
            check = 1;
            strcpy(target_dir,dir_pre);
            strcpy(target_dir,"/");
            strcpy(target_dir,tokens[i]);
            strcpy(path,tokens[i]);
        }
    }
    if(!f_flag && !d_flag) {f_flag = 1 ; d_flag = 1;}
    if(strcmp(search_file,"") != 0) d_flag = 0;
    if(check == 0) {
        strcpy(target_dir,dir_pre);
        strcpy(path,".");
    }
    // printf("%s\n",target_dir);

    if(stat(target_dir,&data_storage)<0){
        perror("Error");
        return ;
    }

    if(d_flag)
    printf("%s\n",path);
    // printf("%d %s %s %d %d %s %s\n",check,target_dir,search_file,f_flag,d_flag,dir_pre,path);
    discover_exe(target_dir,search_file,f_flag,d_flag,dir_pre,path);
}