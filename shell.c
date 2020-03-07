/***************************************************************************//**
  @file         shell.c
  @author       GeonHee Cho
  @date         Thursday,  22 Febrary 2018
  @brief        The Cpre 308 project 1, making shell file
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int split_by_space(char* str, char** res) {
    int i = 0;
    char* p = strtok(str, " ");
    while (p != NULL) {
        res = realloc (res, sizeof (char*) * ++i);
        if (res == NULL)
            exit(-1); //memory allocation failed
        res[i-1] = p;
        p = strtok (NULL, " ");
    }
    return i;
}


/**
 * This is the method to handle user input 
 * Included with our project required commands 
 */
void Built_In_CMD(char** args, int args_size) 
{
    if(args_size == 0){}

    /* builtin commands without arguments */
    else if(strncmp("exit", args[0], 4) == 0) {exit(0); } //exit
    else if(strncmp("pid", args[0], 3) == 0) { printf("%d\n", getpid());}  //pid
    else if(strncmp("ppid", args[0], 4) == 0) { printf("%d\n", getppid());} //ppid
    else if(strncmp("pwd", args[0], 4) == 0) 
    { //pwd 
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) == NULL) 
	{
            printf("Error \n");
            exit(-1);
        }
        printf("%s\n", cwd);
    }

    else if(strncmp("cd", args[0], 2) == 0) 
    { //cd 
        if(args_size == 1) {
            chdir(getenv("HOME"));
        } else if(args_size == 2) {
            if(chdir(args[1]) == -1) {
                printf("No such file or directory\n");
            }
        } else {
            printf("A lot of arguments\n");
        }
    }
    else if(strncmp("get", args[0], 3) == 0) { //get
        if(args_size == 1) {
            printf("Specify the variable\n");
        } else if(args_size == 2) {
            if(getenv(args[1]) == NULL) {
                printf("This variable doesn't exist\n");
            } else {
                printf("%s\n", getenv(args[1]));
            }
        } else {
            printf("Too many arguments\n");
        }
    }
    else if(strncmp("set", args[0], 3) == 0) { //set 
        if(args_size == 1) {printf("Specify\n");
        } 
	else if(args_size == 2) 
	{
            if(unsetenv(args[1]) == -1) {printf("Cannot clear the variable\n");
            }
        } 
	else if(args_size == 3) 
	{
            if(setenv(args[1], args[2], 1) == -1) { printf("Cannot set this variable\n");
            }
        } else { printf("A lot of arguments \n");
        }
    }

    else 
     {
        int background = 0; //check background
        if(strncmp("&", args[args_size-1], 1) == 0) 
	{ 
            background = 1;
            args[args_size-1] = NULL; //remove "&"" from args
        } 

        int child = fork();
        if(child > 0) { //in parent process
            usleep(1);
            int status = -1;
            
            if(background) 
	    {
            } 
	    else
	    {
                waitpid(child, &status, 0); //command block
                printf("[%d] %s Exit %d\n", child, args[0], WEXITSTATUS(status));
            }
        } 
	else if(child == 0) { //in child process
            printf("[%d] %s\n", getpid(), args[0]);
            execvp(args[0], args);
            exit(-1);
        } else {
            printf("fork error\n");
        }
    }
    int status = -1; //check the child
    int pid = waitpid(-1, &status, WNOHANG);
    while(pid > 0) 
    {
        printf("[%d] %s Exit %d\n", pid, args[0], WEXITSTATUS(status));
        pid = waitpid(-1, &status, WNOHANG);
    }
}


int main(int argc, char **argv) 
{
    char* prompt = "308sh> ";
    char input[100];
    
    if(argc != 3 && argc != 1)  // shell
	{ 
     	      printf("Invalid argument\n");
       	      return -1;
  	  } 
	else if(argc == 3)
	 {
       		 if(strlen(argv[1]) != 2 || strncmp("-p", argv[1], 2) != 0) 
		{
                 	  printf("Invalid argument\n");
          		  return -1;
        }
        prompt = strcat(argv[2], "> ");
    }

    while(1) 
	{
        printf("%s", prompt); //use input
        fgets(input, 100, stdin);
        if ((strlen(input) > 0) && (input[strlen(input) - 1] == '\n')) 
	{
            input[strlen(input) - 1] = '\0';
        }
        char** args = malloc(sizeof(char**));
        int args_size = split_by_space(input, args);
        Built_In_CMD(args, args_size);
        free(args);
    }
}
