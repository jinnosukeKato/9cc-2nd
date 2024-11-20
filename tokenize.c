#include "9cc.h"

// 現在着目しているトークン
Token *token;

// 入力プログラム
char *user_input;

// エラー箇所を報告
void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  // 問題のある入力のポインタ-プログラムの頭のポインタ
  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, " ");  // pos個の空白を出力
  fprintf(stderr, "^");

  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

/*
次のトークンが期待しているトークンの時には，トークンを1つ読み進めて
真を返す．それ以外の場合には偽を返す．
*/
bool consume(char *op) {
  if ((token->kind != TK_RESERVED && token->kind != TK_RETURN) ||
      strlen(op) != token->len || memcmp(token->str, op, token->len))
    return false;

  token = token->next;
  return true;
}

/*
次のトークンが識別子の場合にトークンを1つ読み進めてそのトークンを返す．
識別子でない場合はなにもせずNULLを返す，
*/
Token *consume_ident() {
  if (token->kind != TK_IDENT) return NULL;

  Token *t = token;
  token = token->next;
  return t;
}

/*
次のトークンが期待している記号の時には，トークンを1つ読み進める．
それ以外の場合にはエラーを報告する．
*/
void expect(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    error_at(token->str, "'%s'ではありません", op);

  token = token->next;
}

/*
次のトークンが数値の場合，トークンを1つ読み進めてその数値を返す．
それ以外の場合にはエラーを報告する．
*/
int expect_number() {
  if (token->kind != TK_NUM) error_at(token->str, "数ではありません");

  int value = token->val;
  token = token->next;
  return value;
}

bool at_eof() { return token->kind == TK_EOF; }

// 文字列比較用 length(p) <= length(q)
bool isequal(char *a, char *b) { return memcmp(a, b, strlen(b)) == 0; }

// 新しいトークンを作成してcurにつなげる
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));  // callocはmallocと違い0クリアする
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

// 入力文字列user_inputをトークナイズして返す
Token *tokenize() {
  char *p = user_input;
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    // 空白文字をスキップ
    if (isspace(*p)) {
      p++;
      continue;
    }

    // 2文字の演算子
    if (isequal(p, "==") || isequal(p, "!=") || isequal(p, ">=") ||
        isequal(p, "<=")) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    // 1文字の演算子
    if (strchr("+-*/()<>;=", *p)) {
      // 現在の文字ポインタpをnew_tokenに渡してからインクリメントしている
      cur = new_token(TK_RESERVED, cur, p, 1);
      p += 1;
      continue;
    }

    // return
    if (strncmp(p, "return", 6) == 0 && !isalnum(p[6])) {
      cur = new_token(TK_RETURN, cur, p, 6);
      p += 6;
      continue;
    }

    // 変数
    if (isalpha(*p)) {
      char *q = p++;
      while (isalnum(*p)) p++;
      cur = new_token(TK_IDENT, cur, q, p - q);
      continue;
    }

    // 数値
    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p, 0);
      char *q = p;
      // strtolによってpが適切にインクリメントされる
      cur->val = strtol(p, &p, 10);
      cur->len = q - p;  // strtolによるポインタインクリメントを使う
      continue;
    }

    error_at(token->str, "トークナイズできません");
  }

  new_token(TK_EOF, cur, p, 0);
  return head.next;  // 最後にheadの次を返すことでheadを無視
}
