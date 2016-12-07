int f(int);

int main(void) {
  int *x, *y;

  int N;
  for(int j = 0; j < N; ++j) {
    x[j] = f(y[j]);
  }
}
