#include "Abuelfateh.h"

double getDouble() {
  double num = 0.0;
  int d = 10;
  char c;
  bool isNegative = false, hasFraction = false;
  while (13 != (c = _getch()) && '\n' != c && EOF != c) {
    if (!isNegative && !hasFraction && 10 == d && 0.0 == num && '-' == c) {
      isNegative = true;
      printf("-");
    }
    else if (!hasFraction && '.' == c) {
      hasFraction = true;
      printf(".");
    }
    else if ('0' <= c && '9' >= c) {
      c -= '0';
      if (hasFraction) {
        num += (double)((double)c / d);
        d *= 10;
      }
      else {
        num *= 10;
        num += (double)c;
      }
      printf("%d", c);
    }
  }
  printf("\n");
  return isNegative && num > 0.0 ? -num : num;
}

double toDouble(const char *str) {
  double num = 0.0;
  int i = 0, d = 1;
  char c;
  bool hasFraction = false, isNegative = false;

  while (str[i] && (' ' == str[i] || '\t' == str[i]))
    i++;

  if ('-' == str[i]) {
    i++;
    isNegative = true;
  }

  while (str[i]) {
    if (!hasFraction && '.' == str[i])
      hasFraction = true;
    else if ('0' <= str[i] && '9' >= str[i]) {
      c = str[i] - '0';
      if (hasFraction) {
        d *= 10;
        num = num + (double)((double)c / d);
      }
      else {
        num = num * 10;
        num = num + (double)c;
      }
    }
    i++;
  }

  return isNegative ? -num : num;
}

double *toDoubleArray(const char *str, int &len) {
  double *arr = nullptr;
  char num[101];
  int i, j, c;
  len = 0;
  for (i = 0; str[i]; i++) {
    if (',' == str[i])
      len++;
  }

  if (len > 0) {
    len++;
    arr = new double[len];

    for (c = 0, i = 0; c < len; c++) {
      j = 0;
      while (str[i] && j < 100 && ',' != str[i])
        num[j++] = str[i++];

      i++;
      num[j] = '\0';
      arr[c] = toDouble(num);
    }
  }

  return arr;
}
