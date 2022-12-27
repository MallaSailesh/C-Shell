#include "headers.h"

void bold_blue(){
    printf("\033[1;34m");
    return ;
}
void bold_green(){
    printf("\033[1;32m");
}
void no_color(){
    printf("\033[m");
}

char  months[][10] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

int sorting(const struct dirent** d1, const struct dirent ** d2 )
{
    return strcasecmp((*d1)->d_name,(*d2)->d_name);
}

void getpermissions(mode_t file) { 
    char permissions[100]; 
    if(file && S_ISREG(file)) permissions[0]='-';  // for checking if it is a regular file or not 
    else if(file && S_ISDIR(file)) permissions[0]='d'; // for checking if it is a directory or not
    else if(file && S_ISCHR(file)) permissions[0]='c'; // for character device 
    else if(file && S_ISBLK(file)) permissions[0]='b'; // for block device 
    else if(file && S_ISFIFO(file)) permissions[0]='|'; // FIFO 
    else if(file && S_ISSOCK(file)) permissions[0]='s'; // socket?
    else if(file && S_ISLNK(file)) permissions[0]='l'; // S_ISLNK ( symbolic link)
    permissions[1] = (file & S_IRUSR) ? 'r':'-'; // user read permissions 
    permissions[2] = (file & S_IWUSR) ? 'w':'-'; 
    permissions[3] = (file & S_IXUSR) ? 'x':'-'; 
    permissions[4] = (file & S_IRGRP) ? 'r':'-'; // group read permissions 
    permissions[5] = (file & S_IWGRP) ? 'w':'-'; 
    permissions[6] = (file & S_IXGRP) ? 'x':'-'; 
    permissions[7] = (file & S_IROTH) ? 'r':'-'; // others read permissions
    permissions[8] = (file & S_IWOTH) ? 'w':'-'; 
    permissions[9] = (file & S_IXOTH) ? 'x':'-';  
    permissions[10] = '\0'; 
    printf("%s", permissions); 
}

int implement_ls(char token[] , int check_flaga, int check_flagl,char * homedir){
    
    struct dirent * file ; // so that we can use its functions 
    struct stat data_storage ;
    struct tm time ;
    struct passwd *pw ;
    struct group *grp ;
    int linkwidth = 0,linkval_max = 0 , sizewidth = 0 , sizeval_max = 0 ;
    char dir[1000];
    strcpy(dir,token);

    if (stat(dir, &data_storage) < 0) {
        perror("Error");
        return -1;
    }

    if(strcmp(dir,"~") == 0){
        strcpy(dir,homedir);
    }
    else {
        if(S_ISREG(data_storage.st_mode)){
            stat(dir, &data_storage);
            if(!check_flaga && dir[0] == '.') return 0;// should skip if flag a is not included
            if(!check_flagl){
                if( S_ISDIR(data_storage.st_mode) )
                bold_blue();
                else if(data_storage.st_mode & S_IXUSR)
                bold_green();

                printf("%s\n",dir);
                no_color();
                return 0;
            }
            getpermissions(data_storage.st_mode);
            
            pw = getpwuid(data_storage.st_uid);
            grp = getgrgid(data_storage.st_gid);
        
            printf(" %ld",data_storage.st_nlink); //number of link file has
        
            if (pw != 0) 
                printf(" %s\t", pw->pw_name); 
            else 
                printf(" %d", data_storage.st_uid); 

            if ( grp != 0) 
                printf(" %s\t", grp->gr_name); 
            else 
                printf(" %d", data_storage.st_gid); 

            printf(" %ld", data_storage.st_size); 
            localtime_r(&data_storage.st_mtime, &time); 
            printf(" %s %02d %02d:%02d",months[time.tm_mon],time.tm_mday,time.tm_hour, time.tm_min);
            if( S_ISDIR(data_storage.st_mode) )
                bold_blue();
            
            else if(data_storage.st_mode & S_IXUSR)
                bold_green();

            printf(" %s\n",dir);

            no_color();
            return 0;
        }
    }
    /*
        Now for directory 
    */
    if(chdir(dir)<0){
        perror("Error");
        return -1;
    }
    char dir2[1000];
    getcwd(dir2,sizeof(dir2));
    strcpy(dir,dir2);
    DIR * navigate_dir = opendir(dir); // DIR object for navigation through files 
    if(!navigate_dir){
        if(errno = ENOENT){
            //if the directory(here it is shown by dir) is not found 
            perror("Directory not found!");
            return -1;
        }
        else {
            // if it is not possible to read the directory(here it shown by dir)
            perror("Unable to read directory");
            return -1;
        }
    }
    char * temp4 = (char *)malloc(sizeof(char)*1000);
    getcwd(temp4,sizeof(temp4));
    struct dirent** sorted_files;
    int n = scandir(".", &sorted_files, NULL, sorting);

    if (n < 0) {
        perror("Error");
        return -1;
    }
    int total = 0;
    for(int i=0;i<n;++i){
        stat(sorted_files[i]->d_name, &data_storage);
        if(!check_flaga && sorted_files[i]->d_name[0] == '.') continue;// should skip if flag a is not included
        linkval_max = (linkval_max > data_storage.st_nlink) ? (linkval_max):data_storage.st_nlink;
        sizeval_max = (sizeval_max > data_storage.st_size) ? (sizeval_max):data_storage.st_size;
        total += data_storage.st_blocks;
    }
    while(linkval_max > 0){
        linkwidth++;
        linkval_max /= 10;
    }
    while(sizeval_max > 0){
        sizewidth++;
        sizeval_max /= 10;
    }
    if(check_flagl)printf("total %d\n",total/2);
    for(int i=0;i<n;++i){   
        stat(sorted_files[i]->d_name, &data_storage); 
        if(!check_flaga && sorted_files[i]->d_name[0] == '.') continue;// should skip if flag a is not included
        if(!check_flagl){
            if( S_ISDIR(data_storage.st_mode) )
            bold_blue();
            else if(data_storage.st_mode & S_IXUSR)
            bold_green();
            printf("%s\n",sorted_files[i]->d_name);
            no_color();
            continue;
        }
        getpermissions(data_storage.st_mode); 

        pw = getpwuid(data_storage.st_uid);
        grp = getgrgid(data_storage.st_gid);
        
        printf(" %*ld",linkwidth, data_storage.st_nlink); //number of link file has
        
        if (pw != 0) 
            printf(" %s\t", pw->pw_name); 
        else 
            printf(" %d", data_storage.st_uid); 

        if ( grp != 0) 
            printf(" %s\t", grp->gr_name); 
        else 
            printf(" %d", data_storage.st_gid); 

        printf(" %*ld",sizewidth, data_storage.st_size); 
        
        localtime_r(&data_storage.st_mtime, &time); 
        printf(" %s %02d %02d:%02d",months[time.tm_mon],time.tm_mday,time.tm_hour, time.tm_min);
        
        if( S_ISDIR(data_storage.st_mode) )
            bold_blue();
        else if(data_storage.st_mode & S_IXUSR)
            bold_green();

        printf(" %s\n", sorted_files[i]->d_name); 

        no_color();
        free(sorted_files[i]); 
    }
    free(sorted_files); 
    if(chdir(homedir)<0){
        perror("Error");
        return -1;
    }
}
void ls1(char * tokens[],int tokenscnt)
{
    int check_flaga = 0;
    int check_flagl = 0;
    
    char dir[1000] ;
    getcwd(dir,sizeof(dir));

    int no_of_arguments = 0;
    int argument = 0;
    for(int i=1;tokens[i]!=NULL;++i)
    {
        if(tokens[i][0] == '-'){
            if(tokens[i][1] == 'a' || ( strlen(tokens[i]) > 2 && tokens[i][2] == 'a') ) check_flaga = 1;
            if(tokens[i][1] == 'l' || ( strlen(tokens[i]) > 2 && tokens[i][2] == 'l') ) check_flagl = 1;
        }
        else {
            no_of_arguments ++;
            argument = i;
        }
    }    
    if(no_of_arguments == 0){
        implement_ls(dir , check_flaga,check_flagl,dir);
    }
    else if(no_of_arguments == 1){
        if(tokens[argument][0] == '~')
        {
            char * temp5 = (char *)malloc(sizeof(char)*1000);
            getcwd(temp5,1000);
            if(strlen(tokens[argument]) > 1)
            strcat(temp5,&tokens[argument][1]);
            // printf("%s\n",temp5);
            implement_ls(temp5 , check_flaga,check_flagl,dir);
        }
        else 
        implement_ls(tokens[argument] , check_flaga,check_flagl,dir);
        return ;
    }
    for(int i=1;tokens[i]!=NULL;++i)
    {
        if(tokens[i][0] == '-') continue;
        else if(tokens[i][0] == '~'){
            no_of_arguments--;
            char * temp6 = (char *)malloc(sizeof(char)*1000);
            getcwd(temp6,1000);
            if(strlen(tokens[argument]) > 1)
            strcat(temp6,&tokens[argument][1]);
            printf("%s:\n",temp6);
            implement_ls(temp6 , check_flaga,check_flagl,dir);
            if(no_of_arguments != 0)
            printf("\n");
        }
        else {
            no_of_arguments--;
            printf("%s:\n",tokens[i]);
            implement_ls(tokens[i],check_flaga,check_flagl,dir);
            if(no_of_arguments != 0)
            printf("\n");
        }
    }  
}

