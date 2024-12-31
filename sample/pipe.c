#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

int main() {
	
	char *argv[]={"ls","|","grep","output",">","output.txt",NULL};
	int is_redirect=0;
	int *pipe_locate=NULL;
	pipe_locate=(int*)malloc(sizeof(int));
	pipe_locate[0]=-1;
	int pipe_count=0;
	
	for(int i=0;argv[i]!=NULL;i++){
		if(strcmp(argv[i],">")==0){
			is_redirect=i;
			argv[i]=NULL;
		}else if(strcmp(argv[i],"|")==0){
			pipe_count++;
			pipe_locate=realloc(pipe_locate,(pipe_count+1) * sizeof(int));
			pipe_locate[pipe_count]=i;
			argv[i]=NULL;
		}
	}

	int pipefd[pipe_count+1][2];

	if(pipe_count==0 && is_redirect==0){
		if(fork()==0){
			if(execvp(argv[0],argv)){
				perror("execvp");
				exit(EXIT_FAILURE);
			}		
		}
	}else if(is_redirect!=0 && pipe_count==0){
		if(fork()==0){
			int fd=open(argv[is_redirect+1], O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
			dup2(fd, 1);
			close(fd);
			if(execvp(argv[0],argv)){
				perror("execvp");
				exit(EXIT_FAILURE);
			}
		}
	}else{
		for(int i=0;i<pipe_count+1;i++){
			if(i!=pipe_count){
				pipe(pipefd[i]);
			}
			if(fork()==0){
				if(i==0){
					close(pipefd[i][0]);
					dup2(pipefd[i][1],STDOUT_FILENO);
					close(pipefd[i][1]);
				}else if(i==pipe_count){
					dup2(pipefd[i-1][0],STDIN_FILENO);
					close(pipefd[i-1][0]);
					if(is_redirect){
						int fd=open(argv[is_redirect+1], O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
						dup2(fd, 1);
						close(fd);
					}
					close(pipefd[i-1][1]);
				}else{
					close(pipefd[i][0]);
					close(pipefd[i-1][1]);
					dup2(pipefd[i-1][0], 0);
					dup2(pipefd[i][1], 1);
					close(pipefd[i][1]);
					close(pipefd[i-1][0]);
				}
				if(execvp(argv[pipe_locate[i] + 1], argv + pipe_locate[i] + 1) < 0){
					perror("execvp");
					exit(EXIT_FAILURE);
				}
			}
		}
	}
	for(int i=0;i<pipe_count;i++){
		wait(NULL);
	}

	return 0;
}
