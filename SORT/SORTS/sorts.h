#ifndef SORT_H_INCLUDED_9asd923bcd76
#define SORT_H_INCLUDED_9asd923bcd76
#pragma once

typedef void (*sort_func_t)(int array[], int size);

typedef enum
{
  SORT_FAMILY_NA = -1,

  SORT_SELECTION,
  SORT_INSERTION,
  SORT_BUBBLE,
  SORT_SHELL,
  SORT_COMB,
  SORT_MERGE,
  SORT_QUICK,
  SORT_HEAP,
  SORT_COUNT,
  SORT_RADIX,
  SORT_BUCKET,

  SORT_FAMILY_COUNT
} sort_family_t;

typedef enum
{
  SORT_COMPLEXITY_NA = -1,

  SORT_QUADRATIC,
  SORT_SUBQUADRATIC,
  SORT_QUASILINEAR,
  SORT_LINEAR,

  SORT_COMPLEXITY_COUNT
} sort_complexity_t;

typedef struct
{
  sort_func_t sort;
  sort_family_t family;
  sort_complexity_t complexity;
  char const *name;
} sort_info_t;

#endif // SORT_H_INCLUDED_9asd923bcd76
