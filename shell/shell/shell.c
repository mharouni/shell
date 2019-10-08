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
		state = execCommand(arguments);
		
	}
	while (state);
	
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
	return argv;
}

int createProcess(char ** arguments)
{
	pid_t pid, waitPid ;
	int state;
	pid = fork();
	if (pid == 0)
	{
		//child process
		printf("I'm a child");
		execvp(arguments[0] , arguments );
		
	}
	else if (pid < 0){
		exit(1);
	}
	else
	{
		do
			{
				waitPid = waitpid(pid, &state, WUNTRACED);
			}
		while(!WIFEXITED(state) && !WIFSIGNALED(state));
		printf("I'm the parent");

	}
	return 1;
}int exitCommand()
{
	exit(0);
}
int execCommand( char ** arguments)
{
	int x;
	if (!strcmp(arguments[0],"exit"))
		x = exitCommand();
	else
		x= createProcess(arguments);
	return x;
}
