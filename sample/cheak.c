#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
    char *command[] = {"ls", "|", "grep", "output", ">", "output.txt", NULL};
    int redirect_locate = -1;
    int pipe_count = 0;
    int *pipe_locate = NULL;
    int literal_num = 0;

    // コマンド解析
    for (int i = 0; command[i] != NULL; i++) {
        literal_num++;
        if (strcmp(command[i], ">") == 0) {
            redirect_locate = i;
            command[i] = NULL;
        } else if (strcmp(command[i], "|") == 0) {
            pipe_count++;
            pipe_locate = realloc(pipe_locate, pipe_count * sizeof(int));
            pipe_locate[pipe_count - 1] = i;
            command[i] = NULL;
        }
    }

    int pipe_connection[pipe_count][2];
    for (int i = 0; i < pipe_count; i++) {
        if (pipe(pipe_connection[i]) < 0) {
            perror("pipe");
            exit(1);
        }
    }

    int pipe_sub = 0;
    for (int i = 0; i <= pipe_count; i++) {
        if (fork() == 0) {
            // リダイレクト処理
            if (i == pipe_count && redirect_locate != -1) {
                int fd = open(command[redirect_locate + 1], O_CREAT | O_WRONLY | O_TRUNC,
                              S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                if (fd < 0) {
                    perror("open");
                    exit(1);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }

            // パイプ処理
            if (pipe_count > 0) {
                if (i == 0) {  // 最初のコマンド
                    dup2(pipe_connection[0][1], STDOUT_FILENO);
                } else if (i == pipe_count) {  // 最後のコマンド
                    dup2(pipe_connection[pipe_count - 1][0], STDIN_FILENO);
                } else {  // 中間のコマンド
                    dup2(pipe_connection[i - 1][0], STDIN_FILENO);
                    dup2(pipe_connection[i][1], STDOUT_FILENO);
                }
            }

            // 親プロセスで開いたパイプを閉じる
            for (int j = 0; j < pipe_count; j++) {
                close(pipe_connection[j][0]);
                close(pipe_connection[j][1]);
            }

            // 実行するコマンドの位置を計算
            int start = (i == 0) ? 0 : pipe_locate[i - 1] + 1;
            if (execvp(command[start], &command[start]) < 0) {
                perror("execvp");
                exit(1);
            }
        }
    }

    // 親プロセスでパイプを閉じる
    for (int i = 0; i < pipe_count; i++) {
        close(pipe_connection[i][0]);
        close(pipe_connection[i][1]);
    }

    // 子プロセスの終了を待つ
    for (int i = 0; i <= pipe_count; i++) {
        wait(NULL);
    }

    free(pipe_locate);
    return 0;
}
