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
  
  // 背景
  Drawing::DrawRect(0, 0, WindowSize.Width, WindowSize.Height, RGB(30, 30, 30));
  
  
  
  
  
  auto hdc = BeginPaint(hwnd, &ps);
  BitBlt(hdc, 0, 0, WindowSize.Width, WindowSize.Height, hBuffer, 0, 0, SRCCOPY);
  EndPaint(hwnd, &ps);
}

LRESULT CALLBACK Application::WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
  HDC hdc;
  
  auto& ctx = GetCurrentContext();
  
  switch( msg ) {
    // ウィンドウ作成
    case WM_CREATE: {
      hdc = GetDC(hwnd);
      
      hBitmap = CreateCompatibleBitmap(hdc, WindowSize.Width, WindowSize.Height);
      hBuffer = CreateCompatibleDC(hdc);
      
      SelectObject(hBuffer, hBitmap);
      SelectObject(hBuffer, GetStockObject(NULL_PEN));
      Drawing::SetTarget(hBuffer);
      
      PatBlt(hBuffer, 0, 0, WindowSize.Width, WindowSize.Height, WHITENESS);
      
      ReleaseDC(hwnd, hdc);
      break;
    }
    
    // ウィンドウ破棄
    case WM_DESTROY: {
      DeleteDC(hBuffer);
      DeleteObject(hBitmap);
      
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
