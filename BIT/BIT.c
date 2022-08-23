#include <stdio.h>
#include <locale.h>

typedef unsigned long long integer_t;

inline int lengthNumber(integer_t number) {
  int k = 1;

  while (number /= 10)
    ++k;

  return k;
}

void printBits(integer_t number) {
  integer_t i = ((integer_t) 1) << (sizeof(integer_t) * 8 - 1);
  int k = 0;

  while (1) {
    if (k % 8 == 0)
      printf(" ");
    if (number & i)
      printf("1");
    else
      printf("0");
    if (i == 1)
      break;
    i >>= 1;
    ++k;
  }
  printf("\n");
}

integer_t process(integer_t number, int n) {
  integer_t number2 = 0, b0 = 0, b1 = 0;
  int size = sizeof(integer_t);

  for (int i = 1; i <= size; ++i) {
    b0 = (number >> ((size - i) * 8)) & 255;
    b1 = ((b0 >> n) | (b0 << (8 - n))) & 255;
    number2 <<= 8;
    number2 = number2 | b1;
  }

  return  number2;
}

void printResult(integer_t number, int n) {
  int k, l1, l2, size = sizeof(integer_t) * 2;
  integer_t number2 = process(number, n);

  l1 = lengthNumber(number);
  l2 = lengthNumber(number2);
  if (l1 > l2)
    k = l1;
  else
    k = l2;

  printf("Вы ввели   : %*llu = 0x%0*llX =", k, number, size, number);
  printBits(number);
  printf("Результат  : %*llu = 0x%0*llX =", k, number2, size, number2);
  printBits(number2);
}

int main(void) {
  integer_t a;
  int n;

  setlocale(LC_CTYPE, "Russian");
  printf("Задание BIT-11: Циклический сдвиг вправо внутри байтов\nСигнал к завершению - ввод 0 и произвольного сдвига.\n");
  while (1) {
    printf("Введите число и сдвиг: ");
    scanf_s("%llu %i", &a, &n);
    if (!a)
      break;
    n = n % 8;
    printResult(a, n);
  }
  return 0;
}