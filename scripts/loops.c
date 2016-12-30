#include <stdio.h>

int loopy() {
  volatile int x;
  int i;
  for(i = 0; i < 10; i++) {
    printf("%d\n", i);
    x+=i;
  }
  return x;
}
