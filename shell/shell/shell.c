#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "shell.h"
void mainLoop (void)
{
	char * line;
	char **arguments;
	int state = 1;
	do {
		printf("shell >> ");
		line = readLine();
		if(line == NULL)
			continue;
		arguments = parseLine(line);
	//	printf("%s",**arguments);
		state = execCommand(arguments);
		free(line);
	 	free(arguments);
		fflush(stdin);
	}
	while (state);
//free(line);
//free(argumrnts);
	
}
char * readLine(void)
{
	char * buffer = NULL;
	size_t bufferSize = 0;
	getline(&buffer, &bufferSize, stdin);
	return buffer;
}

char ** parseLine(char * line)
{
	
	char ** argv = malloc(MAX_ARGS * sizeof(char *));
	int index = 0;
	char * token;
	while((	token = strtok_r(line, TOK, &line)))
	{
		argv[index] = token;
		index++;
	}
	while (index <= MAX_ARGS)
	{
		argv[index] = NULL;
		index ++;
	}
	return argv;
}
char * getLastArg(char ** arguments)
{
	char * arg;
	int i =0;
	arg = arguments[i];
	while (arg != NULL)
	{
		i++;
		arg = arguments[i];
	}
	return arguments[i-1];
}
int lastIndex(char ** arguments)
{
		char * arg;
		int i =0;
		arg = arguments[i];
		while (arg != NULL)
		{
			i++;
			arg = arguments[i];
		}
		return i-1;
}

int createProcess(char ** arguments)
{
	FILE * file;
	file = fopen("log.txt", "w");
	fprintf(file, "PID\tSIGNUM\n");
	fclose(file);
	file = fopen("log.txt", "a");
	
	pid_t pid, childPid, asyncPid;
	int state;
	int background=0;
	pid = fork();
	if (!strcmp(getLastArg(arguments), "&"))
	{
		background =1;
		arguments[lastIndex(arguments)] = NULL;
		
	}
	
	if (pid == 0)
	{
		execvp(arguments[0] , arguments );
		
	}
	else if (pid < 0){
		exit(1);
	}
	else
	{
		if (background)
		{
			asyncPid = waitpid(pid, &state, WNOHANG);
			
		}
		else
		{
		do
			{
					childPid = waitpid(pid, &state, WUNTRACED);
				printf("%d\n",state);
			}
			while(!WIFEXITED(state) && !WIFSIGNALED(state));
			
		//
			
			
		}
		fprintf(file, "%d\t%d",childPid,state);
	}
	return 1;
}int exitCommand()
{
	exit(0);
}
int execCommand( char ** arguments)
{
	int x;
	if (!strcmp(arguments[0], ""))
		x=1;
	
	else if  (!strcmp(arguments[0],"exit"))
		x = exitCommand();
	else
		x= createProcess(arguments);
	return x;
}
