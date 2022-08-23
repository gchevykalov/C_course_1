#include "labengine.h"
#include <math.h>

#define N 100
#define PI 3.14159265

// �����
typedef struct {
  double x;
  double y;
} point_t;

// �������������
typedef struct {
  point_t upperLeftCorner;
  point_t bottomRightCorner;
} rect_t;

// ��������� ����� �� ����� ��������� � ������
point_t Transform(point_t p, rect_t const* from, rect_t const* to) {
  point_t point;
  
  point.x = (p.x - from->upperLeftCorner.x)/(from->bottomRightCorner.x - from->upperLeftCorner.x)
      *(to->bottomRightCorner.x - to->upperLeftCorner.x) + to->upperLeftCorner.x;
  point.y = (p.y - from->upperLeftCorner.y) / (from->bottomRightCorner.y - from->upperLeftCorner.y)
      * (to->bottomRightCorner.y - to->upperLeftCorner.y) + to->upperLeftCorner.y;
  return point;
}

// ���������� ���
void DrawAxes(rect_t const* math, rect_t const* screen) {
  point_t center = {0, 0};
  
  // ��������� ����� � �������� ����������
  center = Transform(center, math, screen);
  // ���������� ���
  LabSetColor(LABCOLOR_BLUE);
  LabDrawLine((int)screen->upperLeftCorner.x, (int)center.y, (int)screen->bottomRightCorner.x, (int)center.y);
  LabDrawLine((int)center.x, (int)screen->bottomRightCorner.y, (int)center.x, (int)screen->upperLeftCorner.y);
}

// ��������� ����� �� ����� ������ ����������
labbool_t IsInsideDisk(point_t p) {
  labbool_t isInside = LAB_FALSE;
  double R = 1;
  
  if (sqrt(p.x*p.x+p.y*p.y) < R) {
    isInside = LAB_TRUE;
  }
  return isInside;
}

// ���������� ���� ������� 1
void DrawDisk(rect_t const* math, rect_t const* screen) {
  point_t z, point;
  int xcounter, ycounter;
  
  LabSetColor(LABCOLOR_RED);
  // ��������� ��� �������� ����� � �������� �������
  for (ycounter = (int)screen->upperLeftCorner.y; ycounter <= screen->bottomRightCorner.y; ycounter++) {
    for (xcounter = (int)screen->upperLeftCorner.x; xcounter <= screen->bottomRightCorner.x; xcounter++) {
      z.x = xcounter;
      z.y = ycounter;
      // ��������� ����� � �������������� ����������
      point = Transform(z, screen, math);
      // ��������� �������������� ��������� � ����������
      if (IsInsideDisk(point)) {
        LabDrawPoint((int)z.x, (int)z.y);
      }
    }
  }
}

// ��������� �������������� ����� ��������� �����
labbool_t IsInsideJulia(point_t p, point_t c) {
  labbool_t isInside = LAB_TRUE;
  int counter = 0;
  point_t z, z0;
  double R = 2;
  
  z0 = p;
  // �������� N ��������
  while (counter <= N) {
    z.x = z0.x * z0.x - z0.y * z0.y + c.x;
    z.y = 2 * z0.x * z0.y + c.y;
    // ��������� �����������
    if (sqrt(z.x * z.x + z.y * z.y) > R) {
      isInside = LAB_FALSE;
      break;
    }
    else {
      z0 = z;
      counter++;
    }
  }
  return isInside;
}

// ���������� ��������� �����
void DrawJulia(rect_t const* math, rect_t const* screen, point_t c) {
  point_t z, point;
  int xcounter, ycounter;
  
  LabSetColor(LABCOLOR_DARK_GREEN);
  // ��������� ��� �������� ����� � �������� �������
  for (ycounter = (int)screen->upperLeftCorner.y; ycounter <= screen->bottomRightCorner.y; ycounter++) {
    for (xcounter = (int)screen->upperLeftCorner.x; xcounter <= screen->bottomRightCorner.x; xcounter++) {
      z.x = xcounter;
      z.y = ycounter;
      // ��������� ����� � �������������� ����������
      point = Transform(z, screen, math);
      // ��������� �������������� ��������� � ����������
      if (IsInsideJulia(point, c)) {
        LabDrawPoint((int)z.x, (int)z.y);
      }
    }
  }
}

int main(void) {
  // ���������������� ����������
  if (LabInit()) {
    point_t c;
  double r = 0.32, alpha = 0;
    // ������ ������� ����
    int width = LabGetWidth();
    int height = LabGetHeight();
    // ������ ������������� �������
    rect_t math = {{-2, 3}, {2, -3}};
    rect_t leftHalfOfTheScreen;
    rect_t rightHalfOfTheScreen;
    
    leftHalfOfTheScreen.upperLeftCorner.x = 0;
    leftHalfOfTheScreen.upperLeftCorner.y = 0;
    leftHalfOfTheScreen.bottomRightCorner.x = width / 2;
    leftHalfOfTheScreen.bottomRightCorner.y = height;
    rightHalfOfTheScreen.upperLeftCorner.x = width / 2;
    rightHalfOfTheScreen.upperLeftCorner.y = 0;
    rightHalfOfTheScreen.bottomRightCorner.x = width;
    rightHalfOfTheScreen.bottomRightCorner.y = height;

    while (!LabInputKeyReady()) {
      LabClear();
      // ��������� ��������� ����� - ���������� ����� ����� ������� 1
      DrawAxes(&math, &leftHalfOfTheScreen);
      DrawDisk(&math, &leftHalfOfTheScreen);
      
      c.x = r * sin(alpha);
      c.y = r - r * cos(alpha);
      // ��������� ��������� �����
      DrawAxes(&math, &rightHalfOfTheScreen);
      DrawJulia(&math, &rightHalfOfTheScreen, c);
      
      // ���������� �������� � ����
      LabDrawFlush();
      alpha = fmod(alpha+0.1, 2*PI);
      LabDelay(5);
    }
    // ��������� ������� ������� � ��������� ������
    LabTerm();
  }
  return 0;
}
