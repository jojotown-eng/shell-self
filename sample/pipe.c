#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int main() {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // 子プロセス
        close(pipefd[0]); // 読み取り用は閉じる

        // 標準出力をパイプの書き込み用にリダイレクト
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]); // 書き込み用は閉じる

        // lsコマンドを実行
        execlp("ls", "ls", NULL);
        
        // execlpが失敗した場合のエラーハンドリング
        perror("execlp");
        exit(EXIT_FAILURE);
    } else { // 親プロセス
        close(pipefd[1]); // 書き込み用は閉じる

        char buffer[100];
        ssize_t count;

        // 子プロセスからの出力を読み取る
        while ((count = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[count] = '\0'; // 終端文字を追加
            printf("%s", buffer); // 出力を表示
        }

        close(pipefd[0]); // 読み取り後は閉じる
        wait(NULL); // 子プロセスの終了を待つ
    }

    return 0;
}
