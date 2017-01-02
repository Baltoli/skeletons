#include "sum.h"

long sum(long *arr, long n) {
  long sum = 0;
  for(long i = 0; i < n; i++) {
    sum += arr[i];
  }
  return sum;
}
