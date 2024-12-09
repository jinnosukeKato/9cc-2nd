#include "9cc.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  // 入力プログラムを取得
  user_input = argv[1];
  // トークナイズ
  token = tokenize();

  // ローカルのメモリ領域を確保
  locals = calloc(1, sizeof(LIdent));
  // パース
  parse();

  // アセンブリの最初の部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");

  // 先頭の式から順にコード生成
  label_if = 0;    // ifのラベル
  label_else = 0;  // elseのラベル
  label_while = 0;
  label_for = 0;
  for (int i = 0; code[i]; i++) {
    printf("\n  #statement %d\n", i);
    gen(code[i]);
  }

  return 0;
}
