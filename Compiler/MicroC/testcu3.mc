function main() {
  i = 1;
  printf(" n    n!\n");
  while (i <= 10) {
    printf("%2d    %d\n", i, fact(i));
  }
  return 0;
}

function fact(n) {
  i = 1;
  factorial = 1;
  while (i <= n) {
    factorial = factorial * i;
    i = i + 1;
  }
  return factorial;
}

