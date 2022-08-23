#include <math.h>
#include <windows.h>
#include "labengine.h"

typedef struct {
  double x;
  double y;
} point_t;

typedef struct {
  point_t a;
  point_t b;
} rect_t;

typedef struct {
  rect_t screen;
  rect_t math;
} viewport_t;

typedef struct {
  point_t endOfVector;
} vec_t;

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
  
  LabSetColor(LABCOLOR_RED);
  zero = Transform(zero, &view->math, &view->screen);

  if (zero.x >= a.x && zero.x <= b.x)
    LabDrawLine((int)zero.x, (int)a.y, (int)zero.x, (int)b.y + 1);
  if (zero.y >= a.y && zero.y <= b.y)
    LabDrawLine((int)a.x, (int)zero.y, (int)b.x + 1, (int)zero.y);
}

void DrawAnaliticPath(rect_t const* math, rect_t const* screen, vec_t r0, vec_t v0, vec_t a0, double dt) {
  vec_t r = { 0 }, rLast = r0;
  point_t vecBeg, vecEnd;
  double tMax = fabs(2 * v0.endOfVector.y / a0.endOfVector.y), t;
  
  for (t = 0; t < tMax; t += dt) {
    r.endOfVector.x = r0.endOfVector.x + v0.endOfVector.x * t + 1.0 / 2.0 * a0.endOfVector.x * t * t;
    r.endOfVector.y = r0.endOfVector.x + v0.endOfVector.y * t + 1.0 / 2.0 * a0.endOfVector.y * t * t;
    
    vecBeg = Transform(rLast.endOfVector, math, screen);
    vecEnd = Transform(r.endOfVector, math, screen);
    
    LabDrawLine((int)vecBeg.x, (int)vecBeg.y, (int)vecEnd.x, (int)vecEnd.y);
    rLast = r;
  }
}

void DrawEilerPath(rect_t const* math, rect_t const* screen, vec_t r0, vec_t v0, vec_t a0, double dt) {
  vec_t r = r0, rLast = r0, deltaV, deltaR;
  point_t vec, vecEnd;
  
  while (rLast.endOfVector.y >= 0) {
    deltaV.endOfVector.x = a0.endOfVector.x * dt;
    deltaV.endOfVector.y = a0.endOfVector.y * dt;
    
    deltaR.endOfVector.x = v0.endOfVector.x * dt;
    deltaR.endOfVector.y = v0.endOfVector.y * dt;
    
    vec = Transform(rLast.endOfVector, math, screen);
    vecEnd = Transform(r.endOfVector, math, screen);
    
    LabDrawLine((int)vec.x, (int)vec.y, (int)vecEnd.x, (int)vecEnd.y);
    rLast = r;
    
    r.endOfVector.x += deltaR.endOfVector.x;
    r.endOfVector.y += deltaR.endOfVector.y;
    
    v0.endOfVector.x += deltaV.endOfVector.x;
    v0.endOfVector.y += deltaV.endOfVector.y;
  }
}

void SimulateEulerPath(rect_t const* math, rect_t const* screen, vec_t r0, vec_t v0, vec_t a0) {
  LARGE_INTEGER freq, count1, count2;
  double dt;
  vec_t r = r0, rLast = r0, deltaV, deltaR;
  point_t vec, vecEnd;

  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&count1);
  do {
    QueryPerformanceCounter(&count2);
    dt = (count2.QuadPart - count1.QuadPart) / (double)freq.QuadPart;

    deltaV.endOfVector.x = a0.endOfVector.x * dt;
    deltaV.endOfVector.y = a0.endOfVector.y * dt;
  
    deltaR.endOfVector.x = v0.endOfVector.x * dt;
    deltaR.endOfVector.y = v0.endOfVector.y * dt;
  
    vec = Transform(rLast.endOfVector, math, screen);
    vecEnd = Transform(r.endOfVector, math, screen);
  
    LabDrawLine((int)vec.x, (int)vec.y, (int)vecEnd.x, (int)vecEnd.y);
    rLast = r;
    LabDrawFlush();

    r.endOfVector.x += deltaR.endOfVector.x;
    r.endOfVector.y += deltaR.endOfVector.y;
  
    v0.endOfVector.x += deltaV.endOfVector.x;
    v0.endOfVector.y += deltaV.endOfVector.y;

    count1 = count2;
  } while (rLast.endOfVector.y >= 0);
}

void Run(void) {
  int width = LabGetWidth();
  int height = LabGetHeight();
  
  rect_t screen = { 0.0, 0.0, width - 1.0, height - 1.0 };
  rect_t math = { -3.0, 28.0, 41.0, -5.0 };

  viewport_t view = {
  {0.0, 0.0, width - 1.0, height - 1.0},
  {-3.0, 28.0, 41.0, -5.0},
  };
  
  vec_t r0 = { 0 }, v0 = { {10, 10 * sqrt(3)} }, g = { 0, -9.81 };
  
  while (!LabInputKeyReady()) {
    LabClear();
    
    LabSetColor(LABCOLOR_DARK_RED);
    DrawAxes(&view);
    
    LabSetColor(LABCOLOR_DARK_GREEN);
    DrawAnaliticPath(&math, &screen, r0, v0, g, 0.5);
    DrawAnaliticPath(&math, &screen, r0, v0, g, 1);
    
    LabSetColor(LABCOLOR_RED);
    DrawEilerPath(&math, &screen, r0, v0, g, 0.5);
    DrawEilerPath(&math, &screen, r0, v0, g, 1);
    LabDrawFlush();

    LabSetColor(LABCOLOR_YELLOW);
    SimulateEulerPath(&math, &screen, r0, v0, g);
  }
  
  LabInputKey();
}

int main(void) {
  if (LabInit()) {
    Run();
    LabTerm();
  }
  return 0;
}
