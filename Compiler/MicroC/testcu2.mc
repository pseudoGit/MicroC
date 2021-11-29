function main() {
  i = 1;
  sum = 0;
  while (i < 10) {
    sum = add(sum, i);
    printf("i = %d sum = %d\n", i, sum);
    i = add(i, 1);
  }
  return 0;
}

function add(a1, a2) {
  a = a1 + a2;
  return a;
}

