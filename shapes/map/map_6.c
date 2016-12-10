int f(int);

int main(void) {
  int *x, *y;

  // CHECK: Found
  // CHECK: line [[@LINE+1]]
  for(int i = 0; i < 100; i++) {
    x[9] = 45;
    x[i] = f(y[i]);
  }
}
