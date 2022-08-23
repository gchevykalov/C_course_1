#include <math.h>
#include "labengine.h"

#define MAX_DISTANCE    2.0
#define MAX_ITERATIONS  1023

typedef struct {
  double x, y;
} point_t;

typedef struct {
  point_t a, b;
} rect_t;

typedef struct {
  rect_t screen;
  rect_t math;
} viewport_t;

// a color set to apply in the new MAN task

typedef struct {
  unsigned char r, g, b;
} color_t;

static color_t s_palette[] = {
  {0x00, 0x00, 0xFF},
  {0x00, 0xFF, 0xFF},
  {0xFF, 0xFF, 0x00},
  {0xFF, 0x00, 0x00},
  {0xFF, 0xFF, 0x00},
  {0x00, 0xFF, 0xFF},
  {0x00, 0x00, 0xFF},
};

const int MAX_COLORS = sizeof(s_palette) / sizeof(s_palette[0]);

point_t Transform(point_t p, rect_t const* from, rect_t const* to) {
  point_t res = {
    to->a.x + (p.x - from->a.x) * (to->b.x - to->a.x) / (from->b.x - from->a.x),
    to->a.y + (p.y - from->a.y) * (to->b.y - to->a.y) / (from->b.y - from->a.y)
  };
  return res;
}

void DrawAxes(viewport_t const* view) {
  point_t a = view->screen.a;
  point_t b = view->screen.b;
  point_t zero = { 0.0, 0.0 };
  zero = Transform(zero, &view->math, &view->screen);
  LabDrawRectangle((int)a.x, (int)a.y, (int)b.x + 1, (int)b.y + 1);
  if (zero.x >= a.x && zero.x <= b.x)
    LabDrawLine((int)zero.x, (int)a.y, (int)zero.x, (int)b.y + 1);
  if (zero.y >= a.y && zero.y <= b.y)
    LabDrawLine((int)a.x, (int)zero.y, (int)b.x + 1, (int)zero.y);
}

double IsOutsideJulia(point_t p, point_t arg) {
  int i;
  point_t c = arg;
  point_t z = p;
  double t;

  for (i = 0; i < MAX_ITERATIONS; i++) {
    if (z.x * z.x + z.y * z.y > MAX_DISTANCE * MAX_DISTANCE)
      return ((double)(MAX_ITERATIONS - i)) / (MAX_ITERATIONS + 1);

    t = 2 * z.x * z.y + c.y;
    z.x = z.x * z.x - z.y * z.y + c.x;
    z.y = t;
  }

  return 0;
}

double IsOutsideMandelbrot(point_t p, point_t arg) {
  int i;
  point_t c = p;
  point_t z = arg;
  double t;

  for (i = 0; i < MAX_ITERATIONS; i++) {
    if (z.x * z.x + z.y * z.y > MAX_DISTANCE * MAX_DISTANCE)
      return ((double)(MAX_ITERATIONS - i)) / (MAX_ITERATIONS + 1);

    t = 2 * z.x * z.y + c.y;
    z.x = z.x * z.x - z.y * z.y + c.x;
    z.y = t;
  }

  return 0;
}

void DrawSet(viewport_t const* view, double(*isInside)(point_t, point_t), point_t arg) {
  double x, y;
  point_t p;
  double is = 0, alpha = 0;
  int k = 0;
  color_t color;

  for (x = view->screen.a.x; x <= view->screen.b.x; x++) {
    for (y = view->screen.a.y; y <= view->screen.b.y; y++) {
      p.x = x;
      p.y = y;
      p = Transform(p, &view->screen, &view->math);
      is = isInside(p, arg);
      is = pow(is, 3);
      k = (int)(is * (MAX_COLORS - 1));
      alpha = (is * (MAX_COLORS - 1)) - k;
      color.r = (1 - alpha) * s_palette[k].r + alpha * s_palette[k + 1].r;
      color.g = (1 - alpha) * s_palette[k].g + alpha * s_palette[k + 1].g;
      color.b = (1 - alpha) * s_palette[k].b + alpha * s_palette[k + 1].b;
      if (is != 0) {
        LabSetColorRGB(color.r, color.g, color.b);
        LabDrawPoint((int)x, (int)y);
      }
    }
  }
}

void Run(void) {
  int width = LabGetWidth();
  int height = LabGetHeight();
  viewport_t view[2] = { {
    {0.0, 0.0, (double)(width / 2 - 1), (double)(height - 1)},
    {-2.0, 3.0, 2.0, -3.0},
  }, {
    {(double)(width / 2), 0.0, (double)(width - 1), (double)(height - 1)},
    { -0.7454356, 0.113019, -0.7454215, 0.1129986},
  } };
  point_t c = { -0.835, 0.2321 };
  point_t z0 = { 0.0, 0.0 };
  double alpha = 0.0, r = 0.32;


  DrawAxes(&view[0]);
  DrawSet(&view[0], IsOutsideJulia, c);

  DrawAxes(&view[1]);
  DrawSet(&view[1], IsOutsideMandelbrot, z0);

  LabDrawFlush();

  LabInputKey();
}

int main(void) {
  if (LabInit()) {
    Run();
    LabTerm();
  }
  return 0;
}