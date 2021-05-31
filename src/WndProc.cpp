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
void Application::UpdateMouseInfo() {
  auto& ctx = GetCurrentContext();

  // マウス座標取得 (モニター)
  POINT mpos;
  GetCursorPos(&mpos);

  MousePos = { mpos.x, mpos.y };

  MousePos_Client = {
    mpos.x - WindowInfo.rcClient.left - LINENUM_BAR_WIDTH,
    mpos.y - WindowInfo.rcClient.top
  };

  if( MousePos_Client.X < 0 ) MousePos_Client.X = 0;
  if( MousePos_Client.Y < 0 ) MousePos_Client.Y = 0;

  // if( ctx.CursorPos.Y < 0 ) ctx.CursorPos.Y = 0;
  // if( ctx.CursorPos.Y >= ctx.Source.size() ) ctx.CursorPos.Y = ctx.Source.size() - 1;

  // if( ctx.CursorPos.X < 0 ) ctx.CursorPos.X = 0;
  // if( ctx.CursorPos.X > ctx.Source[ctx.CursorPos.Y].length() )
    // ctx.CursorPos.X = ctx.Source[ctx.CursorPos.Y].length();

}

void Application::UpdateCursorPos() {
  auto& ctx = GetCurrentContext();

  ctx.CursorPos = {
    MousePos_Client.X / CHAR_WIDTH,
    MousePos_Client.Y / CHAR_HEIGHT + ctx.ScrollY
  };

  if( ctx.CursorPos.Y < 0 ) ctx.CursorPos.Y = 0;
  if( ctx.CursorPos.Y >= ctx.Source.size() ) ctx.CursorPos.Y = ctx.Source.size() - 1;

  if( ctx.CursorPos.X < 0 ) ctx.CursorPos.X = 0;
  if( ctx.CursorPos.X > ctx.Source[ctx.CursorPos.Y].length() )
    ctx.CursorPos.X = ctx.Source[ctx.CursorPos.Y].length();

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

  barSize = ((float)ClientSize.Height / (float)ctx.Source.size());
  if( barSize < 40 ) barSize = 40;

  UpdateWindowInfo();

  switch( msg ) {
    // メニューバーなど
    case WM_COMMAND:
    {
      switch( LOWORD(wp) ) {
        // ファイル -> 開く
        case MENUBAR_FILE_OPEN:
        {
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

          SourceColoring();

          ctx.CursorPos = { 0, 0 };
          ctx.ScrollY = 0;
          ctx.ScrollBar_Pos_Real = false;
          ctx.ScrollBar_Pos_Draw = false;
          ctx.IsMouseDown = false;
          DrawEditor();
          ForceRedraw();
          break;
        }
      }

      break;
    }

    // タイマー
    case WM_TIMER:
    {
      switch( wp ) {
        case TIMER_SCROLLBAR:
        {
          //auto old_val = ctx.ScrollBar_Pos_Draw;

          ctx.ScrollBar_Pos_Draw += (ctx.ScrollBar_Pos_Real - ctx.ScrollBar_Pos_Draw) / 4;

          // if( !IsScrollTimerLocked && abs(old_val - ctx.ScrollBar_Pos_Draw) <= 1 ){
            // KillTimer(hwnd, TIMER_SCROLLBAR);
            // break;
          // }

          DrawEditor();
          ForceRedraw();
          break;
        }
      }

      break;
    }

    // ウィンドウ 作成
    case WM_CREATE:
    {
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

      //
      // ダイアログ構造体の初期化
      ofn.lStructSize = sizeof(OPENFILENAME);
      ofn.hwndOwner = hwnd;
      ofn.lpstrFilter = TEXT("C++ Files (.cpp)\0*.cpp\0All files\0*.*\0");
      ofn.nMaxCustFilter = 256;
      ofn.nFilterIndex = 0;
      ofn.lpstrFile = OpenFile_buf;
      ofn.nMaxFile = MAX_PATH;
      ofn.Flags = OFN_FILEMUSTEXIST;

      SelectObject(hBuffer, hBitmap);
      SelectObject(hBuffer, GetStockObject(NULL_PEN));
      SelectObject(hBuffer, hFont);

      Drawing::SetTarget(hBuffer);

      // タイマー初期化
      SetTimer(hwnd, TIMER_SCROLLBAR, 8, NULL);
      //SetTimer(hwnd, TIMER_COLORING, 16, NULL);

      ReleaseDC(hwnd, hdc);

      break;
    }

    // ウィンドウ 表示 / 非表示
    case WM_SHOWWINDOW:
    {
      UpdateWindowInfo();
      SourceColoring();
      DrawEditor();
      //ForceRedraw();
      break;
    }

    // ウィンドウ 破棄
    case WM_DESTROY:
    {
      DeleteDC(hBuffer);
      DeleteObject(hBitmap);
      DeleteObject(hFont);

      PostQuitMessage(0);
      break;
    }

    // ウィンドウ 描画
    case WM_PAINT:
    {
      DrawEditor();
      break;
    }

    // ウィンドウ サイズ変更
    case WM_SIZE:
    {
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

      ctx.ScrollBar_Pos_Real = (float)ctx.ScrollY * ((float)ClientSize.Height / (float)ctx.Source.size()) - barSize / 2;
      CheckScrollBarPos(ctx.ScrollBar_Pos_Real);

      //DrawEditor();
      //ForceRedraw();
      break;
    }

    // マウスクリック
    case WM_LBUTTONDOWN:
    {
      UpdateMouseInfo();
      ctx.IsMouseDown = true;

      if(
        MousePos.X >= WindowInfo.rcClient.left + LINENUM_BAR_WIDTH &&
        MousePos.Y >= WindowInfo.rcClient.top &&
        MousePos.X < WindowInfo.rcClient.right - SCROLLBAR_WIDTH &&
        MousePos.Y < WindowInfo.rcClient.bottom
        ) {
        UpdateCursorPos();

      }
      else if( MousePos.X >= WindowInfo.rcClient.right - SCROLLBAR_WIDTH ) {
        IsScrolling = true;

        ctx.ScrollY = (MousePos_Client.Y - barSize / 2) / ((ClientSize.Height - barSize) / (float)ctx.Source.size());
        CheckScrollY(ctx.ScrollY);

        ctx.ScrollBar_Pos_Real = MousePos_Client.Y - barSize / 2;
        CheckScrollBarPos(ctx.ScrollBar_Pos_Real);

        IsScrollTimerLocked = true;
        SetTimer(hwnd, TIMER_SCROLLBAR, 2, NULL);
      }

      SetCapture(hwnd);

      //DrawEditor();
      //ForceRedraw();
      break;
    }

    // マウス 移動
    case WM_MOUSEMOVE:
    {
      if( !ctx.IsMouseDown )
        break;

      UpdateMouseInfo();

      if( IsScrolling ) { // スクロールバー
        ctx.ScrollY = (MousePos_Client.Y - barSize / 2) / ((ClientSize.Height - barSize) / (float)ctx.Source.size());
        CheckScrollY(ctx.ScrollY);

        ctx.ScrollBar_Pos_Real = MousePos_Client.Y - barSize / 2;
        CheckScrollBarPos(ctx.ScrollBar_Pos_Real);

      }
      else { // カーソル

        UpdateCursorPos();
      }

      DrawEditor();
      //ForceRedraw();
      break;
    }

    // マウス 離す
    case WM_LBUTTONUP:
    {
      ctx.IsMouseDown = false;
      IsScrolling = false;
      IsScrollTimerLocked = false;

      ReleaseCapture();
      break;
    }

    // マウスホイール
    case WM_MOUSEWHEEL:
    {
      int delta = GET_WHEEL_DELTA_WPARAM(wp);
      delta /= 40;

      ctx.ScrollY -= delta;
      CheckScrollY(ctx.ScrollY);

      ctx.ScrollBar_Pos_Real = (float)ctx.ScrollY * ((float)(ClientSize.Height - (ctx.Source.size() >= ClientSize.Height / CHAR_HEIGHT ? barSize : 0)) / (float)ctx.Source.size());
      CheckScrollBarPos(ctx.ScrollBar_Pos_Real);

      DrawEditor();
      ForceRedraw();
      break;
    }

    // 文字挿入
    case WM_CHAR:
    {
      static const std::wstring check_str = L" `~!@#$%^&*()_-+={[}]\\|'\";:,<.>/?";
      wchar_t keycode = wp & 0xFFFF;

      if( !isalnum(keycode) ) {
        if( check_str.find(keycode) == std::wstring::npos )
          break;
      }

      auto& line = ctx.Source[ctx.CursorPos.Y];
      line.insert(line.begin() + ctx.CursorPos.X, keycode);
      ctx.CursorPos.X++;

      SourceColoring();

      //DrawEditor();
      //ForceRedraw();
      break;
    }

    // キー入力
    case WM_KEYDOWN:
    {
      auto& line = ctx.Source[ctx.CursorPos.Y];
      auto& pos = ctx.CursorPos;

      switch( wp ) {
        // 改行
        case VK_RETURN:
        {
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
        case VK_BACK:
        {
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
        case VK_DELETE:
        {
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

      SourceColoring();

      //DrawEditor();
      //ForceRedraw();
      break;
    }


    //
    default:
      return DefWindowProc(hwnd, msg, wp, lp);
  }

  return 0;
}
