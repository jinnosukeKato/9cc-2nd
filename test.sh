#!/bin/bash

assert() {
  expected="$1";
  input="$2";

  ./9cc "$input" > tmp.s
  cc -o tmp tmp.s callee.o
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 8 "int main() { int *p; alloc4(&p, 1, 2, 4, 8); int *q; q = p + 3; return *q;}"
assert 0 "int main() return 0;"
assert 42 "int main() return 42;"
assert 21 "int main() return 5+20-4;"
assert 41 "int main() return 12 + 34 -  5;"
assert 5 "int main() return 2*5/2;"
assert 4 "int main() return 8 / 4  * 2;"
assert 2 "int main() return (2+3) * 2 / 5;"
assert 5 "int main() return -10 + 15;"
assert 0 "int main() return -(7+3) + 10;"
assert 1 "int main() return 1 == 1;"
assert 1 "int main() return 1 != 0;"
assert 0 "int main() return 5 < 1;"
assert 1 "int main() return 100 <= 100;"
assert 1 "int main() return 25 > 24;"
assert 0 "int main() return 10 <= 2;"
assert 9 "int main() { int a = 9; return a; }"
assert 14 "int main() {int a = 3; int b = 5 * 6 - 8; return a + b / 2;}"
assert 5 "int main() {int a; int b; int c; int d; a=b=c=5; return c;}"
assert 80 "int main() {int foo = 40; return foo*2;}"
assert 35 "int main() {int bar1 = 2 + 3; return bar1 * (4 + 3);}"
assert 5 "int main() { if(0) return 10; return 5;}"
assert 10 "int main() {if(0 < 1) return 10; return 5;}"
assert 253 "int main() {if(1 == 1) return 253; return 0;}"
assert 5 "int main() {if (1 == 0) return 0; else return 5;}"
assert 10 "int main() {if (5 > 10) return 1; else if(9 > 10) return 2; else return 10;}"
assert 90 "int main() {int a = 90; if (0) 0; return a;}"
assert 5 "int main() {int a = 0; while(a < 5) a=a+1; return a;}"
assert 1 "int main() {int a = 1; while(0) 0; return a;}"
assert 10 "int main() {int foo = 0; for(int i = 0; i <= 10; i = i + 1) foo = i; return foo;}"
assert 5 "int main() {for(int i = 10;; i = i-1) if(i == 5) return i;}"
assert 0 "int main() {for(int i = 10; i != 10; i = 10) return 1; return 0;}"
assert 10 "int main() {int a = 0; if(a == 0) {a = 5; a = a + 5;} return a;}"
assert 55 "int main() {int a = 0; for(int i = 1;; i = i + 1) { a = a+i; if(i >= 10) {return a;}}}"
assert 16 "int main() {int i = 2; while(1){ if(i >= 16){ return i; } i = i + 1;} return 999;}"
assert 5 "int main() {return foo();}"
assert 0 "int main() {return print();}"
assert 77 "int main() {return argument(77);}"
assert 30 "int main() {return add2args(20, 10);}"
assert 21 "int main() {return add6args(1, 2, 3, 4, 5, 6);}"
assert 20 "int main() {int ten = 10; return add2args(ten, 10);}"

assert 10 "int bar() return 10; int main() return bar();"
assert 15 "int bar() { return 5 + 10; } int main() {return bar();}"
assert 30 "int main() {return add2args(20, 10);}"
assert 19 "int bar(int a) return a; int main() {return bar(19);}"
assert 21 "int add6args(int a, int b, int c, int d, int e, int f) {return a + b + c + d + e + f;} int main() { return add6args(1, 2, 3, 4, 5, 6);}"

assert 3 "int main() { int x = 3; int y = &x; return *y;}"
assert 3 "int main() { int x; int *y; y = &x; *y = 3; return x; }"

echo OK
