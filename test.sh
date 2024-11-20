#!/bin/bash

assert() {
  expected="$1";
  input="$2";

  ./9cc "$input" > tmp.s
  cc -o tmp tmp.s
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
assert 9 "a = 9;"
assert 14 "a = 3; b = 5 * 6 - 8; a + b / 2;"
assert 5 "a=b=c=5;"
assert 80 "foo = 40; foo*2;"
assert 35 "bar1 = 2 + 3; bar1 * (4 + 3);"
assert 0 "return 0;"
assert 90 "a = 45; b = a*2; return b;"

echo OK
