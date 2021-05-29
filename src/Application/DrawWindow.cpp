#include "../main.h"

// ------------------------------------------------------- //
//  強制的に再描画する
//  WM_PAINT 以外のイベントで描画が必要な際、DrawEditor を呼び出しあと、必ずこれを呼んでください
// ------------------------------------------------------- //
void Application::ForceRedraw() {
  auto hdc = GetDC(hwnd);
  
  StretchBlt(
    hdc, 0, 0, WindowSize.Width, WindowSize.Height,
    hBuffer, 0, 0, WindowSize.Width, WindowSize.Height,
    SRCCOPY
  );
  
  DeleteDC(hdc);
}

// ------------------------------------------------------- //
//  エディタ 描画
// ------------------------------------------------------- //
void Application::DrawEditor() {
  PAINTSTRUCT ps;
  
  auto& ctx = GetCurrentContext();
  
  // 背景
  Drawing::DrawRect(0, 0, ClientSize.Width, ClientSize.Height, WINDOW_BACKCOLOR);
  
  // 行番号 背景
  Drawing::DrawRect(0, 0, LINENUM_BAR_WIDTH, ClientSize.Height, LINENUM_BACKCOLOR);
  
  // ソースコードの描画する範囲
  // 開始 ~ 終了 まで (単位 = 行)
  const int begin = 0;
  const int end = std::min<int>(ctx.Source.size(), begin + ClientSize.Height / CHAR_HEIGHT);
  
  // ソースコード 描画
  int posY = 0;
  for( auto i = begin; i < end; i++, posY += CHAR_HEIGHT ) {
    auto const& line = ctx.Source[i];
    
    Drawing::DrawString(line, LINENUM_BAR_WIDTH, posY, RGB(255, 255, 255), 0, true);
    
    // 行番号
    Drawing::DrawString(format(L"%8d", i + 1), 0, posY, RGB(255, 255, 255), 0, true);
  }
  
  // カーソル
  if( ctx.CursorPos.Y >= begin && ctx.CursorPos.Y < end )
    Drawing::DrawRect(ctx.CursorPos.X * CHAR_WIDTH, ctx.CursorPos.Y * CHAR_HEIGHT, 2, CHAR_HEIGHT, RGB(255, 255, 255));
  
  
  // バッファからウィンドウにコピー (ダブルバッファリング)
  auto hdc = BeginPaint(hwnd, &ps);
  BitBlt(hdc, 0, 0, WindowSize.Width, WindowSize.Height, hBuffer, 0, 0, SRCCOPY);
  EndPaint(hwnd, &ps);
}