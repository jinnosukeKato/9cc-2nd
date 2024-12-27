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

assert 0 "main() return 0;"
assert 42 "main() return 42;"
assert 21 "main() return 5+20-4;"
assert 41 "main() return 12 + 34 -  5;"
assert 5 "main() return 2*5/2;"
assert 4 "main() return 8 / 4  * 2;"
assert 2 "main() return (2+3) * 2 / 5;"
assert 5 "main() return -10 + 15;"
assert 0 "main() return -(7+3) + 10;"
assert 1 "main() return 1 == 1;"
assert 1 "main() return 1 != 0;"
assert 0 "main() return 5 < 1;"
assert 1 "main() return 100 <= 100;"
assert 1 "main() return 25 > 24;"
assert 0 "main() return 10 <= 2;"
assert 9 "main() { a = 9; return a; }"
assert 14 "main() {a = 3; b = 5 * 6 - 8; return a + b / 2;}"
assert 5 "main() {a=b=c=5; return c;}"
assert 80 "main() {foo = 40; return foo*2;}"
assert 35 "main() {bar1 = 2 + 3; return bar1 * (4 + 3);}"
assert 5 "main() { if(0) return 10; return 5;}"
assert 10 "main() {if(0 < 1) return 10; return 5;}"
assert 253 "main() {if(1 == 1) return 253; return 0;}"
assert 5 "main() {if (1 == 0) return 0; else return 5;}"
assert 10 "main() {if (5 > 10) return 1; else if(9 > 10) return 2; else return 10;}"
assert 90 "main() {a = 90; if (0) 0; return a;}"
assert 5 "main() {a = 0; while(a < 5) a=a+1; return a;}"
assert 1 "main() {a = 1; while(0) 0; return a;}"
assert 10 "main() {foo = 0; for(i = 0; i <= 10; i = i + 1) foo = i; return foo;}"
assert 5 "main() {for(i = 10;; i = i-1) if(i == 5) return i;}"
assert 0 "main() {for(i = 10; i != 10; i = 10) return 1; return 0;}"
assert 10 "main() {a = 0; if(a == 0) {a = 5; a = a + 5;} return a;}"
assert 55 "main() {a = 0; for(i = 1;; i = i + 1) { a = a+i; if(i >= 10) {return a;}}}"
assert 16 "main() {i = 2; while(1){ if(i >= 16){ return i; } i = i + 1;} return 999;}"
# assert 5 "main() {return foo();}"
# assert 0 "main() {return print();}"
assert 77 "main() {return argument(77);}"
assert 30 "main() {return add2args(20, 10);}"
assert 21 "main() {return add6args(1, 2, 3, 4, 5, 6);}"
assert 20 "main() {ten = 10; return add2args(ten, 10);}"

assert 10 "bar() return 10; main() return bar();"
assert 15 "bar() { return 5 + 10; } main() {return bar();}"
assert 30 "main() {return add2args(20, 10);}"
assert 19 "bar(a) return a; main() {return bar(19);}"
assert 21 "add6args(a, b, c, d, e, f) {return a + b + c + d + e + f;} main() { return add6args(1, 2, 3, 4, 5, 6);}"

echo OK
