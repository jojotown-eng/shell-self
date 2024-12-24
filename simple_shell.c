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
 * @note リダイレクト処理を実行する関数
 */
void redirect(char **argv);

/**
 * @note 多段パイプを実行する関数
 */
void serise_pipe(char **argv);

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

void redirect(char **argv){
	for(int i=0;argv[i]!=NULL;i++){
		if(strcmp(argv[i],">")==0){
			int fd = open(argv[i+1], O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
			close(1);
			dup(fd);
			close(fd);
			argv[i]=NULL;
			break;
		}
	}
	if (execvp(argv[0], argv) < 0){
		perror("execvp");
		exit(-1);
	}
}

void serise_pipe(char **argv){
	int *pipe_locate;
	pipe_locate=(int*)malloc(sizeof(int));
	*pipe_locate=-1;
	int pipe_count=0;
	pipe_locate[0]=-1;
	for(int i=0;argv[i]!=NULL;i++){
		if(strcmp(argv[i],"|")==0){
			pipe_count++;
			pipe_locate=(int*)realloc(pipe_locate,sizeof(int)*(pipe_count+1));
			pipe_locate[pipe_count]=i;
			argv[i]=NULL;
		}
	}
	int pipe_connect[pipe_count+1][2];
	for(int i=0;i<pipe_count+1;i++){
		if(i!=pipe_count){
			pipe(pipe_connect[i]);
		}
		if(fork()==0){//子プロセス処理 パイプをつなげる
			if(i==0){
				dup2(pipe_connect[i][1],1);
				close(pipe_connect[i][0]);
				close(pipe_connect[i][1]);
			}else if(i==pipe_count){
				dup2(pipe_connect[i-1][0],0);
				close(pipe_connect[i-1][0]);
				close(pipe_connect[i-1][1]);
			}else{
				dup2(pipe_connect[i-1][0],0);
				dup2(pipe_connect[i][1],1);
				close(pipe_connect[i][0]);
				close(pipe_connect[i][1]);
				close(pipe_connect[i-1][0]);
				close(pipe_connect[i-1][1]);
			}
			if(execvp(argv[pipe_locate[i] + 1], argv + pipe_locate[i] + 1)<0){
				perror("execvp");
				exit(EXIT_FAILURE);
			}			
		}else{//親プロセス 何もしないで待つ
		}
	}
	for(int i=0;i<pipe_count;i++){//生成した子プロセス分waitして終了を待つ
		wait(NULL);
	}
	free(pipe_locate);
}

void runcmd(char *buf)
{
	char *argv[ARGVSIZE];
	int judg=-1;
	
	memset(argv, 0, ARGVSIZE);//値をすべて0にする
	if (parsecmd(argv, buf, &buf[strlen(buf)]) > 0)//コマンド実行
	for(int i=0;argv[i]!=NULL;i++){
		if(strcmp(argv[i],">")==0){
			judg=1;
			break;
		}else if(strcmp(argv[i],"|")==0){
			judg=2;
			break;
		}
	}
	switch (judg)
	{
	case 1:
		redirect(argv);
		break;
	
	case 2:
		serise_pipe(argv);
		break;
	
	default:
		if (execvp(argv[0], argv) < 0){
			perror("execvp");
			exit(EXIT_FAILURE);
		}
	}
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