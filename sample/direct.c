#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(void){
  int fd;
  fd=open("./output.txt",O_WRONLY);
  write(fd, "123",3);

  return 0;
}