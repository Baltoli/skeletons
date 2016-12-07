int f(int);

int main(void) {
  int *x, *y;
  for(int i = 0; i < 100; i++) {
    x[i] = f(y[i]);
  }
}
