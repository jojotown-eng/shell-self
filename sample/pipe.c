#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

int main() {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    if(fork()==0){
			close(pipefd[0]);
			dup2(pipefd[1],STDOUT_FILENO);
			close(pipefd[1]);
			execvp("ls",(char *[]){"ls",NULL});
			perror("execlp ls");
      exit(1);
		}
		if(fork()==0){
			close(pipefd[1]);
			dup2(pipefd[0],STDIN_FILENO);
			close(pipefd[0]);
			int fd=open("output.txt", O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
			dup2(fd, 1);
			close(fd);
			execvp("grep",(char *[]){"grep","output",NULL});
		}

		close(pipefd[0]);
		close(pipefd[1]);

		wait(NULL);
		wait(NULL);  

    return 0;
}
