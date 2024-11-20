#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// tokenize.c
typedef enum {
  TK_RESERVED,  // 記号
  TK_RETURN,    // return
  TK_IF,        // if
  TK_IDENT,     // 識別子
  TK_NUM,       // 整数トークン
  TK_EOF,       // 入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;

// トークン型
struct Token {
  TokenKind kind;  // トークンの型
  Token *next;     // 次の入力トークン
  int val;         // kindがTK_NUMの場合，その数値
  char *str;       // トークン文字列
  int len;         // トークンの長さ
};

void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
Token *consume_ident();
void expect(char *op);
int expect_number();
bool at_eof();
bool isequal(char *a, char *b);
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
Token *tokenize();

// parse.c
// ローカル変数の型
typedef struct LVar LVar;

struct LVar {
  LVar *next;  // 次のローカル変数かNULL
  char *name;  // 変数名
  int len;     // 変数名の長さ
  int offset;  // RBPからのオフセット
};

extern LVar *locals;

// 抽象構文木のノードの種類
typedef enum {
  ND_RETURN,  // return
  ND_IF,      // if
  ND_ADD,     // +
  ND_SUB,     // -
  ND_MUL,     // *
  ND_DIV,     // /
  ND_EQ,      // ==
  ND_NE,      // !=
  ND_LT,      // < | >
  ND_LE,      // <= | >=
  ND_ASSIGN,  // =
  ND_LVAR,    // ローカル変数
  ND_NUM,     // 整数
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
  NodeKind kind;  // ノードの型
  Node *lhs;      // 左辺
  Node *rhs;      // 右辺
  int val;        // kindがND_NUMの場合のみ使う
  int offset;     // kindがND_LVARの場合のみ使う
};

extern Token *token;
extern char *user_input;
extern Node *code[100];

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);

void program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *primary();
Node *unary();

// codegen.c
void gen_lval(Node *node);
void gen(Node *node);

extern int label_if;
