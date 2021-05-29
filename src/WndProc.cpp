#include "main.h"

LRESULT CALLBACK WndProc_Wrap(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
  return Application::GetInstance()->WndProc(hwnd, msg, wp, lp);
}

void Application::ForceRedraw() {
  auto hdc = GetDC(hwnd);
  
  StretchBlt(
    hdc, 0, 0, WindowSize.Width, WindowSize.Height,
    hBuffer, 0, 0, WindowSize.Width, WindowSize.Height,
    SRCCOPY
  );
  
  DeleteDC(hdc);
}

void Application::DrawEditor() {
  PAINTSTRUCT ps;
  
  auto& ctx = GetCurrentContext();
  
  // 背景
  Drawing::DrawRect(0, 0, WindowSize.Width, WindowSize.Height, RGB(30, 30, 30));
  
  // ソースコード 描画
  int begin = 0;
  int end = std::min<int>(ctx.Source.size(), begin + WindowSize.Height / CHAR_HEIGHT);
  int posX = 0, posY = 0;
  for( ; begin < end; begin++, posY += 12 ) {
    auto const& line = ctx.Source[begin];
    
    Drawing::DrawString(line, posX, posY, RGB(255, 255, 255), 0, true);
  }
  
  
  
  auto hdc = BeginPaint(hwnd, &ps);
  BitBlt(hdc, 0, 0, WindowSize.Width, WindowSize.Height, hBuffer, 0, 0, SRCCOPY);
  EndPaint(hwnd, &ps);
}

LRESULT CALLBACK Application::WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
  HDC hdc;
  HFONT hFont;
  
  auto& ctx = GetCurrentContext();
  
  switch( msg ) {
    // ウィンドウ作成
    case WM_CREATE: {
      hdc = GetDC(hwnd);
      
      hBitmap = CreateCompatibleBitmap(hdc, WindowSize.Width, WindowSize.Height);
      hBuffer = CreateCompatibleDC(hdc);
      
      hFont = CreateFont(
        CHAR_HEIGHT, CHAR_WIDTH,
        0, 0, 0,
        0, 0, 0,
        SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        VARIABLE_PITCH | FF_ROMAN, TEXT("MeiryoKe_Console")
      );
      
      SelectObject(hBuffer, hBitmap);
      SelectObject(hBuffer, GetStockObject(NULL_PEN));
      SelectObject(hBuffer, hFont);
      Drawing::SetTarget(hBuffer);
      
      PatBlt(hBuffer, 0, 0, WindowSize.Width, WindowSize.Height, WHITENESS);
      
      ReleaseDC(hwnd, hdc);
      break;
    }
    
    // ウィンドウ破棄
    case WM_DESTROY: {
      DeleteDC(hBuffer);
      DeleteObject(hBitmap);
      DeleteObject(hFont);
      
      PostQuitMessage(0);
      break;
    }
    
    // ウィンドウ描画
    case WM_PAINT: {
      DrawEditor();
      break;
    }
    
    // マウスクリック
    case WM_LBUTTONDOWN: {
      
      
      DrawEditor();
      ForceRedraw();
      break;
    }
    
    
    //
    default:
      return DefWindowProc(hwnd, msg, wp, lp);
  }
  
  return 0;
}
