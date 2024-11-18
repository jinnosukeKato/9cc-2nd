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
  // パース
  Node *node = expr();

  // アセンブリの最初の部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  // 抽象構文木を下りながらコード生成
  gen(node);

  // スタックトップに式全体の値が残るはず→それをRAXにロードし返す
  printf("  pop rax\n");
  printf("  ret\n");

  return 0;
}
