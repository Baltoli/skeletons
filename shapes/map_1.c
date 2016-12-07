int f(int);

int main(void) {
  int *x, *y;

  // CHECK: Found
  // CHECK: line 8
  for(int i = 0; i < 100; i++) {
    x[i] = f(y[i]);
  }
}
