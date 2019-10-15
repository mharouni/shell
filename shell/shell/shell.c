#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

FILE * file; // file pointer to initialise and write in the log file.
void handleSignal(int sig)
{
	pid_t child;
	file = fopen("log.txt", "a");
	child = waitpid(-1, NULL, WNOHANG);// returns the pid of the terminated process if successful, 0 if information about the terminated child is not avaliable and -1 if unsuccessful.
	if (child > 0)
		fprintf(file, "%d\t%d\n",child,sig); //logs the Pid of the terminated child is avaliable, along witht the signal number sent by the child (SIGCHILD is the only signal handled).
	else
		fprintf(file, "A child was terminated\n"); // prints this statement if pid is not avalaible happens if child isn't alive for long (observed during testing).
	fclose(file);
}
void mainLoop (void)
{
	signal(SIGCHLD, handleSignal);// signal funtion called with signal type to be handled and the handling function which was previously defined in the code.
	file = fopen("log.txt", "w");
	fprintf(file, "PID\tSIGNUM\n");//init the log file
	fclose(file);
	char * line; // pointer to the string read by the program, passed to parseLine to be parsed into an array of arguments.
	char **arguments; // pointer to the arrat of arguments passed to the execCommand to act as arguments for exec system call.
	int state = 1; // acts as the condition that controls the loop, exit returns 0 to exit the loop.
	do {
		printf("shell >> "); //prompt indicating that the shell is ready to take in commands.
		line = readLine();
		//if(line == NULL)
		//	continue; // if condition to handle buffer issue that arises with pressing return. the command is passed along with a Null command that causes a null pointer exception.
		arguments = parseLine(line);
		state = execCommand(arguments);
		free(line);
	 	free(arguments);
	}
	while (state);
}
char * readLine(void)
{
	char * buffer = NULL;
	size_t bufferSize = 0;// init with 0 to allow getline to set it to the size of the string reead dynamically.
	getline(&buffer, &bufferSize, stdin); // reads a line from stdin and uses buffer to point to the firsr element, uses buffersize as an indicator to the max number of characters allowed. if buffer size is equal to 0, getline doesnt have a maximum input limit and stores the number of characters inputed in buffersize.
	return buffer;
}
char ** parseLine(char * line)
{
	char ** argv = malloc(MAX_ARGS * sizeof(char *));//init an array of pointers to point to the arguments after parsing. Max_Args previously defined with 32.
	int index = 0; // index to itterate on the previously declared array.
	char * token; // pointer used to point to the first token when tokenising the input string
	while((	token = strtok_r(line, TOK, &line)))// tokenizing the string... strtok_r is a modified strtok that renter the function as long as the string to be tokeniszed is not null, can be done with strtok and a loop instead.
	{
		argv[index] = token;
		index++;
		
	}
	while (index <= MAX_ARGS)// this loop assigns the unused elements in the array to point to null to aid in getLastArg() and lastIndex().
	{
		argv[index] = NULL;
		index ++;
	}
	return argv; // returns the ready array of pointers.
}
char * getLastArg(char ** arguments)
{
	char * arg; // temp pointer to be used in the loop.
	int i =0; // index for the loop
	arg = arguments[i];
	while (arg != NULL)// loop that iterates on the array of arguments and terminates when it hits an argument with null value
	{
		i++;
		arg = arguments[i];
	}
	return arguments[i-1]; // return a pointer to the last argument in the array of arguments
}


int lastIndex(char ** arguments) // Same logic as getLastArg(), however, it returns the index as an int instead of the element with the index.
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
	pid_t pid, childpid;
	int state ;
	int background=0;// flag used to indicate background execution.
	pid = fork(); // duplicates the current process and passes thr id of the child process to the parent, 0 to the child, returns a negative value if it failed.
	if (!strcmp(getLastArg(arguments), "&"))// first step for handling background execution, if the condition is true, the flag is set and the "&" is removed from the array of arguments so it doesnt interfere with the command functionality.
	{
		background =1;
		arguments[lastIndex(arguments)] = NULL;
	}
	if (pid == 0)//child process
	{
		execvp(arguments[0] , arguments );// passes the array of arguments previously parsed to the execvp command which executes the command. This block is in the child process.
	}
	else if (pid < 0){// this means there was an error while forking, an error message is printed to the user to indicate this.
		printf("Error forking\n");
	}
	else // parent process
	{
		if (background) // if the background flag is set due to the existance of an & at the end of the command, the parent process does nothing, it doesnt check for the status of the child. It continues execution, waiting for the next command.
		{
		
		}
		else // if the background flag is not set, the parent process calls the following funtion, which waits until info on the process with the pid passed in the first argument is obtained, if it returns before the process is terminated the do while loop recalls waitpid until it exits normally or because of an uncaught signal. this information is obtained throught the wait macros used to analyze the value given in the variable state by waitpid.
		{
			do
			{
				childpid = waitpid(pid, &state, WUNTRACED);
			}while (!WIFEXITED(state) && !WIFSIGNALED(state));// this logical is true (nonzero) if the child process is not terminated either normally or due to a signal.
			//the loop recalls the wait pid funtion until the parent learns that the child has been terminated.
			
			
		}
			
	}
	return 1;// returns 1 to the main loop so the condition for the main loop remains true.
}



int exitCommand()//Built in command to handle the command "exit".
{
	printf("Exiting\n");
	exit(0);// exit system call.
}


int changeDir(char * dir)// built in command to handle "cd arg".
{
	if (dir == NULL)// in case the cd command is called alone, it returns to the root directory.
	{
		chdir("/");
	}
	else
		{
		char * path = malloc(300 * sizeof(char));// declare a pointerthat points to a reserved 300 character array.
		getcwd(path, 300);//assign path to point to the current working directory.
		strcat(path, "/");// concatenate a "/" to prepare the inputed directory to be concatenated.
		strcat(path, dir);//path now points tp the readily built new path and ready to be passed to chdir.
		chdir(path);
		}
		return 1;
}


int execCommand( char ** arguments)
{
	int x;
	if (arguments[0] == NULL) // if no command is entered, nothing happens.
		x=1;
	
	else if  (!strcmp(arguments[0],"exit"))// if the command is the built in command "exit", the exit function is called to handle it.
		x = exitCommand();
	
	else if (!strcmp(arguments[0], "cd"))//if the built in command cd is called, the changeDir function is called to handle it
			x = changeDir(arguments[1]);
	
	
	else // if the command entered is neither built-in nor empty, the create process is called to fork a child process and execute the given command.
		x= createProcess(arguments);
	return x;
}
