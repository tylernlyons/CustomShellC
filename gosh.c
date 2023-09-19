#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/sysmacros.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>

//reads the input entered on the command line
int cmdLine(char* string) {
    char* buff;

    buff = readline("");
    if(strlen(buff) != 0) {
        strcpy(string,buff);
        return 0;
    } else {
        return 1;
    }
}

//parses the nonpiped arguments
void nPArgs(char** input)
{
    pid_t  pid = fork();
    if (pid == -1) {
        printf("\nFork failed\n");
        return;
    } else if (pid == 0) {
        if(execvp(input[0],input) < 0) {
            printf("\nCommand failed\n");
        }
        exit(0);
    } else {
        wait(NULL);
        return;
    }
}

//parses the piped arguments
void pArgs(char** input, char** pInput)
{
    int pipeDes[2];
    pid_t pid1;
    pid_t pid2;

    if(pipe(pipeDes) < 0) {
        printf("\nPipe error\n");
        return;
    }
    pid1 = fork();
    if(pid1 < 0) {
        printf("\nFork failed\n");
        return;
    }
    if(pid1 == 0) {
        close(pipeDes[0]);
        dup2(pipeDes[1], STDOUT_FILENO);
        close(pipeDes[1]);

        if(execvp(input[0],input) < 0) {
            printf("\nCommand failed\n");
            exit(0);
        }
    } else {
        pid2 = fork();
        if(pid2 < 0) {
            printf("\nFork failed\n");
            return;
        }
        if(pid2 == 0) {
            close(pipeDes[1]);
            dup2(pipeDes[0],STDIN_FILENO);
            close(pipeDes[0]);


            if(execvp(pInput[0],pInput) < 0) {
                printf("\nCommand failed\n");
                exit(0);
            }
        } else {
            close(pipeDes[1]);
            wait(NULL);
            wait(NULL);
        }
    }
}

//generates the prompt to print the current directory
void wDir(char* cwd)
{
    char* token = strtok(cwd, "/");
    char* tmp;
    while(token != NULL) {
        tmp = token;
        token = strtok(NULL, "/");
    }
    printf("gosh@%s> ",tmp);
}

//prints built in help command
void help()
{
    printf("\nCommands available: \n>cd\n>ls\n>exit\n>support for pipes\ngeneral UNIX commands\n");
    return;
}

//handles the built in commands of the unix shell
int builtInHandler(char** input)
{
    int nCmds = 3;
    char* lCmds[nCmds];
    int switchN = 0;

    lCmds[0] = "cd";
    lCmds[1] = "exit";
    lCmds[2] = "help";

    for(int i = 0; i < nCmds; i++) {
        if(strcmp(input[0], lCmds[i]) == 0) {
            switchN = i + 1;
            break;
        }
    }

    switch(switchN) {
    case 1:
        chdir(input[1]);
        return 1;
    case 2:
        exit(0);
    case 3:
        help();
        return 1;
    default:
        break;
    }
    return 0;
}

//parses the actual pipes themselves
int pPipe(char* string, char** sPiped)
{
    for(int i = 0; i < 2; i++) {
        sPiped[i] = strsep(&string, "|");
        if(sPiped[i] == NULL) {
            break;
        }
    }
    if(sPiped[1] == NULL) {
        return 0;
    } else {
        return 1;
    }
}

//parses the command line for spaces
void pCmd(char* string, char** input)
{
    for(int i = 0;i < 15; i++) {
        input[i] = strsep(&string, " ");
        if(input[i] == NULL) {
            break;
        }
        if(strlen(input[i]) == 0) {
            i--;
        }
    }
}

//initializes the command line commands and their argumentsn
int intCmd(char* string, char** input, char** pInput)
{
    char* sPiped[2];
    int piped = 0;
    piped = pPipe(string,sPiped);

    if(piped) {
        pCmd(sPiped[0],input);
        pCmd(sPiped[1],pInput);
    } else {
        pCmd(string,input);
    }
    if(builtInHandler(input)) {
        return 0;
    } else {
        return 1 + piped;
    }
}

int main()
{
    char input[250];
    char* npArgs[100];
    char* pargs[100];
    int notice = 0;



    while(1) {
        char* cwd = getenv("PWD");
        wDir(cwd);

        if(cmdLine(input)) {
            continue;
        }
        notice = intCmd(input,npArgs,pargs);

        if(notice == 1) {
            nPArgs(npArgs);
        }
        if(notice == 2) {
            pArgs(npArgs,pargs);
        }
    }
    return 0;
}
