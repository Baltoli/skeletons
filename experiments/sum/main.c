#include "fill.h"
#include "sum.h"

#include <stdio.h>
#include <stdlib.h>

int main(void) {
  long *arr = malloc(sizeof(*arr) * 1000000);
  fill(arr, 1000000);
  long s = sum(arr, 1000000);
  printf("%ld\n", s);
  return 0;
}
