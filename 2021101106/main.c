#include "headers.h"
#include "shellprompt.h"
#include "token.h"
#include "history.h"
#include <ctype.h>
#include <termios.h>


char dir[1000];
char prevdir[1000] = "~";
char homedir[1000];
char dir_store[1000] = "";
int running_fgpid ;
int running ;

void die(const char *s)
{
    perror(s);
    exit(1);
}

struct termios orig_termios;

void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

/**
 * Enable row mode for the terminal
 * The ECHO feature causes each key you type to be printed to the terminal, so you can see what you’re typing.
 * Terminal attributes can be read into a termios struct by tcgetattr().
 * After modifying them, you can then apply them to the terminal using tcsetattr().
 * The TCSAFLUSH argument specifies when to apply the change: in this case, it waits for all pending output to be written to the terminal, and also discards any input that hasn’t been read.
 * The c_lflag field is for “local flags”
 */
void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        die("tcgetattr");
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}


void bg_func(int check)
{
    if (check == SIGCHLD)
        zombie_check(); // checks if process exits normally or abnormally
}

int main(int argc, char *argv[])
{
    int c = 1;
    time_t start, end;
    char timetaken[100], x[1000];
    strcpy(timetaken, "");
    char **commands;       // for storing info after seperated by semicolon
    char **piped_commands; // for storing info seperated by "|"
    int fildes[2];         // for storing the file_descriptors after using pipe (one for read and one for write)

    // to display that we entered the custom shell
    entershell();

    // getting the directories
    strcpy(dir, dirctry());
    getcwd(homedir, sizeof(homedir));

    // for creating a file "history.txt" only once
    FILE *history_file;
    char path[1000];
    strcpy(path, homedir);
    strcat(path, "/history.txt");
    if (history_file = fopen(path, "r"))
    {
        fclose(history_file);
    }
    else
    {
        history_file = fopen(path, "w");
        fclose(history_file);
    }
    // end

    // declaring the str variable
    extern char *str[21];
    for (int i = 0; i < 21; ++i)
    {
        str[i] = (char *)malloc(sizeof(str[i]));
    }
    // end

    while (1)
    {
        running = 0;
        int pt=0;
        x[0] = '\0'; // for making length of x is equal to zero so that previous xut is cleared out
        
        printf("<\033[1;32m%s\033[01;00m:\033[1;34m%s%s\033[01;00m> ", shellprompt(), dir, timetaken);
        strcpy(timetaken, "");
        int ctrld=scanf("%[^\n]s",x);
        getchar();
        if(ctrld==-1)
        {
            printf("\n");
            exit(0);
        }
        running =1 ;
        // if(!strcmp(x,EOF))
        // {
        //     exit(0);
        // }
        // this signal detects when the background process ends
        signal(SIGCHLD, bg_func);       // SIGCHLD is the number of signal sent to parent process when child terminates
        signal(SIGINT, ctrlc_handler);  // for control c
        signal(SIGTSTP, ctrlz_handler); // for control z

        // for storing the command typed so that it can be used for history
        store_info(x, homedir);

        // quit for exiting my custom terminal
        if (strcmp(x, "quit") == 0)
        {
            // exitshell();
            break;
        }

        strcpy(dir_store, dir);

        // calculating the time taken for foreground process . If it is more than 1 second then print it
        time(&start);
        tokeniseamp(x, dir, prevdir, homedir);
        time(&end);
        if (end - start >= 1)
        {
            int val = floor(end - start);
            sprintf(timetaken, "took %ds", val);
        }
        // end

        free(commands); // freeing the memory
    }
    return 0;
}