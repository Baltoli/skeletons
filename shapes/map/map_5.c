int f(int);
void g(void);
void h(int);

int main(void) {
  int *x, *y;

  // CHECK: Found
  // CHECK: line [[@LINE+1]]
  for(int i = 0; i < 100; i++) {
    g();
    x[i] = f(y[i]);
    h(y[i]);
  }
}
