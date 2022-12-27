#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <dirent.h>
#include <string.h>

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

/**
 * stdout and stdin are buffered we disable buffering on that
 * After entering in raw mode we read characters one by one
 * Up arrow keys and down arrow keys are represented by 3 byte escape codes
 * starting with ascii number 27 i.e. ESC key
 * This way we interpret arrow keys
 * Tabs are usually handled by the term, but here we are simulating tabs for the sake of simplicity
 * Backspace move the cursor one control character to the left
 * @return
 */
int main()
{
    char *inp = malloc(sizeof(char) * 100);
    char c;
    while (1)
    {
        setbuf(stdout, NULL);
        enableRawMode();
        printf("Prompt>");
        memset(inp, '\0', 100);
        int pt = 0;
        while (read(STDIN_FILENO, &c, 1) == 1)
        {
            if (iscntrl(c))
            {
                if (c == 10)
                    break;
                else if (c == 27)
                {
                    char buf[3];
                    buf[2] = 0;
                    if (read(STDIN_FILENO, buf, 2) == 2)
                    { // length of escape code
                        printf("\rarrow key: %s", buf);
                    }
                }
                else if (c == 127)
                { // backspace
                    if (pt > 0)
                    {
                        if (inp[pt - 1] == 9)
                        {
                            for (int i = 0; i < 7; i++)
                            {
                                printf("\b");
                            }
                        }
                        inp[--pt] = '\0';
                        printf("\b \b");
                    }
                }
                else if (c == 9)
                { // TAB character
                    // inp[pt++] = c;
                    // for (int i = 0; i < 8; i++) { // TABS should be 8 spaces
                    //     printf(" ");
                    // }
                    char t[1000] = {'\0'};
                    char fil[40][100];
                    int len = 0;
                    // strcpy(t,inp);
                    int i = strlen(inp) - 1;
                    while (t[i] != 0)
                    {
                        i--;
                    }
                    for (; i < strlen(inp); i++)
                    {
                        t[len++] = inp[i];
                    }
                    struct dirent *file;
                    file = readdir(".");

                    int nofiles = 0;
                    while (file != NULL)
                    {
                        if (!strncmp(t, file->d_name, strlen(t)))
                        {
                            memset(fil[nofiles], 0, 100);
                            strcpy(fil[nofiles++], t);
                        }
                        file = readdir(".");
                    }
                    if (nofiles == 1)
                    {
                        for(int i=strlen(t);i<strlen(fil[0]);i++)
                        {
                            printf("%c",fil[i]);
                            inp[pt++]=fil[i];
                        }
                    }
                    else if(nofiles>1)
                    {
                        printf("\n");
                        for(int i=0;i<nofiles;i++)
                        {
                            printf("%s\n",fil[i]);
                        }
                    }
                }
                else if (c == 4)
                {
                    exit(0);
                }
                else
                {
                    printf("%d\n", c);
                }
            }
            else
            {
                inp[pt++] = c;
                printf("%c", c);
            }
        }
        disableRawMode();

        printf("\nInput Read: [%s]\n", inp);
    }
    return 0;
}