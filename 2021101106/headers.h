#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include<fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include<signal.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<dirent.h> // for handling directory files 
#include <grp.h> 
#include <time.h> 
#include <locale.h> 
#include<sys/dir.h>
#include "ls.h"
#include "system_commands.h"
#include "pinfo.h"
#include "history.h"
#include "discover.h"
#include "echo.h"
#include "pwd.h"
#include "shellprompt.h"
#include "changedir.h"
#include "token.h"
#include "discover.h"
#include "time.h"
#include<math.h>

