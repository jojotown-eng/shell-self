/*
 *  Copyright (c) 2017 Hiroshi Yamada <hiroshiy@cc.tuat.ac.jp>
 *
 *  a simple shell
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

#define BUFSIZE  1024
#define ARGVSIZE 100

const char whitespace[] = " \t\r\n\v";

int index=0;//配列の番号をプログラムを通して判断する

// Parsed command representation
#define EXEC  1
#define REDIR 2
#define PIPE  3

#define MAXARGS 10

struct cmd{
  int type;
};

struct execcmd {
  int type;
  char *argv[MAXARGS];
  //char *eargv[MAXARGS];
};

struct redircmd {
  int type;
  struct cmd *cmd;
  char *file;
  char *efile;//ファイルの名前を管理するため
  int mode;
  int fd;
};

struct pipecmd {
  int type;
  struct cmd *left;
  struct cmd *right;
};

struct cmd*
execcmd(void)
{
  struct execcmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = EXEC;
  return (struct cmd*)cmd;
}

struct cmd*
redircmd(struct cmd *subcmd, char *file, char *efile, int mode, int fd)
{
  struct redircmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = REDIR;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->efile = efile;
  cmd->mode = mode;
  cmd->fd = fd;
  return (struct cmd*)cmd;
}

struct cmd*
pipecmd(struct cmd *left, struct cmd *right)
{
  struct pipecmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = PIPE;
  cmd->left = left;
  cmd->right = right;
  return (struct cmd*)cmd;
}

int parsecmd(char **argv, char *buf, char *ebuf)
{
	char *s;
	int  i = 0;
	
	s = buf;

	while (s < ebuf) {
		while (s < ebuf && strchr(whitespace, *s)) s++;
		if (ebuf <= s) return -1;

		argv[i++] = s;
		while (s < ebuf && !strchr(whitespace, *s)) s++;
		*s = '\0'; 
		s++;
	}

	return 1;
}

void runcmd(char *buf)
{
	char *argv[ARGVSIZE];
	
	memset(argv, 0, ARGVSIZE);
	if (parsecmd(argv, buf, &buf[strlen(buf)]) > 0);
  
	exit(-1);
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

char cheakcmd(char **argv){//コマンドが入る,indexを使って文字の番号を見る
	
}