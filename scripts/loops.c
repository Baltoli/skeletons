#include <stdio.h>

int loopy() {
  int x = 0;
  for(int i = 0; i < 10; i++) {
    printf("%d\n", i);
    x+=i;
  }
  return x;
}
