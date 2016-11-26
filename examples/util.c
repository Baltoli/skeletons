#include <stddef.h>
#include <stdio.h>

#include "util.h"

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
