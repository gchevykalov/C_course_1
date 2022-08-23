#include "pch.h"
#include "framework.h"
#include <math.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "fixed.h"

#define COEFFICIENT 65536
#define FRACT 65535
#define INT 4294901760
#define SHIFT 16
#define MAIN 281474976710655
#define MAXLENGTH 30

fixed_t FixedFromInt(int value) {
  fixed_t fixedValue;

  fixedValue.f = value << SHIFT;
  return fixedValue;
}

int FixedAsInt(fixed_t value) {
  return value.f >> SHIFT;
}

fixed_t  FixedFromDouble(double value) {
  fixed_t fixedValue;

  fixedValue.f = (int)floor(value * COEFFICIENT + 0.5);
  return fixedValue;
}

double FixedAsDouble(fixed_t value) {
  return (double)value.f / COEFFICIENT;
}

fixed_t FixedAdd(fixed_t value1, fixed_t value2) {
  value1.f = value1.f + value2.f;
  return value1;
}

fixed_t FixedSub(fixed_t value1, fixed_t value2) {
  value1.f = value1.f - value2.f;
  return value1;
}

fixed_t FixedMul(fixed_t value1, fixed_t value2) {
  if ((((long long)value1.f * value2.f) >> SHIFT) >= ((long long)COEFFICIENT << SHIFT) ||
      (((long long)value1.f * value2.f) >> SHIFT) <= -((long long)COEFFICIENT << SHIFT)) {
    value1.f = COEFFICIENT << (SHIFT - 1);
    return value1;
  }
  else {
    value1.f = ((long long)value1.f * value2.f) >> SHIFT;
    return value1;
  }
}

fixed_t FixedDiv(fixed_t value1, fixed_t value2) {
  if (value2.f != 0) {
    if ((value1.f = (((long long)value1.f << (2 * SHIFT)) / value2.f) >> SHIFT) > 0)
      return value1;
    else {
      value1.f++;
      return value1;
    }
  }
  else {
    value1.f = COEFFICIENT << (SHIFT - 1);
    return value1;
  }
}

fixed_t FixedNegate(fixed_t value) {
  value.f = -value.f;
  return value;
}

fixed_t FixedReciprocal(fixed_t value) {
  if (value.f != 0) {
    if ((value.f = (int)((((long long)COEFFICIENT << (2 * SHIFT)) / value.f) >> SHIFT)) > 0)
      return value;
    else {
      value.f++;
      return value;
    }
  }
  else {
    value.f = COEFFICIENT << (SHIFT - 1);
    return value;
  }
}

fixed_t FixedAbs(fixed_t value) {
  if (value.f >= 0)
    return value;
  else {
    value.f = -value.f;
    return value;
  }
}

fixed_t FixedFloor(fixed_t value) {
  value.f = value.f & INT;
  return value;
}

fixed_t FixedCeil(fixed_t value) {
  if ((value.f & FRACT) > 0) {
    value.f = (value.f & INT) + COEFFICIENT;
    return value;
  }
  else
    return value;
}

fixed_t FixedRound(fixed_t value) {
  if ((value.f & FRACT) > (COEFFICIENT >> 1)) {
    value.f = (value.f & INT) + COEFFICIENT;
    return value;
  }
  else {
    value.f = value.f & INT;
    return value;
  }
}

int FixedCompare(fixed_t const* value1, fixed_t const* value2) {
  if (value1->f < value2->f)
    return -1;
  else if (value1->f == value2->f)
    return 0;
  else
    return 1;
}

fixed_t FixedParse(char const* str, char const** end) {
  fixed_t fixedValue = { ZERO };
  char const* decorOpen = "Fix[", * decorClose = "]", * decor;
  int isDecored, order1 = 1, sign = 1;
  long long intPart = 0, order2 = 1, fract = 0;
  char* str0 = "1234567890";
  
  *end = NULL;
  while (*str != '\0') {
    decor = decorOpen;
    if ((*str > -2 && isalpha(*str)) && (*(str - 1) < -1 || !isalpha(*(str - 1)))) {
      isDecored = 1;
      while (*decor != '\0') {
        if (*decor == *str) {
          decor++;
          str++;
        }
        else {
          isDecored = 0;
          str++;
          break;
        }
      }
      if (*str > -2 && (*str == '-' || isdigit(*str)) && isDecored) {
        if (*str == '-') {
          sign = -1;
          str++;
        }
        order1 = strspn(str, str0);
        if (order1 != 0) {
          while (*str > -2 && isdigit(*str)) {
            intPart *= 10;
            intPart += *str - '0';
            str++;
          }
        }
        if (*str == '.') {
          str++;
        }
        order2 = strspn(str, str0);
        if (order2 != 0) {
          order2 = 1;
          while (*str > -2 && isdigit(*str)) {
            fract *= 10;
            fract += *str - '0';
            order2 *= 10;
            str++;
          }
          while ((fract & ~MAIN) != 0) {
            fract /= 10;
            order2 /= 10;
          }
        }
        if ((order1 != 0 || order2 != 0) && *str == *decorClose) {
          if (order2 == 0)
            order2 = 10;
          if ((unsigned long long)(fract << SHIFT) / (order2 / 10) - ((unsigned long long)(fract << SHIFT) / order2) * 10 >= 5) {
            fixedValue.f = sign * (int)((intPart << SHIFT) + (unsigned long long)(fract << SHIFT) / order2 + 1);
          }
          else {
            fixedValue.f = sign * (int)((intPart << SHIFT) + (unsigned long long)(fract << SHIFT) / order2);
          }
          str++;
          *end = str;
        }
      }
    }
    else {
      str++;
    }
  }
  return fixedValue;
}

static char* ValueAsStr(fixed_t fixedValue, int* length) {
  int lengthStr = 0, counter = 0, sign = 1;
  int fractPart = 0, intPart = 0;
  char str[MAXLENGTH], * valueAsStr = NULL;
  
  if (fixedValue.f < 0) {
    fixedValue.f = -fixedValue.f;
    sign = -1;
  }
  intPart = (fixedValue.f & INT) >> SHIFT;
  fractPart = fixedValue.f & FRACT;
  do {
    str[lengthStr++] = (char)(intPart % 10) + '0';
    intPart /= 10;
  } while (intPart);
  while (counter < lengthStr / 2) {
    str[MAXLENGTH - 1] = str[counter];
    str[counter] = str[lengthStr - 1 - counter];
    str[lengthStr - 1 - counter] = str[MAXLENGTH - 1];
    counter++;
  }
  if (fractPart)
    str[lengthStr++] = '.';
  while (fractPart) {
    str[lengthStr++] = (char)((fractPart * 10) >> SHIFT) + '0';
    fractPart = (fractPart * 10) & FRACT;
  }
  str[lengthStr] = '\0';
  counter = 0;
  if (sign > 0) {
    valueAsStr = malloc(sizeof(char) * (lengthStr + 1));
    if (valueAsStr != NULL) {
      while (str[counter] != 0) {
        *(valueAsStr + counter) = str[counter++];
      }
      *(valueAsStr + counter) = '\0';
      *length = counter;
    }
    else {
      return valueAsStr;
    }
  }
  else {
    valueAsStr = malloc(sizeof(char) * (lengthStr + 2));
    if (valueAsStr != NULL) {
      *valueAsStr = '-';
      while (str[counter] != 0) {
        *(valueAsStr + 1 + counter) = (str[counter++]);
      }
      *(valueAsStr + 1 + counter) = '\0';
      *length = counter + 1;
    }
    else {
      return valueAsStr;
    }
  }
  return valueAsStr;
}

char* FixedFormat(fixed_t fixedValue) {
  char const* decorOpen = "Fix[", * decorClose = "]";
  char* fixedAsStr = NULL, * valueAsStr = NULL;
  int length = 0;
  int counter = 0;
  
  valueAsStr = ValueAsStr(fixedValue, &length);
  if (valueAsStr != NULL) {
    fixedAsStr = malloc((6 + length) * sizeof(char));
    if (fixedAsStr != NULL) {
      while (*decorOpen != 0) {
        *(fixedAsStr + counter) = *(decorOpen++);
        counter++;
      }
      while (*valueAsStr != 0) {
        *(fixedAsStr + counter) = *(valueAsStr++);
        counter++;
      }
      while (*decorClose != 0) {
        *(fixedAsStr + counter) = *(decorClose++);
        counter++;
      }
      *(fixedAsStr + counter) = '\0';
      valueAsStr -= length;
      free(valueAsStr);
    }
    else {
      free(valueAsStr);
      return fixedAsStr;
    }
  }
  else {
    return fixedAsStr;
  }
  return fixedAsStr;
}

int FixedRead(fixed_t* value, FILE* stream) {
  char *string = malloc(sizeof(char)), *end;
  int lengthStr = 0;
  char symbol;
  
  value->f = ZERO;
  if (stream != NULL) {
    if (string != NULL) {
      while ((symbol = (char)fgetc(stream)) > -1 && isspace(symbol));
      if (symbol == EOF)
        return 0;
      while (symbol < -1 || (!isspace(symbol) && symbol != EOF)) {
        string[lengthStr] = symbol;
        string = realloc(string, (2 + (lengthStr++)) * sizeof(char));
        if (string == NULL) {
          while ((symbol = (char)fgetc(stream)) < -1 || (!isspace(symbol) && symbol != EOF))
            lengthStr++;
          return 0;
        }
        symbol = (char)fgetc(stream);
      }
      string[lengthStr] = '\0';
      *value = FixedParse(string, &end);
      free(string);
      if (end == NULL) {
        return 0;
      }
      else {
        return 1;
      }
    }
    else {
      return 0;
    }
  }
  else {
    return 0;
  }
}

int FixedWrite(fixed_t value, FILE* stream) {
  char* str;
  
  if (stream != NULL) {
    str = FixedFormat(value);
    fprintf(stream, "%s", str);
    free(str);
    return 1;
  }
  else {
    return 0;
  }
}