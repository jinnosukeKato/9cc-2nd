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

assert 0 "0;"
assert 42 "42;"
assert 21 "5+20-4;"
assert 41 " 12 + 34 -  5;"
assert 5 "2*5/2;"
assert 4 "8 / 4  * 2;"
assert 2 "(2+3) * 2 / 5;"
assert 5 "-10 + 15;"
assert 0 "-(7+3) + 10;"
assert 1 "1 == 1;"
assert 1 "1 != 0;"
assert 0 "5 < 1;"
assert 1 "100 <= 100;"
assert 1 "25 > 24;"
assert 0 "10 <= 2;"
assert 9 "int a = 9;"
assert 14 "int a = 3; int b = 5 * 6 - 8; a + b / 2;"
assert 5 "int a; int b; int c; int d; a=b=c=5;"
assert 80 "int foo = 40; foo*2;"
assert 35 "int bar1 = 2 + 3; bar1 * (4 + 3);"
assert 0 "return 0;"
assert 90 "int a = 45; int b = a*2; return b;"
assert 5 "if(0) return 10; return 5;"
assert 10 "if(0 < 1) return 10; return 5;"
assert 253 "if(1 == 1) return 253; return 0;"
assert 5 "if (1 == 0) return 0; else return 5;"
assert 10 "if (5 > 10) return 1; else if(9 > 10) return 2; else return 10;"
assert 90 "int a = 90; if (0) 0; return a;"
assert 5 "int a = 0; while(a < 5) a=a+1; return a;"
assert 1 "int a = 1; while(0) 0; return a;"
assert 10 "int foo = 0; for(int i = 0; i <= 10; i = i + 1) foo = i; return foo;"
assert 5 "for(int i = 10;; i = i-1) if(i == 5) return i;"
assert 0 "for(int i = 10; i != 10; i = 10) return 1; return 0;"
assert 10 "int a = 0; if(a == 0) {a = 5; a = a + 5;} return a;"
assert 55 "int a = 0; for(int i = 1;; i = i + 1) { a = a+i; if(i >= 10) {return a;}}"
assert 16 "int i = 2; while(1){ if(i >= 16){ return i; } i = i + 1;} return 999;"
assert 5 "int foo(); foo();"
assert 0 "int print(); print();"
assert 77 "int argument(77);"
assert 30 "int add2args(20, 10);"
assert 21 "int add6args(1, 2, 3, 4, 5, 6);"
assert 20 "int ten = 10; return int add2args(ten, 10);"
assert 10 "int a = 10; int b = &a; return *b;"
assert 90 "int foo = 50; int bar = foo + 40; return bar;"

echo OK
