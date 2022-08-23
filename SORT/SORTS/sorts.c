#include "pch.h"
#include "sorts.h"
#include <stdlib.h>

#define VALUES_DIG 10
#define VALUES_BIN 2
#define DIGITS 10
#define DIGITS_BIN 32
#define MASK_BIN 1
#define VALUES_B 256
#define DIGITS_B 4
#define MASK_B 255

static void _bucketSort(int array[], int size) {
  int* count = calloc(size, sizeof(int)), * b = malloc(size * sizeof(int));
  int counter, digit, max = 0;
  int newElement, location;
  
  if (array != NULL && count != NULL && b != NULL) {
    for (counter = 0; counter < size; counter++) {
      if (max < array[counter])
        max = array[counter];
    }
    for (counter = 0; counter < size; counter++) {
      count[(int)(array[counter] / ((double)max + 1) * size)]++;
    }
    for (counter = 1; counter < size; counter++) {
      count[counter] += count[counter - 1];
    }
    for (counter = size - 1; counter >= 0; counter--) {
      digit = (int)(array[counter] / ((double)max + 1) * size);
      *(b - 1 + count[digit]) = array[counter];
      count[digit]--;
    }
    for (counter = 1; counter < size; counter++) {
      newElement = *(b + counter);
      location = counter - 1;
      while (location >= 0 && *(b + location) > newElement) {
        *(b + location + 1) = *(b + location);
        location--;
      }
      *(b + location + 1) = newElement;
    }
    for (counter = 0; counter < size; counter++) {
      *(array + counter) = *(b + counter);
    }
  }
  if (count != NULL) {
    free(count);
  }
  if (b != NULL) {
    free(b);
  }
}

static void _radixSort10(int array[], int size) {
  int auxiliary[VALUES_DIG], * b = malloc(size * sizeof(int));
  int digit = 0;
  int counter = 0;
  int degree = 1;
  int max = 0;
  
  if (array != NULL && b != NULL) {
    for (counter = 0; counter < size; counter++) {
      if (max < array[counter])
        max = array[counter];
    }
    for (; max / degree > 0; degree *= 10) {
      for (counter = 0; counter < VALUES_DIG; counter++) {
        auxiliary[counter] = 0;
      }
      for (counter = 0; counter < size; counter++) {
        auxiliary[(array[counter] / degree) % VALUES_DIG]++;
      }
      for (counter = 1; counter < VALUES_DIG; counter++) {
        auxiliary[counter] += auxiliary[counter - 1];
      }
      for (counter = size - 1; counter >= 0; counter--) {
        digit = (array[counter] / degree) % VALUES_DIG;
        *(b - 1 + auxiliary[digit]) = array[counter];
        auxiliary[digit]--;
      }
      for (counter = 0; counter < size; counter++) {
        *(array + counter) = *(b + counter);
      }
    }
  }
  if (b != NULL) {
    free(b);
  }
}

static void _radixSort2(int array[], int size) {
  int auxiliary[VALUES_BIN], *b = malloc(size * sizeof(int));
  int digit = 0;
  int counter = 0;
  int degree = 0;
  int max = 0;
  
  if (array != NULL && b != NULL) {
    for (counter = 0; counter < size; counter++) {
      if (max < array[counter])
        max = array[counter];
    }
    for (; (max >> degree) > 0; degree++) {
      for (counter = 0; counter < VALUES_BIN; counter++) {
        auxiliary[counter] = 0;
      }
      for (counter = 0; counter < size; counter++) {
        auxiliary[(array[counter] >> degree) & MASK_BIN]++;
      }
      for (counter = 1; counter < VALUES_BIN; counter++) {
        auxiliary[counter] += auxiliary[counter - 1];
      }
      for (counter = size - 1; counter >= 0; counter--) {
        digit = (array[counter] >> degree) & MASK_BIN;
        *(b - 1 + auxiliary[digit]) = array[counter];
        auxiliary[digit]--;
      }
      for (counter = 0; counter < size; counter++) {
        *(array + counter) = *(b + counter);
      }
    }
  }
  if (b != NULL) {
    free(b);
  }
}

static void _radixSort256(int array[], int size) {
  int auxiliary[VALUES_B], *b = malloc(size * sizeof(int));
  int digit = 0;
  int counter = 0;
  int degree = 0;
  int max = 0;
  int p = 1;
  
  if (array != NULL && b != NULL) {
    for (counter = 0; counter < size; counter++) {
      if (max < array[counter])
        max = array[counter];
    }
    for (degree = 0; (max >> degree) > 0; p++) {
      for (counter = 0; counter < VALUES_B; counter++) {
        auxiliary[counter] = 0;
      }
      for (counter = 0; counter < size; counter++) {
        auxiliary[(array[counter] >> degree) & MASK_B]++;
      }
      for (counter = 1; counter < VALUES_B; counter++) {
        auxiliary[counter] += auxiliary[counter - 1];
      }
      for (counter = size - 1; counter >= 0; counter--) {
        digit = (array[counter] >> degree) & MASK_B;
        *(b - 1 + auxiliary[digit]) = array[counter];
        auxiliary[digit]--;
      }
      for (counter = 0; counter < size; counter++) {
        *(array + counter) = *(b + counter);
      }
      if (p != 4) {
        degree += 8;
      }
      else {
        degree += 7;
      }
    }
  }
  if (b != NULL) {
    free(b);
  }
}

static sort_info_t s_sortList[] = 
{
  {_bucketSort, SORT_BUCKET, SORT_LINEAR, "bucket" },
  {_radixSort10, SORT_RADIX, SORT_LINEAR, "radix10" },
  {_radixSort2, SORT_RADIX, SORT_LINEAR, "radix2" },
  {_radixSort256, SORT_RADIX, SORT_LINEAR, "radix256" }
};

__declspec(dllexport) sort_info_t* GetSortList(int* count) {
   *count = sizeof(s_sortList) / sizeof(s_sortList[0]);
   return s_sortList;
}