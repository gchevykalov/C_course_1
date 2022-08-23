#define _CRTDBG_MAP_ALLOC

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <crtdbg.h>

#include "labengine.h"

typedef struct {
  double x;
  double y;
} point_t;

typedef struct node_t {
  point_t point;
  struct node_t* next;
} node_t;

typedef struct {
  int size;
  node_t* head;
} curve_t;

typedef struct {
  unsigned char r, g, b;
} color_t;

color_t colors[2] = { { 0, 255, 255 }, { 255, 0, 255 } };

curve_t* CurveCreate(double ax, double ay, double bx, double by) {
  curve_t* curve = malloc(sizeof(curve_t));
  node_t* point1 = malloc(sizeof(node_t));
  node_t* point2 = malloc(sizeof(node_t));

  if (point1 && point2 && curve) {
    point1->point.x = ax;
    point1->point.y = ay;
    point2->point.x = bx;
    point2->point.y = by;

    curve->size = 2;

    point1->next = point2;
    point2->next = NULL;

    curve->head = point1;
    return curve;
  }

  free(curve);
  free(point1);
  free(point2);

  return NULL;
}

void CurveDestroy(curve_t* curve) {
  node_t* node;

  assert(curve);

  while (curve->head != NULL) {
    node = curve->head;
    curve->head = node->next;

    free(node);
  }

  free(curve);
}

void CurveDraw(curve_t* curve) {
  node_t* cur;
  node_t* next;
  node_t* end;

  color_t currentColor;
  double t = 0.0;

  assert(curve);
  assert(curve->head);

  cur = curve->head;
  next = curve->head->next;
  end = curve->head;
  do {
    currentColor.r = (unsigned char)(colors[0].r * (1 - t / curve->size) + t / curve->size * colors[1].r);
    currentColor.g = (unsigned char)(colors[0].g * (1 - t / curve->size) + t / curve->size * colors[1].g);
    currentColor.b = (unsigned char)(colors[0].b * (1 - t / curve->size) + t / curve->size * colors[1].b);

    LabSetColorRGB(currentColor.r, currentColor.g, currentColor.b);
    LabDrawLine((int)(cur->point.x), (int)(cur->point.y), (int)(next->point.x), (int)(next->point.y));
    cur = next;
    next = next->next;
    t++;
  } while (next);
}

void CurveNext(curve_t* curve) {
  int direction = 1;
  node_t* point;
  node_t* next;
  node_t* end;
  int size;

  assert(curve);
  assert(curve->head);

  size = curve->size * 2 - 1;

  point = curve->head;
  next = point->next;
  end = curve->head;

  do {
    node_t* spike = malloc(sizeof(node_t));
    double dx, dy, cx, cy;

    dx = next->point.x - point->point.x;
    dy = next->point.y - point->point.y;
    cx = 0.5 * (point->point.x + next->point.x + dy * direction);
    cy = 0.5 * (point->point.y + next->point.y - dx * direction);

    spike->point.x = cx;
    spike->point.y = cy;
    spike->next = next;
    point->next = spike;

    point = next;
    next = next->next;
    direction = -direction;
  } while (next);

  curve->size = size;
}

void CurvePrev(curve_t* curve) {
  node_t* point, * node;
  int size;

  assert(curve);
  assert(curve->head);

  if (curve->size <= 2)
    return;

  assert(curve->size % 2 == 1);

  size = curve->size / 2 + 1;
  point = curve->head;

  while (point != NULL && point->next != NULL) {
    node = point->next;
    point->next = node->next;
    point = point->next;
    free(node);
  }

  curve->size = size;
}

int main(void) {
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);

  if (LabInit()) {
    curve_t* curve;
    labkey_t key;
    int width = LabGetWidth();
    int height = LabGetHeight();

    curve = CurveCreate(width / 4, height / 2, 3 * width / 4, 3 * height / 4);

    LabClear();
    CurveDraw(curve);
    LabDrawFlush();

    while ((key = LabInputKey()) != LABKEY_ESC) {
      switch (key) {
        case LABKEY_UP:
          CurveNext(curve);
          break;
        case LABKEY_DOWN:
          CurvePrev(curve);
          break;
        default:
          continue;
      }

      LabClear();
      CurveDraw(curve);
      LabDrawFlush();
    }

    CurveDestroy(curve);
  }

  LabTerm();
  return 0;
}