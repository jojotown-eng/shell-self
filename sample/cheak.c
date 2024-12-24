#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/wait.h>


int main(int argc,char **argv){

  char command[][20]={"echo","hello",">","output.txt"};
  
  

  return 0;
}

void redirect(char **argv){
	for(int i=0; argv[i]!=NULL; i++){
		if(strcmp(argv[i], ">") == 0){
			if(fork() == 0){
        int j=0;
				int fd = open(argv[i+1], O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
				dup2(fd, 1);
				close(fd);
				argv[i] = NULL;
        for(j=0; argv[i-j]!=NULL && (i-j)==0; j++)//リダイレクトの手前に必ずコマンドがある。
				if (execvp(argv[i-j], argv+i-j) < 0){
					perror("execvp");
					exit(-1);
				}
			}
		}
	}
}
