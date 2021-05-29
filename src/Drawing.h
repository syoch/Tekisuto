#pragma once

namespace Drawing {
  void SetTarget(HDC hdc);
  
  void DrawPixel(int x, int y, COLORREF color);
  COLORREF GetPixel(int x, int y);

  void DrawRect(int x, int y, int width, int height, COLORREF color);
  void DrawLine(int x1, int y1, int x2, int y2, COLORREF color);
  void DrawString(std::wstring const& str, int x, int y, COLORREF forecolor, COLORREF backcolor = RGB(0, 0, 0), bool transparent = false);
  void DrawChar(wchar_t ch, int x, int y, COLORREF color);
}