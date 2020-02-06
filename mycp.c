// mycp
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
 
#define BUFFERSIZE 1024
#define COPYMORE 0644
 
void error(char *, char *);
int copyDir(char *src, char *dest);
int copyFiles(char *src, char *dest);
int check(char *filename);
int mode_isReg(struct stat info);
 
int main(int ac, char *av[])
{
   char *src = av[1];
   char *dest = av[2];
 
   if( src[0] != '/' && dest[0] != '/' ) // "./mycp sourceFile.txt destFile.txt"
   {
       copyFiles(src, dest);
   }
   else if( src[0] == '/' && dest[0] == '/' ) // "./mycp /SrcDir /DestDir"    
   {                        
      int i;
      for(i=1; i<=strlen(dest); i++)
      {
          dest[(i-1)] = dest[i];
      }
      for(i=1; i<=strlen(src); i++)
      {
          src[(i-1)] = src[i];
      }
      copyDir(src, dest);
   }
   else
   {
      fprintf(stderr, "usage: mycp source destination\n");
      exit(1);
   }
}
 
int copyDir(char *source, char *destination)
{
   DIR *dir_ptr = NULL;
   struct dirent *direntp;
   char tempDest[strlen(destination)+1];
   char tempSrc[strlen(source)+1];
   strcat(destination, "/");
   strcat(source, "/");
   strcpy(tempDest, destination);
   strcpy(tempSrc, source);
 
   if( (dir_ptr = opendir(source)) == NULL )
   {
      fprintf(stderr, "mycp: cannot open %s for copying\n", source);
      return 0;
   }
   else
   {
      while( (direntp = readdir(dir_ptr)))
      {      
         if(check(direntp->d_name))  
         {   
            strcat(tempDest, direntp->d_name);         
            strcat(tempSrc, direntp->d_name);
            copyFiles(tempSrc, tempDest);
            strcpy(tempDest, destination);
            strcpy(tempSrc, source);            
         }
      }
      closedir(dir_ptr);
      return 1;
   }
}
 
int check(char *filename)
{
    struct stat fileInfo;
     
    if(stat(filename, &fileInfo) >=0)
       if(S_ISREG(fileInfo.st_mode))
          return 1;
    else return 0;
}

int copyFiles(char *source, char *destination)
{
   int in_fd, out_fd, n_chars;
   char buf[BUFFERSIZE];
 
   // Open files
   if( (in_fd=open(source, O_RDONLY)) == -1 )
   {
      error("Cannot open ", source);
   }
 
   if( (out_fd=creat(destination, COPYMORE)) == -1 )
   {
      error("Cannot create ", destination);
   }
 
   // Copy files
   while( (n_chars = read(in_fd, buf, BUFFERSIZE)) > 0 )
   {
      if( write(out_fd, buf, n_chars) != n_chars )
      {
         error("Write error to ", destination);
      }
 
      if( n_chars == -1 )
      {
         error("Read error from ", source);
      }
   }

   // Close files
   if( close(in_fd) == -1 || close(out_fd) == -1 )
   {
      error("Error closing files", "");
   }
 
   return 1;
}
 
void error(char *s1, char *s2)
{
   fprintf(stderr, "Error: %s ", s1);
   perror(s2);
   exit(1);
}

