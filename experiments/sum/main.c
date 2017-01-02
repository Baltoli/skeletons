#include "fill.h"
#include "sum.h"

#include <stdio.h>
#include <stdlib.h>

#define SIZE 100000

int main(void) {
  long *arr = malloc(sizeof(*arr) * SIZE);
  long *out = malloc(sizeof(*out) * SIZE);
  long *other = malloc(sizeof(*other) * 1000);

  fill(arr, SIZE);
  random_fill(other, 1000);

  csum(arr, out, SIZE);
  printf("%ld\n", out[45]);
  printf("%d\n", other[576]);
  return 0;
}
