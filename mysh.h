void init_shell();                       //initialize shell for use

void print(char *string);                //print string

void execute(char* command, int);             //takes char* command and splits it up into char*array holding each argument individually then 
void executeinchild(char* command, int);      //calls execvp to execute the command. the int is 0 if the command is regular cat, ls, or cp.

void getcommand(char * string);          //takes char* string and determins if it is a command and if it is, how that command should be
void getcommandinchild(char* string);    //handled.

void runcommands(char* input);		 //takes char* input and checks for pipes and $() and handles those cases.
void runcommandsinchild(char* input);

void mypip(char* first, char* second);   //takes care of pipe

//commands included in shell

int changeDirectory(char* agrs[]);

void my_pwd ();
