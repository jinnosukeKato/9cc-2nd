#include "9cc.h"

// プログラムを格納する配列
Node *code[100];

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

void program() {
  int i = 0;
  while (!at_eof()) code[i++] = stmt();

  code[i] = NULL;
}

// stmt := expr ";"
Node *stmt() {
  Node *node = expr();
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

// primary := num | "(" add ")"
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
    node->offset = (tok->str[0] - 'a' + 1) * 8;  // 文字数(\n込み)*8bit
    return node;
  }

  // "("でも識別子でもなければ数値のはず
  return new_node_num(expect_number());
}
