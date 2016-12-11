int f(int);

int main(void) {
  int *x, *y;

  // CHECK: Near miss
  for(int i = 0; i < 100; i++) {
    x[9] = 45;
    x[i] = f(y[i]);
  }
}
