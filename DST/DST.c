#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define NUMLET 2

typedef struct {
  char* word;
  size_t len;
} elem_t;

char* readLine(void) {
  size_t lengthStr = 0, size = 10;
  char ch, * str = malloc(sizeof(char) * size), * str0;

  if (str == NULL) {
    printf("Не удалось выделить память для сохранения строки");
    return NULL;
  }

  while ((ch = getchar()) != '\n') {
    if (lengthStr + 1 == size) {
      str0 = realloc(str, 2 * size * sizeof(char));
      size *= 2;
      if (str0 == NULL) {
        free(str);
        printf("Не удалось выделить память для сохранения строки");
        return NULL;
      }
      else {
        str = str0;
      }
    }
    str[lengthStr++] = ch;
  }

  if (str)
    str[lengthStr] = '\0';

  return str;
}

inline int isAlpha(char sumbol) {
  return sumbol <= 'z' && sumbol >= 'a' || sumbol <= 'Z' && sumbol >= 'A'
    || sumbol <= 'я' && sumbol >= 'а' || sumbol <= 'Я' && sumbol >= 'А';
}

void freeWords(elem_t* words, int size) {
  if (words == NULL)
    return;

  for (int i = 0; i < size; ++i)
    free(words[i].word);
  free(words);

  words = NULL;
}

int checkChain(elem_t const* chains, int num, elem_t chain) {
  for (int i = 0; i < num; ++i) {
    if (chains[i].len == chain.len) {
      int j = 0;
      while (chains[i].word[j] == chain.word[j++]) {
        if (j == chain.len)
          return 1;
      }
    }
  }

  return 0;
}

int addNew(elem_t** words, int* size, int* num, elem_t newWord) {
  if (words == NULL || *words == NULL)
    return 0;

  if (*num == *size) {
    elem_t* wordsTmp = realloc(*words, sizeof(elem_t) * 2 * (*size));
    if (wordsTmp == NULL) {
      printf("Не удалось выделить память для сохранения строки");
      return 0;
    }
    *words = wordsTmp;
    (*size) *= 2;
  }

  if (!checkChain(*words, *num, newWord)) {
    (*words)[*num] = newWord;
    ++(*num);
  }
  else
    free(newWord.word);

  return 1;
}

void printResult(elem_t const* words, int num) {
  if (words == NULL)
    return;

  printf("Результат:\n");
  for (int i = 0; i < num; ++i) {
    printf("%s\n", words[i].word);
  }
  printf("\n");
}

elem_t createNewWord(elem_t word1, elem_t word2, int delta) {
  char* newWord = NULL;
  size_t i = 0, lengthNewWord = word1.len + word2.len - delta - 1;

  if (lengthNewWord <= 0)
    return (elem_t) { NULL, 0 };

  newWord = malloc(sizeof(char) * (lengthNewWord));
  if (newWord == NULL) {
    printf("Не удалось выделить память для сохранения строки");
    return (elem_t) { NULL, 0 };
  }

  for (; i < word1.len - 1 && i < lengthNewWord; ++i)
    newWord[i] = word1.word[i];

  for (int j = delta; j < word2.len && i < lengthNewWord; ++j, ++i)
    newWord[i] = word2.word[j];

  return (elem_t) { newWord, lengthNewWord };
}

elem_t* findWords(elem_t* words, int i, int num, int delta, int* numFind) {
  int size = 10;
  elem_t* words2 = malloc(sizeof(elem_t) * size);

  *numFind = 0;
  if (words == NULL) {
    free(words2);
    return NULL;
  }

  if (words[i].len - 1 < delta) {
    free(words2);
    return NULL;
  }

  for (int j = 0; j < num; ++j) {
    if (j == i)
      continue;
    if (words[j].len - 1 < delta)
      continue;

    if (words[i].word[words[i].len - delta - 1] == words[j].word[0]) {
      int len = 1;

      for (int k = 1; k < delta; ++k) {
        if (words[i].word[words[i].len - delta - 1 + k] == words[j].word[k]) {
          ++len;
        }
      }
      if (len == delta) {
        if (!addNew(&words2, &size, numFind, words[j])) {
          free(words2);
          return NULL;
        }
      }
    }
  }

  return words2;
}

elem_t* getWords(char const* str, int* num) {
  int size = 10;
  elem_t* words = calloc(size, sizeof(elem_t));

  *num = 0;

  if (words == NULL) {
    printf("Не удалось выделить память для сохранения строки");
    return NULL;
  }

  for (size_t i = 0; str[i] != '\0'; ++i) {
    size_t len = 0;
    elem_t newWord;

    while (str[i] != '\0' && !isAlpha(str[i]))
      ++i;

    while (isAlpha(str[i])) {
      ++len;
      ++i;
    }

    newWord.word = malloc(sizeof(char) * (len + 1));
    if (newWord.word == NULL) {
      freeWords(words, *num);
      printf("Не удалось выделить память для сохранения строки");
      return NULL;
    }
    for (size_t j = i - len, k = 0; j < i; ++j, ++k)
      newWord.word[k] = str[j];
    newWord.word[len] = '\0';
    newWord.len = len + 1;
    
    if (!addNew(&words, &size, num, newWord)) {
      freeWords(words, *num);
      return NULL;
    }
    --i;
  }

  return words;
}

elem_t* findChains(char const* str1, int* numChains) {
  int size = 10, num = 0, numWords2 = 0;
  elem_t* words = getWords(str1, &num), * chains = malloc(sizeof(elem_t) * size);
  elem_t* words2 = NULL, newWord;

  *numChains = 0;

  if (words == NULL || chains == NULL) {
    freeWords(words, num);
    printf("Не удалось выделить память для сохранения строки");
    return NULL;
  }

  for (int i = 0; i < num; i++) {
    words2 = findWords(words, i, num, NUMLET, &numWords2);

    if (words2 != NULL) {
      for (int j = 0; j < numWords2; ++j) {
        newWord = createNewWord(words[i], words2[j], NUMLET);

        if (newWord.word == NULL)
          return NULL;

        if (!addNew(&chains, &size, numChains, newWord)) {
          freeWords(chains, *numChains);
          return NULL;
        }
      }
      free(words2);
    }
  }

  freeWords(words, num);
  return chains;
}

int main(void) {
  int num = 0;
  char* str1;
  elem_t* chains = NULL;

  SetConsoleCP(1251);
  SetConsoleOutputCP(1251);
  printf("DST-8: Поиск цепочек\nСигнал к завершению - пустая сторка.\n");

  while (1) {
    printf("Введите строку: ");
    str1 = readLine();
    printf("\n");

    if (str1 == NULL || str1[0] == '\0') {
      free(str1);
      break;
    }

    chains = findChains(str1, &num);
    if (chains != NULL)
      printResult(chains, num);
    free(str1);
    freeWords(chains, num);
  }
  return 0;
}