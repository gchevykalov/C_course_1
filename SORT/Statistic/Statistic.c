#include "..\SORTS\sorts.h"
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <locale.h>

#define REPEAT_COUNT 10
#define MIN_SIZE 1
#define MAX_SIZE 10001
#define SIZE_STEP 100

#ifndef _DEBUG
#define DEFINITION(name)
#define COPY(arr1, arr2, size)
#define FREECOPY(arr)
#define CHECK(arr1, arr2, size)
#else
#define DEFINITION(name) int* name = NULL
#define COPY(arr1, arr2, size) arr1 = _copyArr(arr2, size)
#define FREECOPY(arr) if (arr != NULL) {free(arr);}
#define CHECK(arr1, arr2, size) _check(arr1, arr2, size)
#endif

typedef sort_func_t* (*info_t)(int* count);

//������� ��������� �������
typedef enum {
  RAND_GENERATE_FIRST,
  RAND_GENERATE_NEW
} rand_generate_t;

//��������� �������
static int* _generateArr(int size, rand_generate_t condition) {
  int* array = malloc(size * sizeof(int));
  int counter = 0;
  
  if (array != NULL) {
    if (condition == RAND_GENERATE_FIRST) {
      srand(0);
    }
    for (counter = 0; counter < size; counter++) {
      *(array + counter) = (int)((double)rand() / RAND_MAX * 2147483647);
    }
  }
  return array;
}

//����������� �������
static int* _copyArr(int array[], int size) {
  int* arr = NULL;
  int counter = 0;
  
  if (array != NULL) {
    arr = malloc(size * sizeof(int));
    if (arr != NULL) {
      for (; counter < size; counter++) {
        *arr++ = *array++;
      }
      return arr - size;
    }
  }
  return arr;
}

//"���������" �����
int _compareIntegers(const int* first, const int* second) {
  return  *first - *second;;
}

//�������� ����������������� �������
static void _check(int array[], int arrayToCheck[], int size) {
  int counter = 0, correct = 1;
  
  if (array != NULL && arrayToCheck != NULL) {
    qsort(arrayToCheck, size, sizeof(int), &_compareIntegers);
    for (; counter < size; counter++) {
      if (array[counter] != arrayToCheck[counter])
        correct = 0;
    }
    if (!correct) {
      printf("ERROR: incorrect sort\n");
    }
  }
  else {
    printf("ERROR: can not to check\n");
  }
}

int main(void) {
  HMODULE h;
  wchar_t* dll = L"SORTS.dll";
  info_t getInfo;
  sort_info_t* sortInfo;
  int count, * array, size = 0, counter = 0, repeat = 0;
  DEFINITION(arrayToCheck);
  FILE* file = NULL;
  LARGE_INTEGER freq, count1, count2;
  double time = 0.0, averageTime = 0.0;
  
  setlocale(LC_NUMERIC, "Russian");
  QueryPerformanceFrequency(&freq);
  //��������� ���� ��� ����� ����������
  file = fopen("StatisticSorts.csv", "w");
  if (file != NULL) {
    //�������� ����������
    h = LoadLibrary(dll);
    if (!h) {
      printf("ERROR: dll was not found\n");
      return 0;
    }
    //��������� ��������� �� ������ � ����������� � �����������
    getInfo = (info_t)GetProcAddress(h, "GetSortList");
    sortInfo = (sort_info_t*)getInfo(&count);
    //�������� �������� �������
    fprintf(file, "Size;");
    for (counter = 0; counter < count; counter++) {
      fprintf(file, "%s;", sortInfo[counter].name);
    }
    fprintf(file, "\n");
    //��������� ���������� ��� ������ �������� ����� �������
    for (size = MIN_SIZE; size <= MAX_SIZE; size += SIZE_STEP) {
      fprintf(file, "%i;", size);
      //��������� ������ ���������� �� �������� ����� �������
      for (counter = 0; counter < count; counter++) {
        averageTime = 0;
        //��������� ������� � ���������� ���������
        array = _generateArr(size, RAND_GENERATE_FIRST);
        COPY(arrayToCheck, array, size);
        //��������� ������������ ���������� ��������� �������� ����� �����
        for (repeat = 0; repeat < REPEAT_COUNT; repeat++) {
          //��������� ����� �������
          QueryPerformanceCounter(&count1);
          sortInfo[counter].sort(array, size);
          //��������� ����� �������
          QueryPerformanceCounter(&count2);
          time = (count2.QuadPart - count1.QuadPart) / (double)freq.QuadPart;
          averageTime += time;
          //�������� ������������ ����������
          CHECK(array, arrayToCheck, size);
          //������� ��������
          if (array != NULL) {
            free(array);
          }
          FREECOPY(arrayToCheck)
            //��������� ������� � �������� ���������
            array = _generateArr(size, RAND_GENERATE_NEW);
          COPY(arrayToCheck, array, size);
        }
        averageTime /= REPEAT_COUNT;
        fprintf(file, "%g;", averageTime);
        //������� ��������
        if (array != NULL) {
          free(array);
        }
        FREECOPY(arrayToCheck)
      }
      fprintf(file, "\n");
    }
    //�������� ����� ����������
    fclose(file);
    //�������� ����������
    FreeLibrary(h);
  }
  else {
    printf("ERROR: file was not opened\n");
  }
  return 0;
}