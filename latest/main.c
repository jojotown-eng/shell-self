/*
 *  Copyright (c) 2017 Hiroshi Yamada <hiroshiy@cc.tuat.ac.jp>
 *
 *  a simple shell
 */
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

const char whitespace[] = " \t\r\n\v";

int main(int argc, char**argv)
{
	static char buf[BUFSIZE];

	while(getcmd(buf, BUFSIZE) >= 0) {
		if(buf[0]=='c' && buf[1]=='d' && buf[2]==' '){
			buf[strlen(buf)-1]=0;
			if(chdir(buf+3)<0){
				printf("cannot cd\n");
			}
			continue;
		}
		if (fork() == 0)
			runcmd(buf);
		wait(NULL);
	}
	exit(0);
}