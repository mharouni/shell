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
		arguments = parseLine(line);
	//	printf("%s",**arguments);
		state = execCommand(arguments);
		free(line);
	 	free(arguments);
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
	pid_t pid, childPid, asyncPid ;
	int state;
	pid = fork();
	if (pid == 0)
	{
		if (!strcmp(getLastArg(arguments), "&"))
		{
			arguments[lastIndex(arguments)] = NULL;
		}
		//child process
		//printf("I'm a child");
		execvp(arguments[0] , arguments );
		
	}
	else if (pid < 0){
		exit(1);
	}
	else
	{
		if (!strcmp(getLastArg(arguments), "&"))
		{
			asyncPid = waitpid(pid, &state, WNOHANG);
		}
		else
		{
			do
				{
					childPid = waitpid(pid, &state, WUNTRACED);
				}
			while(!WIFEXITED(state) && !WIFSIGNALED(state));
		//	printf("I'm the parent");

		}
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
	
	else if  (!strcmp(arguments[0],"/bin/exit"))
		x = exitCommand();
	else
		x= createProcess(arguments);
	return x;
}
