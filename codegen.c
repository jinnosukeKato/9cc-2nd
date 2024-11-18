#include "9cc.h"

void gen(Node *node) {
  // 数値ならばRSPにpushする
  if (node->kind == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  // 数値でないならば左右辺を数値になるまで再帰
  gen(node->lhs);
  gen(node->rhs);

  // 記号ならば
  // rdi, raxに左右辺をpopする
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
