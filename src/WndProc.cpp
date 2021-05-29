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
  
  // メニューバー "ファイル -> 開く"
  static OPENFILENAME  ofn;           // ダイアログ表示用構造体 ( WM_CREATE で初期化されます )
  static wchar_t OpenFile_buf[1000];  // ファイルのフルパス用バッファ
  
  // 現在のコンテキスト
  auto& ctx = GetCurrentContext();
  
  switch( msg ) {
    // メニューバーなど
    case WM_COMMAND: {
      switch( LOWORD(wp) ) {
        // ファイル -> 開く
        case MENUBAR_FILE_OPEN: {
          memset(OpenFile_buf, 0, sizeof(OpenFile_buf));
          GetOpenFileName(&ofn);
          
          std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
          
          std::ifstream ifs(converter.to_bytes(OpenFile_buf));
          std::string line;
          
          if( ifs.fail() ) {
            
            break;
          }
          
          ctx.Source.clear();
          while( std::getline(ifs, line) ) {
            //auto&& ws = std::move(converter.from_bytes(line));
            auto ws = converter.from_bytes(line);
            //replace_wstr(ws, L"\t", L" ");
            ctx.Source.emplace_back(ws);
          }
          
          ctx.CursorPos = { 0, 0 };
          DrawEditor();
          ForceRedraw();
          break;
        }
      }
      
      break;
    }
    
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
      
      ofn.lStructSize = sizeof(OPENFILENAME);
      ofn.hwndOwner = hwnd;
    //  ofn.lpstrFilter = TEXT("Text files {*.txt}\0*.txt\0");
      ofn.lpstrFilter = TEXT("C++ Files (.cpp)\0*.cpp\0All files\0*.*\0");
      //ofn.lpstrCustomFilter = strCustom;
      ofn.nMaxCustFilter = 256;
      ofn.nFilterIndex = 0;
      ofn.lpstrFile = OpenFile_buf;
      ofn.nMaxFile = MAX_PATH;
      ofn.Flags = OFN_FILEMUSTEXIST;
      
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
    
    // キー入力
    case WM_KEYDOWN: {
      auto& line = ctx.Source[ctx.CursorPos.Y];
      auto& pos = ctx.CursorPos;
      
      switch( wp ) {
        // 改行
        case VK_RETURN: {
          if( pos.X == 0 ) {
            ctx.Source.insert(ctx.Source.begin() + pos.Y, L"");
            pos.Y++;
          }
          else if( pos.X == line.length() ) {
            pos.Y++;
            pos.X = 0;
            
            if( pos.Y == ctx.Source.size() )
              ctx.Source.emplace_back(L"");
            else
              ctx.Source.insert(ctx.Source.begin() + pos.Y, L"");
          }
          else {
            auto&& cut = std::move(line.substr(pos.X));
            
            line = std::move(line.substr(0, pos.X));
            pos.Y++;
            pos.X = 0;
            
            if( pos.Y == ctx.Source.size() )
              ctx.Source.emplace_back(cut);
            else
              ctx.Source.insert(ctx.Source.begin() + pos.Y, cut);
          }
          
          break;
        }
        
        // 削除 (Backspace)
        case VK_BACK: {
          if( pos.X == 0 ) {
            if( pos.Y >= 1 ) {
              pos.Y--;
              pos.X = ctx.Source[pos.Y].length();
              
              ctx.Source[pos.Y] += line;
              ctx.Source.erase(ctx.Source.begin() + pos.Y + 1);
            }
          }
          else {
            pos.X--;
            line.erase(line.begin() + pos.X);
          }
          
          break;
        }
        
        // 削除 (Delete)
        case VK_DELETE: {
          if( pos.X == line.length() ) {
            if( pos.Y < ctx.Source.size() - 1 ) {
              line += ctx.Source[pos.Y + 1];
              ctx.Source.erase(ctx.Source.begin() + pos.Y + 1);
            }
          }
          else {
            line.erase(line.begin() + pos.X);
          }
          
          break;
        }
      }
      
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
