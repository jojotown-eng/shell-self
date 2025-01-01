#include "function.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define BUFSIZE  1024
#define ARGVSIZE 100

extern const char whitespace[];

void runcmd(char *buf)
{
	char *argv[ARGVSIZE];
	
	memset(argv, 0, ARGVSIZE);//値をすべて0にする
	if (parsecmd(argv, buf, &buf[strlen(buf)]) > 0)//コマンド実行
	if(strcmp(argv[0],"cd")==0){
		cd(argv);
	}else{
		PipeRedirect(argv);
	}
}

void cd(char **argv){
	char path[BUFSIZE];
	getcwd(path,BUFSIZE);
	if(strstr(argv[1],"/")==NULL){
		strcat(path,"/");
		strcat(path,argv[1]);
	}
	if(chdir(path)==-1){
		printf("no file\n");
		exit(1);
	}
}

void PipeRedirect(char **argv){
	int is_redirect=0;
  int is_oppose_redirect=0;
	int *pipe_locate=NULL;
	pipe_locate=(int*)malloc(sizeof(int));
	pipe_locate[0]=-1;
	int pipe_count=0;
	
	for(int i=0;argv[i]!=NULL;i++){
		if(strcmp(argv[i],">")==0){
			is_redirect=i;
			argv[i]=NULL;
		}else if(strcmp(argv[i],"<")==0){
      is_oppose_redirect=i;
      argv[i]=NULL;
    }else if(strcmp(argv[i],"|")==0){
			pipe_count++;
			pipe_locate=realloc(pipe_locate,(pipe_count+1) * sizeof(int));
			pipe_locate[pipe_count]=i;
			argv[i]=NULL;
		}
	}

	int pipefd[pipe_count+1][2];

	if(pipe_count==0 && is_redirect==0 && is_oppose_redirect==0){
		if(fork()==0){
			if(execvp(argv[0],argv)){
				perror("execvp");
				exit(EXIT_FAILURE);
			}		
		}
	}else if(is_oppose_redirect!=0){
    if(fork()==0){
      if(is_FileOrDir(argv[is_redirect+1])==0){
        exit(1);
      }
      if(is_FileOrDir(argv[is_oppose_redirect+1])==0){
        exit(1);
      }
      int fd_in=open(argv[is_oppose_redirect+1],O_RDONLY);
      dup2(fd_in,STDIN_FILENO);
      close(fd_in);
      if(is_redirect!=0){
        int fd_out=open(argv[is_redirect+1], O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        dup2(fd_out,STDOUT_FILENO);
        close(fd_out);
      }
      if(execvp(argv[0],argv)){
				perror("execvp");
				exit(EXIT_FAILURE);
			}
    }
  }else if(is_redirect!=0 && pipe_count==0){
		if(fork()==0){
      if(is_FileOrDir(argv[is_redirect+1])==0){
        exit(1);
      }
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
					dup2(pipefd[i-1][0], STDIN_FILENO);
					dup2(pipefd[i][1], STDOUT_FILENO);
					close(pipefd[i][1]);
					close(pipefd[i-1][0]);
				}
				if(execvp(argv[pipe_locate[i] + 1], argv + pipe_locate[i] + 1) < 0){
					perror("execvp");
					exit(EXIT_FAILURE);
				}
			}else{
			}
		}
	}
	for(int i=0;i<pipe_count-1;i++){//ここの回数が意味わからん
		wait(NULL);
	}
	free(pipe_locate);
}

int parsecmd(char **argv, char *buf, char *ebuf)
{
	char *s;
	int  i = 0;
	
	s = buf;

	while (s < ebuf) {//字句解析、空白等があればそれ以前、まず空白を飛ばして文字列をargvの配列に先頭アドレスを格納
		while (s < ebuf && strchr(whitespace, *s)) s++;
		if (ebuf <= s) return -1;

		argv[i++] = s;//argvに入るのはポインタアドレス
		while (s < ebuf && !strchr(whitespace, *s)) s++;
		*s = '\0';
		s++;
	}

	return 1;
}

int getcmd(char *buf, int len)
{
	char path_name[BUFSIZE];
	getcwd(path_name, BUFSIZE);
	printf("Joe:%s ",path_name);
	printf("> ");
	fflush(stdin);
	memset(buf, 0, len);
	fgets(buf, len, stdin);

	if (buf[0] == 0)
		return -1;
	return 0;
}

int is_FileOrDir(char *path){
  struct stat path_statu;
  if(stat(path,&path_statu)==0){
    if(S_ISDIR(path_statu.st_mode)){
      return 1;
    }else if(S_ISREG(path_statu.st_mode)){
      return 2;
    }else{
      return 3;
    }
  }
  return 0;
}