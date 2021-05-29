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

// マウス情報を更新
Point Application::UpdateMouseInfo() {
  auto& ctx = GetCurrentContext();
  
  // マウス座標取得 (モニター)
  POINT mpos;
  GetCursorPos(&mpos);
  
  // マウス座標更新 (ウィンドウ上)
  Point mouse_pos = {
    mpos.x - WindowInfo.rcClient.left,
    mpos.y - WindowInfo.rcClient.top
  };
  
  // カーソル位置を更新する
  ctx.CursorPos = {
    mouse_pos.X / CHAR_WIDTH,
    mouse_pos.Y / CHAR_HEIGHT
  };
  
  if( ctx.CursorPos.Y < 0 ) ctx.CursorPos.Y = 0;
  if( ctx.CursorPos.Y >= ctx.Source.size() ) ctx.CursorPos.Y = ctx.Source.size() - 1;
  
  if( ctx.CursorPos.X < 0 ) ctx.CursorPos.X = 0;
  if( ctx.CursorPos.X > ctx.Source[ctx.CursorPos.Y].length() )
    ctx.CursorPos.X = ctx.Source[ctx.CursorPos.Y].length();
  
  
  return { mpos.x, mpos.y };
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
      auto mouse_pos = UpdateMouseInfo();
      
      if(
        mouse_pos.X >= WindowInfo.rcClient.left &&
        mouse_pos.Y >= WindowInfo.rcClient.top &&
        mouse_pos.X < WindowInfo.rcClient.right &&
        mouse_pos.Y < WindowInfo.rcClient.bottom
      ) {
        ctx.IsMouseDown = true;
      }
      
      SetCapture(hwnd);
      
      DrawEditor();
      ForceRedraw();
      break;
    }
    
    // マウス 移動
    case WM_MOUSEMOVE: {
      if( !ctx.IsMouseDown )
        break;
      
      UpdateMouseInfo();
      
      
      
      DrawEditor();
      ForceRedraw();
      break;
    }
    
    // マウス 離す
    case WM_LBUTTONUP: {
      ctx.IsMouseDown = false;
      
      ReleaseCapture();
      
      break;
    }
    
    // 文字挿入
    case WM_CHAR: {
      static const std::wstring check_str = L" `~!@#$%^&*()_-+={[}]\\|'\";:,<.>/?";
      wchar_t keycode = wp & 0xFFFF;
      
      if( !isalnum(keycode) ) {
        if( check_str.find(keycode) == std::wstring::npos )
          break;
      }
      
      auto& line = ctx.Source[ctx.CursorPos.Y];
      line.insert(line.begin() + ctx.CursorPos.X, keycode);
      ctx.CursorPos.X++;
      
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
