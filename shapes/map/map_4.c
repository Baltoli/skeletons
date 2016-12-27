int f(int);

int main(void) {
  int *x, *y;

  int N;
  // CHECK-NOT: Found
  for(int j = 0; j < N;) {
    x[j] = f(y[j]);
  }
}
