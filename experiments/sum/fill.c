#include "fill.h"

#include <assert.h>
#include <stdlib.h>

void fill(long *arr, long n) {
  assert(n >= 0);

  for(long i = 0; i < n; i++) {
    arr[i] = i;
  }
}

void random_fill(long *arr, long n) {
  assert(n >= 0);

  static long seeded = 0;
  if(!seeded) {
    srand(45678);
    seeded = 1;
  }

  for(long i = 0; i < n; i++) {
    arr[i] = rand() % FILL_RANGE;
  }
}
