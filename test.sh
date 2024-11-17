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

assert 0 0
assert 42 42
assert 21 "5+20-4"
assert 41 " 12 + 34 -  5"
assert 5 "2*5/2"
assert 4 "8 / 4  * 2"
assert 2 "(2+3) * 2 / 5"
assert 5 "-10 + 15"
assert 0 "-(7+3) + 10"
echo OK
