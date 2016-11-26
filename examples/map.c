#include <string.h>
#include <stddef.h>
#include <omp.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "util.h"

/**
 * Mapping a function over a list is one of the simplest examples of a parallel
 * skeleton - the sequential implementation is easy to understand in terms of a
 * "traditional" C-style for loop, but there are multiple viable strategies for
 * introducing parallelism into this idiom.
 */

void range_fill(int *arr, size_t n) {
  for(int i = 0; i < n; i++) {
    arr[i] = i;
  }
}

void print_array(int *arr, size_t n) {
  printf("[");
  for(size_t i = 0; i < n - 1; i++) {
    printf("%d, ", arr[i]);
  }

  if(n > 0) {
    printf("%d]\n", arr[n-1]);
  } else {
    printf("]\n");
  }
}

int id(int value) {
  return value;
}

int square(int value) {
  int tmp = 0;
  for(int i = 0; i < 50; i++) {
    tmp += rand();
  }
  return tmp;
}

int succ(int value) {
  return value + 1;
}

void map_seq(int (*work)(int), int *arr, int *out, size_t n) {
  for(size_t i = 0; i < n; i++) {
    out[i] = work(arr[i]);
  }
}

void map_par(int (*work)(int), int *arr, int *out, size_t n) {
  #pragma omp parallel for num_threads(2)
  for(size_t i = 0; i < n; i++) {
    out[i] = work(arr[i]);
  }
}


int main(void) {
  const size_t size = 250000;

  clock_t fill_start = clock();
  int seq_nums[size];
  int *seq_out = malloc(size * sizeof(int));
  range_fill(seq_nums, size);

  int par_nums[size];
  int *par_out = malloc(size * sizeof(int));
  range_fill(par_nums, size);
  clock_t fill_end = clock();
  printf("Array (%zu) fill time: %fs\n", size, (double)(fill_end - fill_start) / CLOCKS_PER_SEC);

  clock_t par_start = clock();
  for(int i = 0; i < 100; i++) map_par(&square, par_nums, par_out, size);
  clock_t par_end = clock();
  printf("Parallel run time: %fs\n", (double)(par_end - par_start) / CLOCKS_PER_SEC);

  clock_t seq_start = clock();
  for(int i = 0; i < 100; i++) map_seq(&square, seq_nums, seq_out, size);
  clock_t seq_end = clock();
  printf("Sequential run time: %fs\n", (double)(seq_end - seq_start) / CLOCKS_PER_SEC);

  return 0;
}
