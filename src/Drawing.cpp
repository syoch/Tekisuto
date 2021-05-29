#include "main.h"

namespace {
  HDC target;
  
}

namespace Drawing {
  void SetTarget(HDC hdc) {
    target = hdc;
  }

  void DrawPixel(int x, int y, COLORREF color) {
    SetPixel(target, x, y, color);
  }

  COLORREF GetPixel(int x, int y) {
    return GetPixel(target, x, y);
  }

  void DrawRect(int x, int y, int width, int height, COLORREF color) {
    DeleteObject(SelectObject(target, CreateSolidBrush(color)));
    PatBlt(target, x, y, width, height, PATCOPY);
  }

  void DrawLine(int x1, int y1, int x2, int y2, COLORREF color) {
    DeleteObject(SelectObject(target, CreatePen(PS_SOLID, 1, color)));
    MoveToEx(target, x1, y1, NULL);
    LineTo(target, x2, y2);
  }

  void DrawString(std::wstring const& str, int x, int y, COLORREF forecolor, COLORREF backcolor, bool transparent) {
    SetTextColor(target, forecolor);
    SetBkColor(target, backcolor);

    if( transparent )
      SetBkMode(target, TRANSPARENT);
    else
      SetBkMode(target, OPAQUE);

    TextOut(target, x, y, str.c_str(), str.length());
  }

  void DrawChar(wchar_t ch, int x, int y, COLORREF color) {
    DrawString(std::wstring(1, ch), x, y, color);
  }
}