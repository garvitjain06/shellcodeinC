#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include<sys/wait.h>
#include<fcntl.h>
#define STORAGE_SPACE 256
#define DELIMETER " \t\r\n\a"

int cdir(char **args)
{
    if (args[1] == NULL)
    {
       printf("myshell: expected argument to \"cd\"\n");
    }
    else
    {
       if (chdir(args[1]) != 0)
       {
             printf("myshell invalid command");
       }
    }
  return 1;
}

int help()
{
    printf("Garvit Jain myshell\n");
    printf("The built in function are as follows in:\n");
    printf("1) help\n");
    printf("2) cdir\n");
    printf("3) quit\n");
    printf("The built in operation are as follows in:\n");
    printf("1) inputRedirection\n");
    printf("2) outputRedirection\n");
    printf("3) createPipe\n");
    printf("4) Multitasking\n");
}

int quit(){ exit(0); }

char *read_line()
{
    int readyQueue = STORAGE_SPACE,index = 0,c;
    char *buffspace = malloc(sizeof(char) * readyQueue);
    while (1)
    {
        c = getchar();
        if (c == EOF || c == '\n')
        {
            buffspace[index] = '\0';
            return buffspace;
        }
        else
        {
            buffspace[index] = c;
        }
        index++;
   }
}

int execute(char **args)
{
    int i=fork();
    if(i== 0)
    {
            if(execvp(args[0], args)<0)
            {
                printf("myshell: command not found\n");
            }
             exit(0);
    }
    else
    {
            wait(NULL);
    }

}

void  multitasking(char **args)
{
   int pid = fork();
   if(pid == 0)
    {
      args[1]=NULL;
      if(execvp(args[0], args)<0)
      {
         printf("myshell: command not found\n");
      }
      exit(0);
    }
}

void pipeline(char **args)
{
    int i,fd,pfd[2],j;
    i=fork();
    if(i==0)
    {
        pipe(pfd);
        j=fork();
        if(j==0)
        {   args[1]=NULL;
            close(1);
            dup(pfd[1]);
            close(pfd[1]);
            close(pfd[0]);
            execvp(args[0],args);
        }
        else
        {
            args[1]=NULL;
            close(0);
            dup(pfd[0]);
            close(pfd[0]);
            close(pfd[1]);
            execvp(args[2],args);
        }
    }
    else
    {
            wait(NULL);
    }
}

void inputRedirect(char **args)
{
    int fd;
    int pid=fork();
        if(pid==0)
        {   args[1]=NULL;
			fd = open(args[2], O_RDONLY);
			close(0);
            dup(fd);
			execvp(args[0],args);
		}
		else
        {
            close(fd);
            wait(NULL);
        }
}

void outputRedirect(char **args){
    int fd;
    int pid=fork();
    if(pid==0)
    {
        args[1]=NULL;
        fd = open(args[2], O_WRONLY | O_TRUNC | O_CREAT, 0666);
        close(1);
        dup(fd);
        execvp(args[0],args);
    }
    else
    {
        close(fd);
        wait(NULL);
    }
}

void  main()
{
    char *line,**args;
    while(1)
    {
        printf("GarvitJain@myshell#");
        line=read_line();
        if(strlen(line)==0){continue;}
        int readyQueue = STORAGE_SPACE, index = 0;
        char **ts = malloc( sizeof(char) *readyQueue);
        char *t;
        t = strtok(line, DELIMETER);
        while (t != NULL)
        {
            ts[index] = t;
            index++;
            t = strtok(NULL, DELIMETER);
        }
        ts[index] = NULL;
            if(index>1)
            {
                if(!strcmp(ts[1],"&"))
                {
                    multitasking(ts);
                }
                else if(!strcmp(ts[1],">"))
                { outputRedirect(ts);
                }
                else if(!strcmp(ts[1],"<"))
                {
                    inputRedirect(ts);
                }
                else if(!strcmp(ts[1],"|"))
                {
                    pipeline(ts);
                }
                else
                {
                     execute(ts);
                }
            }
            else
            {
                if(!strcmp(ts[0],"help"))
                {
                    help();
                }
                else if(!strcmp(ts[0],"cdir"))
                {
                    cdir(ts);
                }
                else if(!strcmp(ts[0],"quit"))
                {
                    quit();
                }
                else
                {
                     execute(ts);
                }
            }
    }
}

