#ifndef SHELL_H
#define MAX_ARGS 32
#define TOK " ,\t\n"

void mainLoop (void);
char * readLine(void);
int createProcess(char ** arguments);
char ** parseLine(char * line);
int execCommand(char ** arguments);
int exitCommand();

#endif //SHELL_H
