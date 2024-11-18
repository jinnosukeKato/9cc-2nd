#include "9cc.h"

void gen_lval(Node *node) {
  if (node->kind != ND_LVAR)
    error_at(token->str, "代入の左辺値が変数ではありません");

  // RBP(ベースポインタ)からローカル変数の位置を計算してpush
  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void gen(Node *node) {
  switch (node->kind) {
    case ND_NUM:
      printf("  push %d\n", node->val);  // 数値なら値をpushするだけ
      return;
    case ND_LVAR:
      gen_lval(node);
      /*
      スタックトップに変数がいることを前提に，スタックトップの値をアドレスとして，
      そのアドレスの値をロードしてpush
      */
      printf("  pop rax\n");
      printf("  mov rax, [rax]\n");
      printf("  push rax\n");
      return;
    case ND_ASSIGN:
      gen_lval(node->lhs);  // 左辺のノードを計算(左辺は必ず変数)
      gen(node->rhs);       // 右辺のノードを計算

      printf("  pop rdi\n");         // 右辺をpop
      printf("  pop rax\n");         // 左辺の変数をpop
      printf("  mov [rax], rdi\n");  // 変数のアドレスに右辺を代入
      printf("  push rdi\n");        // 右辺をpush
      return;
  }

  // 左右辺を数値になるまで再帰
  gen(node->lhs);
  gen(node->rhs);

  // 記号ならば
  // rdi, raxに右→左辺でpopする
  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
    case ND_ADD:
      printf("  add rax, rdi\n");
      break;
    case ND_SUB:
      printf("  sub rax, rdi\n");
      break;
    case ND_MUL:
      printf("  imul rax, rdi\n");
      break;
    case ND_DIV:
      // なぜこの命令になるかは解説を読むこと
      printf("  cqo\n");
      printf("  idiv rdi\n");
      break;
    case ND_EQ:
      printf("  cmp rax, rdi\n");
      printf("  sete al\n");        // rax下位8bit(al)に==の結果を代入
      printf("  movzb rax, al\n");  // raxゼロクリアしつつにal代入
      break;
    case ND_NE:
      printf("  cmp rax, rdi\n");
      printf("  setne al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LT:
      printf("  cmp rax, rdi\n");
      printf("  setl al\n");  // alに<の結果を(略
      printf("  movzb rax, al\n");
      break;
    case ND_LE:
      printf("  cmp rax, rdi\n");
      printf("  setle al\n");  // al ← <=
      printf("  movzb rax, al\n");
      break;
  }

  printf("  push rax\n");
}
