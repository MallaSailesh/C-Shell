#include "headers.h"

char *str[21];

int no_of_commands(char *path)
{
    FILE * history_file = fopen(path, "r");
    int cnt = 0;
    // for (char c = getc(history_file); c != EOF; c = getc(history_file))
    while (fscanf(history_file, "%[^\n]s", str[cnt]) != EOF)
    {
        cnt++;
        if (fgetc(history_file) == EOF)
            break;
    }
    fclose(history_file);
    return cnt;
}

void store_info(char *text, char *homedir)
{
    char path[1000];
    strcpy(path, homedir);
    strcat(path, "/history.txt");
    
    // FILE *history_file = fopen(path,"w");
    // fclose(history_file);

    int commands_cnt = no_of_commands(path);
    
    if (commands_cnt > 0 && strcmp(text, str[commands_cnt-1]) == 0) return;
    FILE * history_file = fopen(path, "w");
    if(!history_file) perror("Error") ;
    
    if (commands_cnt == 20)
    {
        for (int i = 0; i < 19; ++i)
            strcpy(str[i], str[i + 1]);
        strcpy(str[19], text);
        for (int i = 0; i < 20; ++i)
        {
            fprintf(history_file, "%s\n", str[i]);
        }
    }
    else
    {
        strcpy(str[commands_cnt], text);
        for (int i = 0; i <= commands_cnt; ++i)
        {
            fprintf(history_file, "%s\n", str[i]);
        }
    }

    fclose(history_file);
}

void history1(char * homedir)
{
    char path[1000];
    strcpy(path, homedir);
    strcat(path, "/history.txt");
    int cnt = no_of_commands(path);
    if(cnt < 10){
        for(int i=0;i<cnt;++i)
            printf("%s\n",str[i]);
    }
    else {
        for(int i=cnt-10;i<cnt;++i)
            printf("%s\n",str[i]);
    }
}
