#include "main.h"

LRESULT CALLBACK WndProc_Wrap(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
  return Application::GetInstance()->WndProc(hwnd, msg, wp, lp);
}

//
// ウィンドウ情報を更新する
void Application::UpdateWindowInfo() {
  // ウィンドウ情報取得
  GetWindowInfo(hwnd, &WindowInfo);
    
  WindowSize = {  // ウィンドウサイズ 更新
    WindowInfo.rcWindow.right - WindowInfo.rcWindow.left,
    WindowInfo.rcWindow.bottom - WindowInfo.rcWindow.top
  };
  
  ClientSize = {  // クライアントサイズ 更新
    WindowInfo.rcClient.right - WindowInfo.rcClient.left,
    WindowInfo.rcClient.bottom - WindowInfo.rcClient.top
  };
}

// ------------------------------------------------------- //
//  ウィンドウプロシージャ
// ------------------------------------------------------- //
LRESULT CALLBACK Application::WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
  HDC hdc;
  HFONT hFont;  // 文字描画用フォント
  
  // 現在のコンテキスト
  auto& ctx = GetCurrentContext();
  
  switch( msg ) {
    // ウィンドウ 作成
    case WM_CREATE: {
      hdc = GetDC(hwnd);
      
      hBitmap = CreateCompatibleBitmap(hdc, WINDOW_WIDTH_MAX, WINDOW_HEIGHT_MAX);
      hBuffer = CreateCompatibleDC(hdc);
      
      hFont = CreateFont( // フォント作成
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
      
      ReleaseDC(hwnd, hdc);
      break;
    }
    
    // ウィンドウ 表示 / 非表示
    case WM_SHOWWINDOW: {
      UpdateWindowInfo();
      break;
    }
    
    // ウィンドウ 破棄
    case WM_DESTROY: {
      DeleteDC(hBuffer);
      DeleteObject(hBitmap);
      DeleteObject(hFont);
      
      PostQuitMessage(0);
      break;
    }
    
    // ウィンドウ 描画
    case WM_PAINT: {
      DrawEditor();
      break;
    }
    
    // ウィンドウ サイズ変更
    case WM_SIZE: {
      UpdateWindowInfo();
      
      SetWindowPos( // ウィンドウサイズ制限
        hwnd,
        NULL,
        WindowInfo.rcWindow.left,  //
        WindowInfo.rcWindow.top,   // 座標は変える必要なし
        std::min(WindowSize.Width, WINDOW_WIDTH_MAX),   //
        std::min(WindowSize.Height, WINDOW_HEIGHT_MAX), // ウィンドウサイズが最大を超えないようにする
        SWP_SHOWWINDOW
      );
      
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
