#include "9cc.h"

// プログラムを格納する配列
Node *code[100];

// ローカル識別子
LIdent *locals;

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

LIdent *new_ident(Token *tok) {
  LIdent *ident = calloc(1, sizeof(LIdent));
  ident->next = locals;
  ident->name = tok->str;
  ident->len = tok->len;
  ident->offset = locals->offset + 8;
  locals = ident;
}

// 変数を名前でlocalsから検索する．なければNULLを返す．
LIdent *find_ident(Token *tok) {
  for (LIdent *ident = locals; ident; ident = ident->next)
    if (ident->len == tok->len && !memcmp(tok->str, ident->name, ident->len))
      return ident;

  return NULL;
}

// program := function*
void program() {
  int i = 0;
  while (!at_eof()) code[i++] = function();

  code[i] = NULL;
}

/*
  function := ident "(" (" (equality ",")? " ")" stmt
*/
Node *function() {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_FUNCTION;
  Token *tok = consume_ident();
  if (!tok) error("識別子ではありません");

  LIdent *ident = find_ident(tok);
  if (!ident) {
    ident = calloc(1, sizeof(LIdent));
    ident->next = locals;
    ident->name = tok->str;
    ident->len = tok->len;
    ident->offset = locals->offset;  // ローカル変数のためにオフセットを保存
    locals = ident;
  }

  node->len = ident->len;
  node->name = ident->name;

  expect("(");
  while (!consume(")")) {
    consume_ident();
    consume(",");  // カンマがあれば読み飛ばす
    node->arg_len += 1;
    // todo:
    // ここはidentにarg_lenを持たせてそれをコピーするべき，かつ新規ident定義時にのみやるべき
  }

  node->stmt = stmt();
  return node;
}

/*
  stmt := expr ";"
       | "{" stmt* "}"
       | "if" "(" expr ")" stmt ("else" stmt)?
       | "while" "(" expr ")" stmt
       | "return" expr ";"
*/
Node *stmt() {
  Node *node;

  // ブロック {}
  if (consume("{")) {
    Node *head = calloc(1, sizeof(Node));
    head->kind = ND_BLOCK;
    head->next = NULL;
    Node *cur = head;

    while (!consume("}")) {
      Node *tmp = stmt();
      cur->next = tmp;
      cur = tmp;
    }

    return head;
  }

  // if
  if (consume_token(TK_IF)) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_IF;
    expect("(");
    node->lhs = expr();
    expect(")");
    node->rhs = stmt();
    if (consume_token(TK_ELSE))
      node->els = stmt();  // else句がくる場合のみelsを持つ

    return node;
  }

  // while
  if (consume_token(TK_WHILE)) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_WHILE;
    expect("(");
    node->cond = expr();
    expect(")");
    node->rhs = stmt();
    return node;
  }

  if (consume_token(TK_FOR)) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_FOR;
    expect("(");
    if (!consume(";")) {
      node->init = expr();
      expect(";");
    }
    if (!consume(";")) {
      node->cond = expr();
      expect(";");
    }
    if (!consume(")")) {
      node->inc = expr();
      expect(")");
    }
    node->stmt = stmt();
    return node;
  }

  // return
  if (consume_token(TK_RETURN)) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_RETURN;
    node->lhs = expr();
  } else {
    node = expr();
  }

  expect(";");

  return node;
}

// expr := assign();
Node *expr() { return assign(); }

// assign := equality ("=" assign)?
Node *assign() {
  Node *node = equality();
  if (consume("=")) node = new_node(ND_ASSIGN, node, assign());

  return node;
}

// equality := relational ("==" relational | "!=" relational)*
Node *equality() {
  Node *node = relational();

  for (;;) {
    if (consume("=="))
      node = new_node(ND_EQ, node, relational());
    else if (consume("!="))
      node = new_node(ND_NE, node, relational());
    else
      return node;
  }
}

// relational := add ("<" add | "<=" add | ">" add | ">=" add)*
Node *relational() {
  Node *node = add();

  for (;;) {
    if (consume("<"))
      node = new_node(ND_LT, node, add());
    else if (consume("<="))
      node = new_node(ND_LE, node, add());
    else if (consume(">"))
      // 左右辺を入れ替えて不等号の向きを逆にしている
      node = new_node(ND_LT, add(), node);
    else if (consume(">="))
      // 左右辺を入れ替えて不等号の向きを逆にしている
      node = new_node(ND_LE, add(), node);
    else
      return node;
  }
}

// add := mul ("+" mul | "-" mul)*
Node *add() {
  Node *node = mul();

  for (;;) {
    if (consume("+"))
      node = new_node(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}

// mul := unary ("*" unary | "/" unary)*
Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume("*"))
      node = new_node(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

// unary := ("+" | "-")? primary
Node *unary() {
  if (consume("+")) return primary();

  if (consume("-")) return new_node(ND_SUB, new_node_num(0), primary());

  return primary();
}

/*
  primary := num
           | ident ("(" (equality ",")? ")")? 変数または関数呼び出し
           | "(" add ")"
*/
Node *primary() {
  // 次のトークンが"("であるならば"(" add ")"のはず
  if (consume("(")) {
    Node *node = add();
    expect(")");
    return node;
  }

  // 識別子の場合
  Token *tok = consume_ident();
  if (tok) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;

    LIdent *ident = find_ident(tok);
    if (!ident) {
      // その識別子が存在しなければ新しく作成する
      ident = new_ident(tok);
    }

    node->offset = ident->offset;
    node->name = ident->name;
    node->len = ident->len;

    if (consume("(")) {
      node->kind = ND_FUNCCALL;
      for (int i = 0; i < 6; i++) {
        if (consume(")")) return node;
        consume(",");  // カンマがあれば読み飛ばす
        node->arg[i] = equality();
      }
      expect(")");
    }

    return node;
  }

  // "("でも識別子でもなければ数値のはず
  return new_node_num(expect_number());
}
