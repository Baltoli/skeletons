#include "fill.h"
#include "sum.h"

#include <stdio.h>
#include <stdlib.h>

#define SIZE 100000

int main(void) {
  long *arr = malloc(sizeof(*arr) * SIZE);
  long *out = malloc(sizeof(*out) * SIZE);
  fill(arr, SIZE);
  csum(arr, out, SIZE);
  printf("%ld\n", out[45]);
  return 0;
}
