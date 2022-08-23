#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "fixed.h"

//число проверок на функцию
#define CHECK 10

//функция чтения строки
static char* _readLine(FILE* inputStream) {
  int lengthStr = 0, size = 10;
  char* string = malloc(sizeof(char) * size);
  char symbol;
  
  if (string != NULL) {
    //посимвольное чтение до конца строки
    while ((symbol = (char)fgetc(inputStream)) != '\n' && symbol != EOF) {
      if (lengthStr + 1 == size) {
        char* tmp = realloc(string, 2 * size * sizeof(char));

        //пропустить конец строки, если памяти не хватает
        if (tmp == NULL) {
          while ((symbol = (char)fgetc(inputStream)) != '\n' && symbol != EOF);
          break;
        }
        size *= 2;
        string = tmp;
      }
      string[lengthStr++] = symbol;
    }
    //закончить строку
    if (string != NULL)
      string[lengthStr] = '\0';
  }
  //пропустить строку, если памяти не хватает
  else {
    while ((symbol = (char)fgetc(inputStream)) != '\n' && symbol != EOF);
  }

  return string;
}

//функция проверки создания по строке
int _fixedParseCheck(char const* str, fixed_t controlValue, int is) {
  char* end = NULL;
  fixed_t fixedValue;

  fixedValue = FixedParse(str, &end);
  if (fixedValue.f == controlValue.f) {
    if ((end != NULL && is) || (end == NULL && !is))
      return 1;
    else
      return 0;
  }
  else {
    return 0;
  }
}

//функция проверки записи в строку
int _fixedFormatCheck(fixed_t fixedValue, char const* controlValue) {
  char* fixedAsStr = NULL;

  fixedAsStr = FixedFormat(fixedValue);
  if (fixedAsStr != NULL) {
    if (strcmp(fixedAsStr, controlValue)) {
      free(fixedAsStr);
      return 0;
    }
    else {
      free(fixedAsStr);
      return 1;
    }
  }
  else {
    return -1;
  }
}

//функция проверки ZERO
int _fixedZEROCheck(fixed_t controlValue) {
  return ZERO == controlValue.f;
}

//многократная проверка преобразования из double
void _fromDoubleCheck(FILE* inputStream, FILE* controlStream) {
  int counter;
  double value = 0;
  fixed_t fixedControlValue = { ZERO };

  for (counter = 0; counter < CHECK; counter++) {
    fscanf_s(inputStream, "%lf", &value);
    printf("%g -> ", value);
    if (FixedRead(&fixedControlValue, controlStream)) {
      if (FixedFromDouble(value).f == fixedControlValue.f) {
        FixedWrite(fixedControlValue, stdout);
        printf("\n");
      }
      else {
        printf("Incorrect function behavior\n");
      }
    }
    else {
      printf("Failed to read control value\n");
    }
  }
}

//многократная проверка преобразования из int
void _fromIntCheck(FILE* inputStream, FILE* controlStream) {
  int counter;
  int intValue = 0;
  fixed_t fixedControlValue = { ZERO };
  
  for (counter = 0; counter < CHECK; counter++) {
    fscanf_s(inputStream, "%i", &intValue);
    printf("%i -> ", intValue);
    if (FixedRead(&fixedControlValue, controlStream)) {
      if (FixedFromInt(intValue).f == fixedControlValue.f) {
        FixedWrite(fixedControlValue, stdout);
        printf("\n");
      }
      else {
        printf("Incorrect function behavior\n");
      }
    }
    else {
      printf("Failed to read control value\n");
    }
  }
}

//многократная проверка преобразования в double
void _asDoubleCheck(FILE* inputStream, FILE* controlStream) {
  int counter;
  double controlValue = 0;
  fixed_t fixedValue = { ZERO };

  for (counter = 0; counter < CHECK; counter++) {
    fscanf_s(controlStream, "%lf", &controlValue);
    if (FixedRead(&fixedValue, inputStream)) {
      FixedWrite(fixedValue, stdout);
      printf(" -> ");
      if (FixedAsDouble(fixedValue) == controlValue) {
        printf("%g\n", controlValue);
      }
      else {
        printf("Incorrect function behavior\n");
      }
    }
    else {
      printf("Failed to read\n");
    }
  }
}

//многократная проверка преобразования в int
void _asIntCheck(FILE* inputStream, FILE* controlStream) {
  int counter;
  int intControlValue = 0;
  fixed_t fixedValue = { ZERO };

  for (counter = 0; counter < CHECK; counter++) {
    fscanf_s(controlStream, "%i", &intControlValue);
    if (FixedRead(&fixedValue, inputStream)) {
      FixedWrite(fixedValue, stdout);
      printf(" -> ");
      if (FixedAsInt(fixedValue) == intControlValue) {
        printf("%i\n", intControlValue);
      }
      else {
        printf("Incorrect function behavior\n");
      }
    }
    else {
      printf("Failed to read\n");
    }
  }
}

//многократная проверка преобразования из строки
void _parseCheck(FILE* inputStream, FILE* controlStream) {
  int counter, is = 0;
  char* line;
  fixed_t fixedControlValue = { ZERO };

  for (counter = 0; counter < CHECK; counter++) {
    line = _readLine(inputStream);
    fscanf_s(controlStream, "%i", &is);
    if (line != NULL) {
      printf("%s -> ", line);
      if (FixedRead(&fixedControlValue, controlStream)) {
        if (_fixedParseCheck(line, fixedControlValue, is)) {
          if (is)
            FixedWrite(fixedControlValue, stdout);
          else
            printf("NONE");
          printf("\n");
        }
        else {
          printf("Incorrect function behavior\n");
        }
      }
      else {
        printf("Failed to read control value\n");
      }
      free(line);
    }
    else {
      printf("Failed to read\n");
    }
  }
}

//многократная проверка преобразования в строку
void _formatCheck(FILE* inputStream, FILE* controlStream) {
  int counter;
  char* line;
  fixed_t fixedValue = { ZERO };
  
  for (counter = 0; counter < CHECK; counter++) {
    line = _readLine(controlStream);
    if (line != NULL) {
      if (FixedRead(&fixedValue, inputStream)) {
        FixedWrite(fixedValue, stdout);
        printf(" -> ");
        if (_fixedFormatCheck(fixedValue, line) == 1) {
          printf("%s\n", line);
        }
        else if (_fixedFormatCheck(fixedValue, line) == 0) {
          printf("Incorrect function behavior\n");
        }
        else {
          printf("Failed to create string\n");
        }
      }
      else {
        printf("Failed to read\n");
      }
      free(line);
    }
    else {
      printf("Failed to read control value\n");
    }
  }
}

//многократная проверка сложения
void _addCheck(FILE* inputStream, FILE* controlStream) {
  int counter;
  fixed_t fixedValue = { ZERO }, fixedControlValue = { ZERO }, fixedValue2 = { ZERO };

  for (counter = 0; counter < CHECK; counter++) {
    if (FixedRead(&fixedControlValue, controlStream)) {
      if (FixedRead(&fixedValue, inputStream) && FixedRead(&fixedValue2, inputStream)) {
        FixedWrite(fixedValue, stdout);
        printf(" + ");
        FixedWrite(fixedValue2, stdout);
        printf(" = ");
        if (FixedAdd(fixedValue, fixedValue2).f == fixedControlValue.f) {
          FixedWrite(fixedControlValue, stdout);
          printf("\n");
        }
        else {
          printf("Incorrect function behavior\n");
        }
      }
      else {
        printf("Failed to read\n");
      }
    }
    else {
      printf("Failed to read control value\n");
    }
  }
}

//многократная проверка вычитания
void _subCheck(FILE* inputStream, FILE* controlStream) {
  int counter;
  fixed_t fixedValue = { ZERO }, fixedControlValue = { ZERO }, fixedValue2 = { ZERO };

  for (counter = 0; counter < CHECK; counter++) {
    if (FixedRead(&fixedControlValue, controlStream)) {
      if (FixedRead(&fixedValue, inputStream) && FixedRead(&fixedValue2, inputStream)) {
        FixedWrite(fixedValue, stdout);
        printf(" - ");
        FixedWrite(fixedValue2, stdout);
        printf(" = ");
        if (FixedSub(fixedValue, fixedValue2).f == fixedControlValue.f) {
          FixedWrite(fixedControlValue, stdout);
          printf("\n");
        }
        else {
          printf("Incorrect function behavior\n");
        }
      }
      else {
        printf("Failed to read\n");
      }
    }
    else {
      printf("Failed to read control value\n");
    }
  }
}

//многократная проверка умножения
void _mulCheck(FILE* inputStream, FILE* controlStream) {
  int counter;
  fixed_t fixedValue = { ZERO }, fixedControlValue = { ZERO }, fixedValue2 = { ZERO };

  for (counter = 0; counter < CHECK; counter++) {
    if (FixedRead(&fixedControlValue, controlStream)) {
      if (FixedRead(&fixedValue, inputStream) && FixedRead(&fixedValue2, inputStream)) {
        FixedWrite(fixedValue, stdout);
        printf(" * ");
        FixedWrite(fixedValue2, stdout);
        printf(" = ");
        if (FixedMul(fixedValue, fixedValue2).f == fixedControlValue.f) {
          FixedWrite(fixedControlValue, stdout);
          printf("\n");
        }
        else {
          printf("Incorrect function behavior\n");
        }
      }
      else {
        printf("Failed to read\n");
      }
    }
    else {
      printf("Failed to read control value\n");
    }
  }
}

//многократная проверка деления
void _divCheck(FILE* inputStream, FILE* controlStream) {
  int counter;
  fixed_t fixedValue = { ZERO }, fixedControlValue = { ZERO }, fixedValue2 = { ZERO };

  for (counter = 0; counter < CHECK; counter++) {
    if (FixedRead(&fixedControlValue, controlStream)) {
      if (FixedRead(&fixedValue, inputStream) && FixedRead(&fixedValue2, inputStream)) {
        FixedWrite(fixedValue, stdout);
        printf(" / ");
        FixedWrite(fixedValue2, stdout);
        printf(" = ");
        if (FixedDiv(fixedValue, fixedValue2).f == fixedControlValue.f) {
          FixedWrite(fixedControlValue, stdout);
          printf("\n");
        }
        else {
          printf("Incorrect function behavior\n");
        }
      }
      else {
        printf("Failed to read\n");
      }
    }
    else {
      printf("Failed to read control value\n");
    }
  }
}

//многократная проверка сравнения
void _compareCheck(FILE* inputStream, FILE* controlStream) {
  int counter;
  int intControlValue = 0;
  fixed_t fixedValue = { ZERO }, fixedValue2 = { ZERO };

  for (counter = 0; counter < CHECK; counter++) {
    fscanf_s(controlStream, "%i", &intControlValue);
    if (FixedRead(&fixedValue, inputStream) && FixedRead(&fixedValue2, inputStream)) {
      FixedWrite(fixedValue, stdout);
      printf(" , ");
      FixedWrite(fixedValue2, stdout);
      printf(" -> ");
      if (FixedCompare(&fixedValue, &fixedValue2) == intControlValue) {
        printf("%i\n", intControlValue);
      }
      else {
        printf("Incorrect function behavior\n");
      }
    }
    else {
      printf("Failed to read\n");
    }
  }
}

//многократная проверка противоположных чисел
void _negateCheck(FILE* inputStream, FILE* controlStream) {
  int counter;
  fixed_t fixedValue = { ZERO }, fixedControlValue = { ZERO };

  for (counter = 0; counter < CHECK; counter++) {
    if (FixedRead(&fixedControlValue, controlStream)) {
      if (FixedRead(&fixedValue, inputStream)) {
        FixedWrite(fixedValue, stdout);
        printf(" -> ");
        if (FixedNegate(fixedValue).f == fixedControlValue.f) {
          FixedWrite(fixedControlValue, stdout);
          printf("\n");
        }
        else {
          printf("Incorrect function behavior\n");
        }
      }
      else {
        printf("Failed to read\n");
      }
    }
    else {
      printf("Failed to read control value\n");
    }
  }
}

//многократная проверка обратных чисел
void _reciprocalCheck(FILE* inputStream, FILE* controlStream) {
  int counter;
  fixed_t fixedValue = { ZERO }, fixedControlValue = { ZERO };

  for (counter = 0; counter < CHECK; counter++) {
    if (FixedRead(&fixedControlValue, controlStream)) {
      if (FixedRead(&fixedValue, inputStream)) {
        FixedWrite(fixedValue, stdout);
        printf(" -> ");
        if (FixedReciprocal(fixedValue).f == fixedControlValue.f) {
          FixedWrite(fixedControlValue, stdout);
          printf("\n");
        }
        else {
          printf("Incorrect function behavior\n");
        }
      }
      else {
        printf("Failed to read\n");
      }
    }
    else {
      printf("Failed to read control value\n");
    }
  }
}

//многократная проверка модуля
void _absCheck(FILE* inputStream, FILE* controlStream) {
  int counter;
  fixed_t fixedValue = { ZERO }, fixedControlValue = { ZERO };

  for (counter = 0; counter < CHECK; counter++) {
    if (FixedRead(&fixedControlValue, controlStream)) {
      if (FixedRead(&fixedValue, inputStream)) {
        FixedWrite(fixedValue, stdout);
        printf(" -> ");
        if (FixedAbs(fixedValue).f == fixedControlValue.f) {
          FixedWrite(fixedControlValue, stdout);
          printf("\n");
        }
        else {
          printf("Incorrect function behavior\n");
        }
      }
      else {
        printf("Failed to read\n");
      }
    }
    else {
      printf("Failed to read control value\n");
    }
  }
}

//многократная проверка округления вниз
void _floorCheck(FILE* inputStream, FILE* controlStream) {
  int counter;
  fixed_t fixedValue = { ZERO }, fixedControlValue = { ZERO };

  for (counter = 0; counter < CHECK; counter++) {
    if (FixedRead(&fixedControlValue, controlStream)) {
      if (FixedRead(&fixedValue, inputStream)) {
        FixedWrite(fixedValue, stdout);
        printf(" -> ");
        if (FixedFloor(fixedValue).f == fixedControlValue.f) {
          FixedWrite(fixedControlValue, stdout);
          printf("\n");
        }
        else {
          printf("Incorrect function behavior\n");
        }
      }
      else {
        printf("Failed to read\n");
      }
    }
    else {
      printf("Failed to read control value\n");
    }
  }
}

//многократная проверка округления вверх
void _ceilCheck(FILE* inputStream, FILE* controlStream) {
  int counter;
  fixed_t fixedValue = { ZERO }, fixedControlValue = { ZERO };

  for (counter = 0; counter < CHECK; counter++) {
    if (FixedRead(&fixedControlValue, controlStream)) {
      if (FixedRead(&fixedValue, inputStream)) {
        FixedWrite(fixedValue, stdout);
        printf(" -> ");
        if (FixedCeil(fixedValue).f == fixedControlValue.f) {
          FixedWrite(fixedControlValue, stdout);
          printf("\n");
        }
        else {
          printf("Incorrect function behavior\n");
        }
      }
      else {
        printf("Failed to read\n");
      }
    }
    else {
      printf("Failed to read control value\n");
    }
  }
}

//многократная проверка математического округления
void _roundCheck(FILE* inputStream, FILE* controlStream) {
  int counter;
  fixed_t fixedValue = { ZERO }, fixedControlValue = { ZERO };

  for (counter = 0; counter < CHECK; counter++) {
    if (FixedRead(&fixedControlValue, controlStream)) {
      if (FixedRead(&fixedValue, inputStream)) {
        FixedWrite(fixedValue, stdout);
        printf(" -> ");
        if (FixedRound(fixedValue).f == fixedControlValue.f) {
          FixedWrite(fixedControlValue, stdout);
          printf("\n");
        }
        else {
          printf("Incorrect function behavior\n");
        }
      }
      else {
        printf("Failed to read\n");
      }
    }
    else {
      printf("Failed to read control value\n");
    }
  }
}

int main(void) {
  FILE *inputStream, *controlStream;
  fixed_t fixedControlValue = { ZERO }, fixedValue = { ZERO };
  
  //открытие файлов
  inputStream = fopen("test.txt", "r");
  controlStream = fopen("control.txt", "r");
  //проверка потоков
  if (inputStream == NULL || controlStream == NULL) {
    printf("ERROR: stream error\n");
    if (inputStream != NULL)
      fclose(inputStream);
    if (controlStream != NULL)
      fclose(controlStream);
  }
  else {
    //многократная проверка преобразования из double
    printf("FixedFromDouble:\n");
    _fromDoubleCheck(inputStream, controlStream);
    printf("\n");
    //многократная проверка преобразования из int
    printf("FixedFromInt:\n");
    _fromIntCheck(inputStream, controlStream);
    printf("\n");
    //многократная проверка преобразования в double
    printf("FixedAsDouble:\n");
    _asDoubleCheck(inputStream, controlStream);
    printf("\n");
    //многократная проверка преобразования в int
    printf("FixedAsInt:\n");
    _asIntCheck(inputStream, controlStream);
    printf("\n");
    //многократная проверка преобразования из строки
    printf("FixedParse:\n");
    _parseCheck(inputStream, controlStream);
    printf("\n");
    //многократная проверка преобразования в строку
    printf("FixedFormat:\n");
    _formatCheck(inputStream, controlStream);
    printf("\n");
    //многократная проверка сложения
    printf("FixedAdd:\n");
    _addCheck(inputStream, controlStream);
    printf("\n");
    //многократная проверка вычитания
    printf("FixedSub:\n");
    _subCheck(inputStream, controlStream);
    printf("\n");
    //многократная проверка умножения
    printf("FixedMul:\n");
    _mulCheck(inputStream, controlStream);
    printf("\n");
    //многократная проверка деления
    printf("FixedDiv:\n");
    _divCheck(inputStream, controlStream);
    printf("\n");
    //многократная проверка сравнения
    printf("FixedCompare:\n");
    _compareCheck(inputStream, controlStream);
    printf("\n");
    //многократная проверка противоположных чисел
    printf("FixedNegate:\n");
    _negateCheck(inputStream, controlStream);
    printf("\n");
    //многократная проверка обратных чисел
    printf("FixedReciprocal:\n");
    _reciprocalCheck(inputStream, controlStream);
    printf("\n");
    //многократная проверка модуля
    printf("FixedAbs:\n");
    _absCheck(inputStream, controlStream);
    printf("\n");
    //многократная проверка округления вниз
    printf("FixedFloor:\n");
    _floorCheck(inputStream, controlStream);
    printf("\n");
    //многократная проверка округления вверх
    printf("FixedCeil:\n");
    _ceilCheck(inputStream, controlStream);
    printf("\n");
    //многократная проверка математического округления
    printf("FixedRound:\n");
    _roundCheck(inputStream, controlStream);
    printf("\n");
    //проверка нулевого значения
    printf("ZERO:\n");
    if (FixedRead(&fixedControlValue, controlStream)) {
      FixedWrite(fixedValue, stdout);
      printf(" == ");
      if (_fixedZEROCheck(fixedControlValue)) {
        FixedWrite(fixedControlValue, stdout);
        printf("\n");
      }
      else {
        printf("Incorrect function behavior\n");
      }
    }
    else {
      printf("Failed to read control value\n");
    }
    //закрытие файлов
    fclose(inputStream);
    fclose(controlStream);
  }
  return 0;
}