#include <math.h>
#include <stdio.h>
#include "labengine.h"

typedef struct {
  double x;
  double y;
} point_t;

void curve(point_t a, point_t b, int n) {
  if (n == 0) {
    LabSetColor(LABCOLOR_RED);
    LabDrawLine((int)a.x, (int)a.y, (int)b.x, (int)b.y);
  }
  else {
    point_t a1;
    point_t b1;
    point_t c;

    a1.x = a.x + (b.x - a.x) / 3.;
    a1.y = a.y - (a.y - b.y) / 3.;
    b1.x = a.x + 2 * (b.x - a.x) / 3.;
    b1.y = a.y - 2 * (a.y - b.y) / 3.;
    c.x = a1.x + ((b1.x - a1.x) / 2 + (b1.y - a1.y) * sqrt(3) / 2);
    c.y = a1.y + (-(b1.x - a1.x) * sqrt(3) / 2 + (b1.y - a1.y) / 2);

    curve(a, a1, n - 1);
    curve(a1, c, n - 1);
    curve(c, b1, n - 1);
    curve(b1, b, n - 1);
  }
}

int main(void) {
  point_t A, B;
  int n;
  unsigned int character;

  printf("Enter n: ");
  scanf_s("%i", &n);
  printf("Enter coordinates A.x and A.y: ");
  scanf_s("%lf%lf", &A.x, &A.y);
  printf("Enter coordinate B.x and B.y: ");
  scanf_s("%lf%lf", &B.x, &B.y);

  if (LabInit()) {

    curve(A, B, n);
    LabDrawFlush();
    
    while(!LabInputKeyReady()){
      character = LabInputKey();
      if (character == LABKEY_ESC)
        break;

      switch (character) {
        case '-':
          LabClear();
          if (n > 0)
            --n;
          curve(A, B, n);
          LabDrawFlush();
          break;
        case '+':
          LabClear();
          ++n;
          curve(A, B, n);
          LabDrawFlush();
        default:
          break;
      }
    }
    LabTerm();
  }
  return 0;
}