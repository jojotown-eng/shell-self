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

void runcmd(char *buf)
{
	char *argv[ARGVSIZE];
	
	memset(argv, 0, ARGVSIZE);//値をすべて0にする
	if (parsecmd(argv, buf, &buf[strlen(buf)]) > 0)//コマンド実行

	if(strcmp("cd",argv[0])==0){
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
	}else{
		execvp(argv[0],argv);
		perror("execvp");
	}
}


int main(){

	static char buf[BUFSIZE];

	while(getcmd(buf, BUFSIZE) >= 0) {
		if (fork() == 0)
			runcmd(buf);
		wait(NULL);
	}
	exit(0);

	return 0;
}
