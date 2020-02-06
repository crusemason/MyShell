#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <ctype.h>

#define SIZE 1024
#define LENGTH 80
#define BUFSIZE 1000

int strcicmp(const char* a, const char *b) //case insensitive string comparison
{
	for(;; a++, b++)
	{
		int d = tolower(*a) - tolower(*b);
		if (d != 0 || !*a)
			return d;
	}
	return 0;
}

int ls(const char* name, bool option_f/*dont sort*/, bool option_l/*long listing format*/)
{
	struct stat namestat;
	stat(name, &namestat);

	if(!S_ISREG(namestat.st_mode) && !S_ISDIR(namestat.st_mode))
	{
		printf("myls: cannot access '%s': No such file or directory%c", name, '\n');
		return 1;
	}	

	if(S_ISREG(namestat.st_mode)) //if it is a regular file
	{
		if(option_l && !option_f)
		{
			printf("-"); //file is regular file
			//print permission stats
			if(namestat.st_mode & S_IRUSR)
				printf("r");
			else
				printf("-");
			if(namestat.st_mode & S_IWUSR)
				printf("w");
			else
				printf("-");
			if(namestat.st_mode & S_IXUSR)
				printf("x");
			else
				printf("-");
			if(namestat.st_mode & S_IRGRP)
				printf("r");
			else
				printf("-");
			if(namestat.st_mode & S_IWGRP)
				printf("w");
			else
				printf("-");
			if(namestat.st_mode & S_IXGRP)
				printf("x");
			else
				printf("-");
			if(namestat.st_mode & S_IROTH)
				printf("r");
			else
				printf("-");
			if(namestat.st_mode & S_IWOTH)
				printf("w");
			else
				printf("-");
			if(namestat.st_mode & S_IXOTH)
				printf("x");
			else
				printf("-");
			printf(" ");
			//print number of links
			printf("%ld", namestat.st_nlink);
			printf(" ");
			//print user that owns the file
			struct passwd* user;
			user = getpwuid(namestat.st_uid);
			printf("%s", user->pw_name);
			printf(" ");
			//print group that owns the file
			struct group* group;
			group = getgrgid(namestat.st_gid);
			printf("%s", group->gr_name);
			printf(" ");
			//print size in bytes
			printf("%ld", namestat.st_size);
			printf(" ");
			//print date of last modifycation
			printf("%.*s", (int)strlen(ctime(&(namestat.st_mtime)))-9, ctime(&(namestat.st_mtime)));
			printf(" ");
			//print name of file
			printf("%s%c", name, '\n');
		}
		else
			printf("%s%c", name, '\n');
	}

	else if(S_ISDIR(namestat.st_mode)) //if it is a directory
	{	
		DIR* directory;
		struct dirent* file;
		directory = opendir(name);
		if(!directory)//make sure we can open directory
		{
			printf("myls: cannot open directory  '%s'.\n", name);
			return 1;
		}

		char buffer[BUFSIZE];
		if(option_f) //do not sort, disable -l and enable -a
		{
			while((file = readdir(directory)) != NULL)//while there is a file
			{
				sprintf(buffer, "%s/%s", name, file->d_name);
        			printf(" %s\n", file->d_name);
			}
			closedir(directory);	
		}

		else //not option_f
		{
			char** entries; //array to hold stuff in directory so it can be sorted etc
			size_t entcount; //size of entries array
			int maxlength; //maxlenght of file names for nice output

			entries = malloc(SIZE * sizeof(char*));
			if(!entries)
			{
				printf("myls: memory cannot be allocated%c", '\n');
				return 1;
			}
		
			// get entries(list of files in directory)
			file = readdir(directory);
			entcount = 0;
			maxlength = 0; //to keep track of lenghts of file names for nice output
			while(file)//while there is a file in the directory
			{
				if(file->d_name[0] != '.') //if it isn't hidden
				{
					if(entcount >= SIZE)//if we need more space allocate it
					{
						entries = realloc(entries, sizeof(entries) * SIZE);
						if(!entries)
						{
							printf("myls: memory cannot be allocated%c", '\n');
							return 1;
						}
					}

					entries[entcount] = file->d_name; //add file to list
					//keep track of lengths for nice output
					if(strlen(file->d_name) > maxlength)
						maxlength = strlen(file->d_name);
					entcount++; 
				}

				file = readdir(directory);
			}
			
			if(maxlength < 1)
			{
				printf("myls: could not find files in directory\n");
				return 1;
			}

			//sort entries alphabetically
			char* temp;
			temp = malloc(maxlength * sizeof(char));
			if(!temp)
			{
			printf("myls: memory cannot be allocated%c", '\n');
				return 1;
			}
			bool bswtch = true;
			while(bswtch)
			{
				bswtch = false;
				for(int i = 0; i < entcount-1; i++)
				{
					if(strcicmp(entries[i], entries[i+1]) > 0)
					{
						bswtch = true;
						strcpy(temp, entries[i]);
						strcpy(entries[i], entries[i+1]);
						strcpy(entries[i+1], temp);
					}
				}
			}
			free(temp);

			//if long list format
			if(option_l)
			{
				int blocks = 0;

				for(int i = 0; i < entcount; i++)
				{
					sprintf(buffer, "%s/%s", name, entries[i]);
					stat(buffer, &namestat);
					blocks = blocks + namestat.st_blocks;
				}
				printf("total %d\n", blocks/2);
	
				for(int i = 0; i < entcount; i++)
				{
					sprintf(buffer, "%s/%s", name, entries[i]);
					stat(buffer, &namestat);

					if(S_ISREG(namestat.st_mode))
						printf("-"); //entry is a file
					else if(S_ISLNK(namestat.st_mode))
						printf("l"); //entry is a link
					else if(S_ISDIR(namestat.st_mode))
						printf("d"); //entry is a directory	
					else
						printf("?");
						
					if(namestat.st_mode & S_IRUSR)
						printf("r");
					else
						printf("-");
					if(namestat.st_mode & S_IWUSR)
						printf("w");
					else
						printf("-");
					if(namestat.st_mode & S_IXUSR)
						printf("x");
					else
						printf("-");
					if(namestat.st_mode & S_IRGRP)
						printf("r");
					else
						printf("-");
					if(namestat.st_mode & S_IWGRP)
						printf("w");
					else
						printf("-");
					if(namestat.st_mode & S_IXGRP)
						printf("x");
					else
						printf("-");
					if(namestat.st_mode & S_IROTH)
						printf("r");
					else
						printf("-");
					if(namestat.st_mode & S_IWOTH)
						printf("w");
					else
						printf("-");
					if(namestat.st_mode & S_IXOTH)
						printf("x");
					else
						printf("-");
					printf(" ");
					//print number of links
					printf("%ld", namestat.st_nlink);
					printf(" ");
					//print user that owns the file
					struct passwd* user;
					user = getpwuid(namestat.st_uid);
					printf("%s", user->pw_name);
					printf(" ");
					//print group that owns the file
					struct group* group;
					group = getgrgid(namestat.st_gid);
					printf("%s", group->gr_name);
					printf(" ");
					//print size in bytes
					printf("%6ld", namestat.st_size);
					printf(" ");
					//print date of last modifycation
					printf("%.*s", (int)strlen(ctime(&(namestat.st_mtime)))-9, ctime(&(namestat.st_mtime)));
					printf(" ");
					//print name of file
					printf("%s%c", entries[i], '\n');
				}
			}

			else //not long list format
			{
				if(LENGTH/maxlength <= 1) //one column of output
				{
					for(int i = 0; i < entcount; i++) //traverse entries 
					{
						printf("%s\n", entries[i]);
					}
				}
				else //multiple columns of output.
				{
					//how many columns?
					int columns = LENGTH/(maxlength+1);
					//how many rows?
					int rows = (entcount/columns) + 1;

					if(rows==1)
					{
						for(int j = 0; j < entcount; j++)
							printf("%s  ", entries[j]);
					}

					else
					{
						for(int i = 0; i < rows; i++) //for each row
						{
							for(int j = 0; j < entcount; j++)//for each entry
							{
								if(i == j%(rows)) //if entry belongs on that row
								{
									printf("%-*s  ", maxlength, entries[j]); //print entry
								}
							}
							printf("%c", '\n');
						}
					}	
					printf("%c", '\n');
				}
			}

			free(entries);
		}
	}

	else //it is not a file or a directory.
	{
		printf("myls: cannot access '%s': No such file or directory%c", name, '\n');
		return 1;
	}

	return 0;
}

int main(int args, char** argv)
{
	int numoptions = 0;  //holds the number of options(arguemtents starting with -) passed to the function

	bool option_f = false;  //if true do not sort
	bool option_h = false;	//if true display help and exit
	bool option_l = false;	//if ture use the long listing format
	char* help = "--help";

	//set options
	for(int i = 1; i < args; i++) //while there is an argument
	{
		if(argv[i] == help)
		{
			option_h = true;
		}

		else if(argv[i][0] == '-') //if it is an option
		{
			numoptions++;  //count how many arguments are options
			for(int j = 1; j < strlen(argv[i]); j++) //what option is it?
			{
				if(argv[i][j]=='f')
					option_f = true;
				else if(argv[i][j]=='h')
					option_h = true;
				else if(argv[i][j]=='l')
					option_l = true;
				else //if that option is not supported
				{
					printf("myls: invalid option -- '%c'%cTry 'myls -h' for more information.%c", argv[i][j], '\n', '\n');
					return 1;
				}
			}
		}
	}//done setting options

	if(option_h) //displays help and exits
	{
		printf("Usage: myls [OPTION]... [FILE]...%c", '\n');
		printf("List informations about the FILEs (the current directory by default).%c", '\n');
		printf("Sort entries alphabetically by default.%c", '\n');
		printf("%c", '\n');
		printf("Options: %c", '\n');
		printf("  -f          do not sort, disable -l and show hidden files%c", '\n');
		printf("  -h, --help  displays this help and exit%c", '\n');
		printf("  -l          use a long listing format%c", '\n');
		printf("%c", '\n');
		printf("myls will not distinguish file types with color.%c", '\n');
		printf("%c", '\n');
		printf("Exit status:%c 0  if Okay,%c 1  if trouble.%c", '\n', '\n', '\n');
		printf("%c", '\n');
		printf("This myls function was written by Trinity Vassberg.%c", '\n');

		return 0; 
	}

	//get file or directory name(s)
	if((args-1)==numoptions) //no file or directory name(s) given: defult is current dirrectory
		ls(getcwd(NULL, 0), option_f, option_l);

	else if((args-2)==numoptions) //only one file or directory name
	{
		for(int i = 1; i < args; i++)
		{
			if(argv[i][0] != '-') //find the file or directory name
				ls(argv[i], option_f, option_l);
		}
	}
	 
	else //two or more file or directory name(s)
	{
		int count = 0;
		for(int i = 1; i < args; i++) //while there is an argument
		{
			if(argv[i][0] != '-') //if it is not an option
			{
				if(count != 0)
					printf("%c", '\n');
				printf("%s:%c", argv[i], '\n');
				ls(argv[i], option_f, option_l);
				count++;			
			}	
		}
	}

	return(0);
}

