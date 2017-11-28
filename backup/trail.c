#include <unistd.h>
#include <stdio.h>
#include <errno.h>

int main() {
   char cwd[1024];
   if (getcwd(cwd, sizeof(cwd)) != NULL)
       fprintf(stdout, "Current working dir: %s\n", cwd);
   else
       perror("getcwd() error");
	char pwd[1024];   
	snprintf(pwd,sizeof(pwd),"%s%s",cwd,"/clientInfo.txt");
	fprintf(stdout,"file is %s\n",pwd);
   return 0;
}