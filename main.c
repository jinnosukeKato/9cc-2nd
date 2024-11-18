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
  program();

  // アセンブリの最初の部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  // プロローグ
  // 変数26個分の領域を確保する
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n");

  // 先頭の式から順にコード生成
  for (int i = 0; code[i]; i++) {
    gen(code[i]);

    /*
    式の評価結果として最終的にスタックに1つの値が残っている
    はずなので，スタックが溢れないようにポップしておく
    */
    printf("  pop rax\n");
  }

  // エピローグ
  // RAXに結果が残っているのでそれを返り値にする
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");

  return 0;
}
