/*
 *  Copyright (c) 2017 Hiroshi Yamada <hiroshiy@cc.tuat.ac.jp>
 *
 *  a simple shell
 */

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

const char whitespace[] = " \t\r\n\v";

/**
 * @brief パイプとリダイレクトができる
 * @note リダイレクトは最後にあるとしてプログラムされている
 */
void PipeRedirect(char **argv);

/**
 * @brief コマンドcdを実行するための関数
 */
void cd(char **argv);

/**
 * @note パースしたコマンドを実行する
 * judgという変数を用いて条件を分けてパイプとリダイレクトの処理を分けている
 * ほかの機能も足すとなると、switchの条件を多くしないといけない
 */
void runcmd(char *buf);

/**
 * @note 入力された文字列をパースする
 * 文字列の最後にはそれぞれ\0を入れている
 */
int parsecmd(char **argv, char *buf, char *ebuf);

/**
 * @note 表示部分を担う関数
 */
int getcmd(char *buf, int len);

/**
 * @note fileとディレクトリの判定を行う関数
 * @param 1:ディレクトリ
 * @param 2:ファイル
 * @param 3:その他→特別なファイル等々
 * @param 0:エラー、fileが存在しないとき
 */

int is_FileOrDir(char *path);

int main(int argc, char**argv)
{
	static char buf[BUFSIZE];

	while(getcmd(buf, BUFSIZE) >= 0) {
		if (fork() == 0)
			runcmd(buf);
		wait(NULL);
	}
	exit(0);
}

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
				//close();
			}
		}
	}
	for(int i=0;i<pipe_count-1;i++){//ここの回数が意味わからん
		wait(NULL);
	}
	printf("hello\n");
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


/**
 * @note リダイレクトの拡張性、パイプとリダイレクトを同時に行うには
 */