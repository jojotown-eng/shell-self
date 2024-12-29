#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>


int main(int argc,char **argv){

  char *command[]={"ls",">","output.txt",NULL};
	int redirect_locate=-1;
	int *pipe_locate=NULL;
	pipe_locate=(int*)malloc(sizeof(int));
	int pipe_count=0;
	int literal_num=0;
	
	for(int i=0;command[i]!=NULL;i++){
		literal_num++;
		if(strcmp(command[i],">")==0){
			redirect_locate=i;
			command[i]=NULL;
		}else if(strcmp(command[i],"|")==0){
			pipe_locate=realloc(pipe_locate,(pipe_count+1) * sizeof(int));
			pipe_locate[pipe_count]=i;
      pipe_count++;
			command[i]=NULL;
		}
	}
	int pipe_connection[pipe_count+1][2];
	for(int i=0;i<pipe_count+1;i++){
		pipe(pipe_connection[i]);
	}
	
	int pipe_sub=0;
	for(int i=0;i<literal_num;i++){
		if(command[i]==NULL){
			if(fork()==0){
				int j;
				if(i==redirect_locate){
					int fd=open(command[i+1], O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
					dup2(fd, 1);
					close(fd);
				}
				// }else{//パイプの処理
				// 	if(i==pipe_locate[0]){
				// 		dup2(pipe_connection[pipe_sub][1], 1);
				// 		close(pipe_connection[pipe_sub][0]);
				// 		close(pipe_connection[pipe_sub][1]);
				// 	}else if(i==pipe_locate[pipe_count-1]){
				// 		dup2(pipe_connection[pipe_sub-1][0], 0);
				// 		close(pipe_connection[pipe_sub-1][0]);
				// 		close(pipe_connection[pipe_sub-1][1]);
				// 	}else{
				// 		dup2(pipe_connection[pipe_sub-1][0], 0);
				// 		dup2(pipe_connection[pipe_sub][1], 1);
				// 		close(pipe_connection[pipe_sub][0]);
				// 		close(pipe_connection[pipe_sub][1]);
				// 		close(pipe_connection[pipe_sub-1][0]);
				// 		close(pipe_connection[pipe_sub-1][1]);
				// 	}
				// 	pipe_sub++;
				// }
				for(j=i-1;command[j]!=NULL && j>0;j--){}
				if (execvp(command[j], command+j) < 0){
					perror("execvp");
					exit(-1);
				}
			}else{//こいつが重要だった
				wait(NULL);
			}

		}
	}

	for (int i = 0; i < literal_num; i++) {
    wait(NULL);
  }
  free(pipe_locate);


  return 0;
}


