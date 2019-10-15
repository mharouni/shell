
#ifndef SHELL_H
#define MAX_ARGS 32 // Maximun number of arguments that can be passed to the shell, with the first argument being the command itself.
#define TOK " ,\t\n" // the delimeters that are used to split the given string into multiple arguments.

void mainLoop (void); // Initialises the program, contains the main loop of the shell. 	Called by the main function.
char * readLine(void); // Responsible for reading the command from stdin and returns a character pointer to the first element of the command. Called by the main loop
int createProcess(char ** arguments); // Responsible for froking the exisiting process and checking the arguments if the process is to be run in  the background and handling background execution. Also, responsible for executing the given command in the child process. Responsible for handling failed forking. called in execCommand.
char ** parseLine(char * line); // Takes in the command passed by the user as an argument and splits the command into an array of arguments to be readable by the exec system call.
int execCommand(char ** arguments); // handles empty commands, built-in commands and the cereation of childen by calling createProcess.
int exitCommand(void); // handles the exit command as a built in function of the shell.
int changeDir(char * dir);// handles the cd ( change directory) command as a built-in function of the shell.
char * getLastArg(char ** arguments); // Returns a pointer to the last argument in the array of arguments.
int lastIndex(char ** arguments); // Returns the index of the last argument in the array of arguments.
void handleSignal(int sig); // customised signal handler used to log the terminated processes into the log file.

#endif //SHELL_H
