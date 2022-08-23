#include "labengine.h"
#include <math.h>

#define N 100
#define PI 3.14159265

// Точка
typedef struct {
  double x;
  double y;
} point_t;

// Прямоугольник
typedef struct {
  point_t upperLeftCorner;
  point_t bottomRightCorner;
} rect_t;

// Перевести точку из одних координат в другие
point_t Transform(point_t p, rect_t const* from, rect_t const* to) {
  point_t point;
  
  point.x = (p.x - from->upperLeftCorner.x)/(from->bottomRightCorner.x - from->upperLeftCorner.x)
      *(to->bottomRightCorner.x - to->upperLeftCorner.x) + to->upperLeftCorner.x;
  point.y = (p.y - from->upperLeftCorner.y) / (from->bottomRightCorner.y - from->upperLeftCorner.y)
      * (to->bottomRightCorner.y - to->upperLeftCorner.y) + to->upperLeftCorner.y;
  return point;
}

// Изобразить оси
void DrawAxes(rect_t const* math, rect_t const* screen) {
  point_t center = {0, 0};
  
  // Перевести центр в экранные координаты
  center = Transform(center, math, screen);
  // Изобразить оси
  LabSetColor(LABCOLOR_BLUE);
  LabDrawLine((int)screen->upperLeftCorner.x, (int)center.y, (int)screen->bottomRightCorner.x, (int)center.y);
  LabDrawLine((int)center.x, (int)screen->bottomRightCorner.y, (int)center.x, (int)screen->upperLeftCorner.y);
}

// Проверить лежит ли точка внутри окружности
labbool_t IsInsideDisk(point_t p) {
  labbool_t isInside = LAB_FALSE;
  double R = 1;
  
  if (sqrt(p.x*p.x+p.y*p.y) < R) {
    isInside = LAB_TRUE;
  }
  return isInside;
}

// Изобразить диск радиуса 1
void DrawDisk(rect_t const* math, rect_t const* screen) {
  point_t z, point;
  int xcounter, ycounter;
  
  LabSetColor(LABCOLOR_RED);
  // Пробежать все экранные точки в заданной области
  for (ycounter = (int)screen->upperLeftCorner.y; ycounter <= screen->bottomRightCorner.y; ycounter++) {
    for (xcounter = (int)screen->upperLeftCorner.x; xcounter <= screen->bottomRightCorner.x; xcounter++) {
      z.x = xcounter;
      z.y = ycounter;
      // Перевести точки в математические координаты
      point = Transform(z, screen, math);
      // Проверить принадлежность множеству и изобразить
      if (IsInsideDisk(point)) {
        LabDrawPoint((int)z.x, (int)z.y);
      }
    }
  }
}

// Проверить принадлежность точки множеству Жюлиа
labbool_t IsInsideJulia(point_t p, point_t c) {
  labbool_t isInside = LAB_TRUE;
  int counter = 0;
  point_t z, z0;
  double R = 2;
  
  z0 = p;
  // Провести N итераций
  while (counter <= N) {
    z.x = z0.x * z0.x - z0.y * z0.y + c.x;
    z.y = 2 * z0.x * z0.y + c.y;
    // Проверить расхождение
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

// Изобразить множество Жюлиа
void DrawJulia(rect_t const* math, rect_t const* screen, point_t c) {
  point_t z, point;
  int xcounter, ycounter;
  
  LabSetColor(LABCOLOR_DARK_GREEN);
  // Пробежать все экранные точки в заданной области
  for (ycounter = (int)screen->upperLeftCorner.y; ycounter <= screen->bottomRightCorner.y; ycounter++) {
    for (xcounter = (int)screen->upperLeftCorner.x; xcounter <= screen->bottomRightCorner.x; xcounter++) {
      z.x = xcounter;
      z.y = ycounter;
      // Перевести точки в математические координаты
      point = Transform(z, screen, math);
      // Проверить принадлежность множеству и изобразить
      if (IsInsideJulia(point, c)) {
        LabDrawPoint((int)z.x, (int)z.y);
      }
    }
  }
}

int main(void) {
  // Инициализировать библиотеку
  if (LabInit()) {
    point_t c;
  double r = 0.32, alpha = 0;
    // Узнать размеры окна
    int width = LabGetWidth();
    int height = LabGetHeight();
    // Задать прямоугольные области
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
      // Построить множество точек - внутренние точки круга радиуса 1
      DrawAxes(&math, &leftHalfOfTheScreen);
      DrawDisk(&math, &leftHalfOfTheScreen);
      
      c.x = r * sin(alpha);
      c.y = r - r * cos(alpha);
      // Построить множество Жюлиа
      DrawAxes(&math, &rightHalfOfTheScreen);
      DrawJulia(&math, &rightHalfOfTheScreen, c);
      
      // Отобразить картинку в окне
      LabDrawFlush();
      alpha = fmod(alpha+0.1, 2*PI);
      LabDelay(5);
    }
    // Подождать нажатия клавиши и закончить работу
    LabTerm();
  }
  return 0;
}
