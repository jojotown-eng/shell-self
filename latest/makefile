# 変数定義
CC = gcc                # コンパイラ
CFLAGS = -Wall -g       # コンパイラのオプション
INCLUDES = -I.          # ヘッダファイルの場所（ここではカレントディレクトリ）

# 実行可能ファイルのターゲット
shell: main.o function.o
	$(CC) $(CFLAGS) -o shell main.o function.o

# main.o のターゲット
main.o: main.c function.h
	$(CC) $(CFLAGS) $(INCLUDES) -c main.c

# function.o のターゲット
function.o: function.c function.h
	$(CC) $(CFLAGS) $(INCLUDES) -c function.c

# クリーンアップのためのターゲット
clean:
	rm -f *.o shell
