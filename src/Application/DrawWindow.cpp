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

void Application::CheckScrollY(int& y) {
  auto& ctx = GetCurrentContext();
  
  if( y < 0 ) y = 0;
  if( y >= ctx.Source.size() ) y = ctx.Source.size() - 1;
}

void Application::CheckScrollBarPos(int& pos) {
  //pos -= 20;
  
  if( pos < 0 ) pos = 0;
  if( pos > ClientSize.Height - barSize ) pos = ClientSize.Height - barSize;
}

// ------------------------------------------------------- //
//  エディタ 描画
// ------------------------------------------------------- //
void Application::DrawEditor() {
  std::lock_guard<std::mutex> _lock(Mtx);
  
  PAINTSTRUCT ps;
  
  auto& ctx = GetCurrentContext();
  
  // 背景
  Drawing::DrawRect(0, 0, ClientSize.Width, ClientSize.Height, WINDOW_BACKCOLOR);
  
  // 行番号 背景
  Drawing::DrawRect(0, 0, LINENUM_BAR_WIDTH - 4, ClientSize.Height, LINENUM_BACKCOLOR);
  
  // 行番号と編集部分の区切り
  Drawing::DrawLine(LINENUM_BAR_WIDTH - 4, 0, LINENUM_BAR_WIDTH - 4, ClientSize.Height, COLOR_WHITE);
  
  // ソースコードの描画する範囲
  // 開始 ~ 終了 まで (単位 = 行)
  const int begin = ctx.ScrollY;
  const int end = std::min<int>(ctx.Source.size(), begin + ClientSize.Height / CHAR_HEIGHT + 1);
  
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
    Drawing::DrawRect(ctx.CursorPos.X * CHAR_WIDTH + LINENUM_BAR_WIDTH, (ctx.CursorPos.Y - ctx.ScrollY) * CHAR_HEIGHT, 2, CHAR_HEIGHT, RGB(255, 255, 255));
  
  // スクロールバー 背景
  Drawing::DrawRect(ClientSize.Width - SCROLLBAR_WIDTH, 0, SCROLLBAR_WIDTH, ClientSize.Height, SCROLLBAR_BACKCOLOR);
  
  // スクロールバー つまみ
//  int barSize = ((float)ClientSize.Height / (float)ctx.Source.size());
//  if( barSize < 20 ) barSize = 20;
  Drawing::DrawRect(ClientSize.Width - SCROLLBAR_WIDTH + 2, ctx.ScrollBar_Pos_Draw, SCROLLBAR_WIDTH - 4, barSize, SCROLLBAR_BAR_COLOR);
  
  
  // バッファからウィンドウにコピー (ダブルバッファリング)
  auto hdc = BeginPaint(hwnd, &ps);
  BitBlt(hdc, 0, 0, WindowSize.Width, WindowSize.Height, hBuffer, 0, 0, SRCCOPY);
  EndPaint(hwnd, &ps);
}