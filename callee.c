#include <stdio.h>

int foo() { return 5; }

int print() {
  printf("Hello, world!\n");
  return 0;
}

int argument(int a) { return a; }

int add2args(int a, int b) { return a + b; }

int add6args(int a, int b, int c, int d, int e, int f) {
  return a + b + c + d + e + f;
}
