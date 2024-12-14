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

assert_inner_main() {
  expected="$1";
  input="main() {$2}";

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

assert 0 "main() {return 0;}"
assert_inner_main 0 "0;"
assert_inner_main 42 "42;"
assert_inner_main 21 "5+20-4;"
assert_inner_main 41 " 12 + 34 -  5;"
assert_inner_main 5 "2*5/2;"
assert_inner_main 4 "8 / 4  * 2;"
assert_inner_main 2 "(2+3) * 2 / 5;"
assert_inner_main 5 "-10 + 15;"
assert_inner_main 0 "-(7+3) + 10;"
assert_inner_main 1 "1 == 1;"
assert_inner_main 1 "1 != 0;"
assert_inner_main 0 "5 < 1;"
assert_inner_main 1 "100 <= 100;"
assert_inner_main 1 "25 > 24;"
assert_inner_main 0 "10 <= 2;"
assert_inner_main 9 "a = 9;"
assert_inner_main 14 "a = 3; b = 5 * 6 - 8; a + b / 2;"
assert_inner_main 5 "a=b=c=5;"
assert_inner_main 80 "foo = 40; foo*2;"
assert_inner_main 35 "bar1 = 2 + 3; bar1 * (4 + 3);"
assert_inner_main 0 "return 0;"
assert_inner_main 90 "a = 45; b = a*2; return b;"
assert_inner_main 5 "if(0) return 10; return 5;"
assert_inner_main 10 "if(0 < 1) return 10; return 5;"
assert_inner_main 253 "if(1 == 1) return 253; return 0;"
assert_inner_main 5 "if (1 == 0) return 0; else return 5;"
assert_inner_main 10 "if (5 > 10) return 1; else if(9 > 10) return 2; else return 10;"
assert_inner_main 90 "a = 90; if (0) 0; return a;"
assert_inner_main 5 "a = 0; while(a < 5) a=a+1; return a;"
assert_inner_main 1 "a = 1; while(0) 0; return a;"
assert_inner_main 10 "foo = 0; for(i = 0; i <= 10; i = i + 1) foo = i; return foo;"
# assert_inner_main 5 "for(i = 10;; i = i-1) if(i == 5) return i;"
assert_inner_main 0 "for(i = 10; i != 10; i = 10) return 1; return 0;"
assert_inner_main 10 "a = 0; if(a == 0) {a = 5; a = a + 5;} return a;"
# assert_inner_main 55 "a = 0; for(i = 1;; i = i + 1) { a = a+i; if(i >= 10) {return a;}}"
assert_inner_main 16 "i = 2; while(1){ if(i >= 16){ return i; } i = i + 1;} return 999;"
assert_inner_main 5 "foo();"
# assert_inner_main 0 "print();"
assert_inner_main 77 "argument(77);"
assert_inner_main 30 "add2args(20, 10);"
assert_inner_main 21 "add6args(1, 2, 3, 4, 5, 6);"
assert_inner_main 20 "ten = 10; return add2args(ten, 10);"

assert 10 "hoge() {return 5;} main(){ return foo() + 5; }"

echo OK
