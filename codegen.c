#include "9cc.h"

void gen_lval(Node *node) {
  if (node->kind != ND_LVAR)
    error_at(token->str, "代入の左辺値が変数ではありません");

  // RBP(ベースポインタ)からローカル変数の位置を計算してpush
  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

int label_if;
int label_else;
int label_while;
int label_for;

void gen(Node *node) {
  switch (node->kind) {
    case ND_BLOCK:
      Node *stmt = node->next;
      while (stmt) {
        gen(stmt);
        // printf("  pop rax\n");
        stmt = stmt->next;
      }
      return;

    case ND_FUNCCALL:
      if (locals->offset % 16 != 0)
        printf("  sub rsp, 0x8\n");  // RSPを16の倍数にalignmentする

      printf("  call %.*s\n", locals[(locals->offset - node->offset) / 8].len,
             locals[(locals->offset - node->offset) / 8].name);
      printf("  push rax\n");
      return;

    case ND_IF:
      /*
        if (stmt)
          expr;
        ↓
        if (stmt == 0)
          goto fi;
        expr;
        fi:(ここに飛ぶ)

        if (stmt)
          expr;
        else
          expr_els;
        ↓
        if (stmt == 0)
          goto els;
        expr;
        goto fi;
        els:
          expr_els;
        fi:
      */

      // if文の中が一度も実行されなかったときのために前のlineの評価結果をpushする
      printf("  push rax\n");
      gen(node->lhs);
      printf("  pop rax\n");     // lhs(評価式)の値を取る
      printf("  cmp rax, 0\n");  // 評価式==0(偽か)→結果はZFに
      if (node->els) {
        // else節を持つ場合
        printf("  je .Lelse%d\n", label_else);  // .LelseXXXに偽ならjump
        gen(node->rhs);
        printf("  jmp .Lfi%d\n", label_if);
        printf(".Lelse%d:\n", label_else++);
        gen(node->els);
      } else {
        printf("  je .Lfi%d\n", label_if);  // .LendXXXに偽(ZF=0)ならjump
        gen(node->rhs);
      }
      printf(".Lfi%d:\n", label_if++);
      return;

    case ND_WHILE:
      // while文の中が一度も実行されなかったときのために前のlineの評価結果をpushする
      printf("  push rax\n");

      printf(".Lwhl_begin%d:\n", label_while);
      gen(node->lhs);  // expr
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");                   // 条件式が偽か判定
      printf("  je .Lwhl_end%d\n", label_while);  // 偽なら終わりにjump
      gen(node->rhs);
      printf("  jmp .Lwhl_begin%d\n", label_while);  // 頭に戻る
      printf(".Lwhl_end%d:\n", label_while++);
      return;

    case ND_FOR:
      // for文の中が一度も実行されなかったときのために前のlineの評価結果をpushする
      // printf("  push rax\n");

      if (node->init) gen(node->init);  // 初期化式
      printf(".Lfor_begin%d:\n", label_for);
      if (node->cond) gen(node->cond);  // 継続条件式
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");                 // 継続条件が偽か
      printf("  je .Lfor_end%d\n", label_for);  // 継続条件が偽ならendにjump
      gen(node->stmt);                          // forの中身
      if (node->inc) gen(node->inc);  // インクリメント式?(名前わからん)
      printf("  jmp .Lfor_begin%d\n", label_for);  // 最初に飛ぶ
      printf(".Lfor_end%d:\n", label_for++);
      return;

    case ND_RETURN:
      gen(node->lhs);
      printf("  pop rax\n");  // スタックトップ(式の結果があるはず)からraxにpop
      // エピローグ
      printf("  mov rsp, rbp\n");
      printf("  pop rbp\n");
      printf("  ret\n");
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

    case ND_NUM:
      printf("  push %d\n", node->val);  // 数値なら値をpushするだけ
      return;
  }

  // 左右辺を終端記号になるまで再帰
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
