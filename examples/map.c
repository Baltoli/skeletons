#include <stddef.h>
#include <omp.h>

#include "util.h"

/**
 * Mapping a function over a list is one of the simplest examples of a parallel
 * skeleton - the sequential implementation is easy to understand in terms of a
 * "traditional" C-style for loop, but there are multiple viable strategies for
 * introducing parallelism into this idiom.
 */

int id(int value) {
  return value;
}

int square(int value) {
  return value * value;
}

int succ(int value) {
  return value + 1;
}

void map_seq(int (*work)(int), int *arr, size_t n) {
  for(size_t i = 0; i < n; i++) {
    arr[i] = work(arr[i]);
  }
}

void map_par(int (*work)(int), int *arr, size_t n) {
  #pragma omp parallel for
  for(size_t i = 0; i < n; i++) {
    arr[i] = work(arr[i]);
  }
}

int main(void) {
  int nums[] = { 1, 2, 3, 4, 5 };
  map_par(&succ, nums, 5);
  map_par(&square, nums, 5);
  print_array(nums, 5);
  return 0;
}
