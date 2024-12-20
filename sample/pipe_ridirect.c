#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define WRITE 1
#define READ 0

int main(void){
  int pipefd[2];
  if (pipe(pipefd)<0){
    perror("pipe");
    exit(EXIT_FAILURE);
  }

  pid_t pid=fork();
  if(pid<0){
    perror("pid");
    exit(EXIT_FAILURE);
  }
  if(pid==0){//子プロセスの処理
    close(pipefd[WRITE]);
    
  }
  return 0;
}