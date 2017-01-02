#include "sum.h"

long sum(long *arr, long n) {
  long sum = 0;
  for(long i = 0; i < n; i++) {
    sum += arr[i];
  }
  return sum;
}

void csum(long *in, long *out, long n) {
  for(long i = 0; i < n; i++) {
    out[i] = sum(in, i);
  }
}
