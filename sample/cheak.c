#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/wait.h>


int is_FileOrDir(char *path);

int main(int argc,char **argv){
  struct stat path_statu;
  if(argc!=2){
    exit(EXIT_FAILURE);
  }
    printf("%d\n",is_FileOrDir(argv[1]));



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
