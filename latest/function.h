#ifndef FUNCTION_H
#define FUNCTION_H

extern const char whitespace[];

/**
 * @brief パイプとリダイレクトができる
 * @note リダイレクトは最後にあるとしてプログラムされている
 */
void PipeRedirect(char **argv);

/**
 * @brief コマンドcdを実行するための関数
 */
void cd(char **argv);

/**
 * @note パースしたコマンドを実行する
 * judgという変数を用いて条件を分けてパイプとリダイレクトの処理を分けている
 * ほかの機能も足すとなると、switchの条件を多くしないといけない
 */
void runcmd(char *buf);

/**
 * @note 入力された文字列をパースする
 * 文字列の最後にはそれぞれ\0を入れている
 */
int parsecmd(char **argv, char *buf, char *ebuf);

/**
 * @note 表示部分を担う関数
 */
int getcmd(char *buf, int len);

/**
 * @note fileとディレクトリの判定を行う関数
 * @param 1:ディレクトリ
 * @param 2:ファイル
 * @param 3:その他→特別なファイル等々
 * @param 0:エラー、fileが存在しないとき
 */

int is_FileOrDir(char *path);

#endif