#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<signal.h> //for mycd
#include<fcntl.h>  //for mycd
#include"mysh.h"

#define clear() printf("\033[H\033[J")

char inDirArray[81];

void init_shell()
{
	clear();
	char* username = getenv("USER");
	getcwd(inDirArray, 81);
	printf("\n\nUSER is: @%s\n", username);
	sleep(1);
}

void print(char *string)
{
	printf("%s", string);
}

void help()
{
	printf("GROUP 13 SHELL HELP:\n");
	printf("List of commands supported:\n");
	printf("  - help\n");
	printf("  - mycat\n");
	printf("  - mycp\n");
	printf("  - myls\n");
	printf("  - mycd\n");
	printf("  - pwd\n");
	printf("  - cat\n");
	printf("  - cp\n");
	printf("  - ls\n");
	printf("Type quit, stop, or exit to quit\n\n");
	return;
}

void execute(char* command, int notreg)
{
	//separate command into arguments and set up for execvp.
	int argcount = 1;
	char** arguments;
	arguments = malloc((argcount + 1)* sizeof(char*));
	if(!arguments)
	{
		printf("Error allocating memory\n\n");
		return;
	}
	arguments[argcount - 1] = command;
//	printf("arguments[argcount -1] = %s\n\n", arguments[argcount - 1]);
	int n = strlen(command);
	for(int i = 0; i < n; i++)
	{
		if(command[i] == ' ')//if there is a space in command
		{
			command[i] = '\0';
			int j = i + 1;
			while( j < n && command[j] == ' ')
			{
				j++;
			}
			if(j != n) //if j isn't the end
			{
				argcount++;//then we have another agrument
				arguments = realloc(arguments, (argcount + 1) * sizeof(char*));//make space for argument
				if(!arguments)
				{
					printf("Error allocating memory\n\n");
					return;
				}
				arguments[argcount - 1] = &command[j]; //point to new argument
			}
			i = j-1; //continue looking for the next argument after that one.
		}
	}
//	for(int i = 0; i < argcount; i++)
//	{
//		printf("arguments[%d] = %s\n\n", i, arguments[i]);
//	}	
	arguments[argcount] = NULL;

	
	if(strstr(arguments[0], "mycd") && !strstr(arguments[0], "mycd.")) //if we are changing directory
	{
		changeDirectory(arguments);
		return;
	}

	char commandbuffer[100];
	if(notreg)
	{
		sprintf(commandbuffer, "%s/%s", inDirArray, arguments[0]);	
	}
	else
	{
		sprintf(commandbuffer, "%s", arguments[0]);
	}

	int pid;
	pid = fork();
	if(pid < 0)
	{
		printf("Error creating child\n\n");
		return;
	}
	if(pid == 0)
	{
		if(execvp(commandbuffer, arguments) == -1)
		{
			printf("Error executing %s\n\n", commandbuffer);
			exit(1);
		}
	}
	wait(NULL);
	free(arguments);
	return;
}

void executeinchild(char* command, int notreg)
{
	//separate command into arguments and set up for execvp.
	int argcount = 1;
	char** arguments;
	arguments = malloc((argcount + 1)* sizeof(char*));
	if(!arguments)
	{
		printf("Error allocating memory\n\n");
		exit(1);
	}
	arguments[argcount - 1] = command;
	int n = strlen(command);
	for(int i = 0; i < strlen(command); i++)
	{
		if(command[i] == ' ')//if there is a space in command
		{
			command[i] = '\0';
			int j = i + 1;
			while( j < n && command[j] == ' ')
			{
				j++;
			}
			if(j != n) //if j isn't the end
			{
				argcount++;//then we have another agrument
				arguments = realloc(arguments, (argcount + 1) * sizeof(char*));//make space for argument
				if(!arguments)
				{
					printf("Error allocating memory\n\n");
					return;
				}
				arguments[argcount - 1] = &command[j]; //point to new argument
			}
			i = j-1; //continue looking for the next argument after that one.
		}
	}
	arguments[argcount] = NULL;

	if(strstr(arguments[0], "mycd") && !strstr(arguments[0], "mycd.")) //if we are changing directory
	{
		changeDirectory(arguments);
		return;
	}

	char commandbuffer[100];
	if(notreg)
	{
		sprintf(commandbuffer, "%s/%s", inDirArray, arguments[0]);	
	}
	else
	{
		sprintf(commandbuffer, "%s", arguments[0]);
	}	

//	printf("executing %s\n\n", commandbuffer);
	
	if(execvp(commandbuffer, arguments) == -1)
	{
		printf("Error executing %s\n\n", commandbuffer);
		exit(1);
	}

	free(arguments);
	exit(0);
}

void getcommand(char * string)
{
	//check to see that string contains a valid command and if it does execute that command!
	//Valid commands are:
	//   - help   (in shell)
	//   - mycat
	//   - mycp
	//   - myls
	//   - mycd   (in shell)
	//   - pwd    (in shell)
	char* command;
	char* commandtest;
	command = strstr(string, "mycp");
	commandtest = strstr(string, "mycp.");
	if(command && !commandtest) //cp
	{
		execute(command, 1);
		return;
	}

	command = strstr(string, "myls");
	commandtest = strstr(string, "myls.");
	if(command && !commandtest) //ls
	{
		execute(command, 1);
		return;
	}

	command = strstr(string, "mycat");
	commandtest = strstr(string, "mycat.");
	if(command && !commandtest) //cat
	{
		execute(command, 1);
		return;
	}

	command = strstr(string, "help");
	commandtest = strstr(string, "help.");
	if(command && !commandtest) //help
	{
		help();
		return;
	}
	
	command = strstr(string, "mycd");
	commandtest = strstr(string, "mycd.");
	if(command && !commandtest) //cd
	{
		execute(command, 1);
		return;
	}

	command = strstr(string, "pwd");
	commandtest = strstr(string, "pwd.");
	if(command && !commandtest) //pwd
	{
		my_pwd();
		return;
	}

	command = strstr(string, "cp");
	commandtest = strstr(string, "cp.");
	if(command && !commandtest)
	{
		execute(command, 0);
		return;
	}

	command = strstr(string, "ls");
	commandtest = strstr(string, "ls.");
	if(command && !commandtest)
	{ 
		execute(command, 0);
		return;
	}

	command = strstr(string, "cat");
	commandtest = strstr(string, "cat.");
	if(command && !commandtest)
	{ 
		execute(command, 0);
		return;
	}
	
	printf("Error: no such command. Type help for list of commands\n\n");
	return;
}

void getcommandinchild(char* string)
{
	//check to see that string contains a valid command and if it does execute that command
	char* command;
	char* commandtest;
	command = strstr(string, "mycp");
	commandtest = strstr(string, "mycp.");
	if(command && !commandtest) //cp
	{
		executeinchild(command, 1);
		exit(0);
	}

	command = strstr(string, "myls");
	commandtest = strstr(string, "myls.");
	if(command && !commandtest) //ls
	{
		executeinchild(command, 1);
		exit(0);
	}

	command = strstr(string, "mycat");
	commandtest = strstr(string, "mycat.");
	if(command && !commandtest) //cat
	{
		executeinchild(command, 1);
		exit(0);
	}

	command = strstr(string, "help");
	commandtest = strstr(string, "help.");
	if(command && !commandtest) //help
	{
		help();
		exit(0);
	}
	
	command = strstr(string, "mycd");
	commandtest = strstr(string, "mycd.");
	if(command && !commandtest) //cd
	{
		executeinchild(command, 1);
		exit(0);
	}

	command = strstr(string, "pwd");
	commandtest = strstr(string, "pwd.");
	if(command && !commandtest) //pwd
	{
		my_pwd();
		exit(0);
	}
	
	command = strstr(string, "cp");
	commandtest = strstr(string, "cp.");
	if(command && !commandtest)
	{
		executeinchild(command, 0);
		exit(0);
	}

	command = strstr(string, "ls");
	commandtest = strstr(string, "ls.");
	if(command && !commandtest)
	{ 
		executeinchild(command, 0);
		exit(0);
	}

	command = strstr(string, "cat");
	commandtest = strstr(string, "cat.");
	if(command && !commandtest)
	{ 
		executeinchild(command, 0);
		exit(0);
	}

	printf("Error: no such command. Type help for list of commands\n\n");
	exit(1);
}

void runcommands(char* input)
{
	//break input into arguments
	//look to see how many commands we expect save places where there is a pipe and where there is a $
	int pipelocation = -1;
	int dollarlocation = -1;
	int numcommands = 1;
	//if input includes quit, stop, or exit then exit the program
	if(strstr(input, "quit") != NULL || strstr(input, "stop") != NULL || strstr(input, "exit") != NULL)
		exit(0);
	for(int i = 0; i < strlen(input); i++)
	{
		if(input[i] == '|')
		{
			numcommands += 1; //if there is a pipe we are expecting another command
			pipelocation = i;
		}
		if(input[i] == '$')
		{
			numcommands += 1; //if there is a $ we are expecting another command in ().
			dollarlocation = i;
		}
	}

	if(numcommands == 1) //if theres only one command then execute it
		getcommand(input);

	else if(pipelocation != -1)
	{
		//we only support one pipe, so throw an error if there are more than one pipes
		//split the input at pipe
		char* parta;
		char* partb;
		parta = &input[0];
		partb = &input[pipelocation + 1];
		parta[pipelocation] = '\0';
		mypip(parta, partb);
		return;
	}

	else if(dollarlocation != -1)
	{
		printf("I don't know how to deal with $ yet!\n\n");
		return;
	}
	
	return;

}

void runcommandsinchild(char* input)
{
	//break input into arguments
	//look to see how many commands we expect save places where there is a pipe and where there is a $
	int pipelocation = -1;
	int dollarlocation = -1;
	int numcommands = 1;
	for(int i = 0; i < strlen(input); i++)
	{
		if(input[i] == '|')
		{
			numcommands += 1; //if there is a pipe we are expecting another command
			pipelocation = i;
		}
		if(input[i] == '$')
		{
			numcommands += 1; //if there is a $ we are expecting another command in ().
			dollarlocation = i;
		}
	}

	if(numcommands == 1) //if theres only one command then execute it
		getcommandinchild(input);

	else if(pipelocation != -1)
	{
		//we only support one pipe, so throw an error if there are more than one pipes
		//split the input at pipe
		char* parta;
		char* partb;
		parta = &input[0];
		partb = &input[pipelocation + 1];
		parta[pipelocation] = '\0';
		mypip(parta, partb);
		exit(0);
	}

	else if(dollarlocation != -1)
	{
		printf("I don't know how to deal with $ yet!\n\n");
		exit(1);
	}
	
	exit(0);

}

void mypip(char* first, char* second)
{
	int firstpid, secondpid;
	int pp[2];
	if (pipe(pp) < 0)
	{
		printf("Error creating pipe.\n\n");
		return;
	}

	firstpid = fork();
        if (firstpid < 0)
	{
		printf("Error creating child\n\n");
		return;
	}
	if (firstpid == 0)
	{
//		printf("in child 1\n\n");
		dup2(pp[1], 1);
		close(pp[0]);
		close(pp[1]);
		runcommandsinchild(first);
		exit(0);
	}
	wait(NULL);

	secondpid = fork();
	if (secondpid < 0)
	{
		printf("Error creating child\n\n");
		return;
	}
	if (secondpid == 0)
	{
//		printf("in child 2\n\n");
		dup2(pp[0], 0);
		close(pp[0]);
		close(pp[1]);
		runcommandsinchild(second);
		exit(0);
	}
	wait(NULL);

//	printf("done executing pipe"); //when testing the pipe function, this doesn't print, but "executing ..." does as well as output.

	close(pp[0]);
	close(pp[1]);
	return;
}

int changeDirectory(char* args[]){
	// If we write no path (only 'cd'), then go to the home directory
	if (args[1] == NULL) {
		chdir(getenv("HOME")); 
		return 1;
	}
	// Else we change the directory to the one specified by the 
	// argument, if possible
	else{ 
		if (chdir(args[1]) == -1) {
			printf(" %s: no such directory\n", args[1]);
            return -1;
		}
	}
	return 0;
}

void my_pwd()
{
	char pwdArray[81];
	getcwd(pwdArray, 81);
	printf("%s\n", pwdArray);
}//end my_pwd

int main(int args, char** argv)
{
	char* input;
	size_t size = 500;
	input = malloc(size * sizeof(char)); //start with allowing 500 characters
	if(!input)
	{
		printf("Error allocating memory\n\n");
		return 1;
	}

	init_shell();

	while(1)
	{	
		print(getenv("USER"));
		printf(":");
		print(getcwd(NULL, 0));
		printf("$ ");

		//get user input
		size_t tempsize = 0;
		size = getdelim(&input, &tempsize, '\n', stdin); //put arguments into array
		input[size-1] = '\0';

//		test
//		printf("\n\n****%s****\n\n", input);

		runcommands(input);		
	}
	free(input);
	return 0;
}
