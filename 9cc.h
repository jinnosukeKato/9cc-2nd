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
  TK_ELSE,      // else
  TK_WHILE,     // while
  TK_FOR,       // for
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

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
bool consume_token(TokenKind tok);
Token *consume_ident();
void expect(char *op);
int expect_number();
bool at_eof();
bool isequal(char *a, char *b);
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
Token *tokenize();

// parse.c
// 抽象構文木のノードの種類
typedef enum {
  ND_BLOCK,     // block
  ND_FUNCCALL,  // 関数呼び出し
  ND_RETURN,    // return
  ND_IF,        // if
  ND_WHILE,     // while
  ND_FOR,       // for
  ND_ADDR,      // &
  ND_DEREF,     // *
  ND_ADD,       // +
  ND_SUB,       // -
  ND_MUL,       // *
  ND_DIV,       // /
  ND_EQ,        // ==
  ND_NE,        // !=
  ND_LT,        // < | >
  ND_LE,        // <= | >=
  ND_ASSIGN,    // =
  ND_LVAR,      // ローカル変数
  ND_NUM,       // 整数
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
  NodeKind kind;  // ノードの種類

  Node *next;  // ブロックの次の行

  Node *lhs;  // 2項演算の左辺
  Node *rhs;  // 2項演算の右辺

  Node *els;  // if-else文のelse

  Node *init;  // for文のinitialize文
  Node *cond;  // for, while文の条件式
  Node *inc;   // for文の変化式
  Node *stmt;  // for, while文，blockの本文

  int val;  // 数字の数

  int offset;  // ローカル変数のオフセット
};

// ローカル識別子の型
typedef struct LIdent LIdent;

struct LIdent {
  LIdent *next;  // 次のローカル識別子かNULL
  char *name;    // 識別子
  int len;       // 識別子の長さ
  int offset;    // RBPからのオフセット

  Node *arg[6];  // 引数
};

extern LIdent *locals;

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
void gen_comment(Node *node);
void gen_lval(Node *node);
void gen(Node *node);

extern int label_if;
extern int label_else;
extern int label_while;
extern int label_for;
