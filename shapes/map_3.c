int f(int);

int main(void) {
  int *x, *y;

  int N, i;
  // CHECK-NOT: found
  for(int j = 0; i < N; ++j) {
    x[j] = f(y[j]);
  }
}
