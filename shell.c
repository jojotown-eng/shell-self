/**
 * @brief シェルを実装するプログラム
 * 入力はプログラム内で行って、画面からの入力は後から個別で処理を記述する
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define BUFSIZE  1024
#define ARGVSIZE 100

const char whitespace[] = " \t\r\n\v";



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
  argv[i] = NULL;  // 配列の最後にNULLを追加
	return 1;
}

char test[]="ls > new_file.txt";

int main()
{
  char *argv[] = {"ls", "|", "head" , NULL};
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
		if(fork()==0){//子プロセス処理
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
			execvp(argv[pipe_locate[i] + 1], argv + pipe_locate[i] + 1);
			exit(-1);
		}else{//親プロセス 何もしないで待つ
		}
	}
	//ゾンビプロセス避けるために
	for(int i=0;i<pipe_count;i++){
		wait(NULL);
	}
	free(pipe_locate);
  return 0;
}

/**
 * @brief
 * pipeの数はコマンドの数-1としてあらわされるから、ループの数はpipeパイプ+1
 * スタートのpipeに-1が入っているから、+1する
 */