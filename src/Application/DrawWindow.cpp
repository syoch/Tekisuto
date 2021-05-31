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

void Application::DrawLinenum() {
  auto& ctx = GetCurrentContext();

  // 行番号 背景
  Drawing::DrawRect(0, 0, LINENUM_BAR_WIDTH - 4, ClientSize.Height, LINENUM_BACKCOLOR);

  // 行番号と編集部分の区切り
  Drawing::DrawLine(LINENUM_BAR_WIDTH - 4, 0, LINENUM_BAR_WIDTH - 4, ClientSize.Height, COLOR_WHITE);

  int posY = 0;
  for( auto i = DrawIndexRange.Begin; i < DrawIndexRange.End; i++, posY += CHAR_HEIGHT ) {
    Drawing::DrawString(format(L"%8d", i + 1), 0, posY, RGB(255, 255, 255), 0, true);
  }
}

void Application::DrawScrollBar() {
  auto& ctx = GetCurrentContext();

  // スクロールバー 背景
  Drawing::DrawRect(ClientSize.Width - SCROLLBAR_WIDTH, 0, SCROLLBAR_WIDTH, ClientSize.Height, SCROLLBAR_BACKCOLOR);

  // スクロールバー つまみ
//  int barSize = ((float)ClientSize.Height / (float)ctx.Source.size());
//  if( barSize < 20 ) barSize = 20;
  Drawing::DrawRect(ClientSize.Width - SCROLLBAR_WIDTH + 2, ctx.ScrollBar_Pos_Draw, SCROLLBAR_WIDTH - 4, barSize, SCROLLBAR_BAR_COLOR);

}

// ------------------------------------------------------- //
//  エディタ 描画
// ------------------------------------------------------- //
void Application::DrawEditor() {
  static PAINTSTRUCT ps;

  auto& ctx = GetCurrentContext();

  // 背景
  Drawing::DrawRect(0, 0, ClientSize.Width, ClientSize.Height, WINDOW_BACKCOLOR);

  // ソースコードの描画する範囲
  // 開始 ~ 終了 まで (単位 = 行)
  DrawIndexRange = {
    ctx.ScrollY,
    std::min<int>(ctx.Source.size(), DrawIndexRange.Begin + ClientSize.Height / CHAR_HEIGHT + 10)
  };

  // カーソルがある行の背景を明るくする
  if( ctx.CursorPos.Y >= DrawIndexRange.Begin &&
    ctx.CursorPos.Y <= DrawIndexRange.End ) {
    Drawing::DrawRect(0, (ctx.CursorPos.Y - ctx.ScrollY) * CHAR_HEIGHT, ClientSize.Width, CHAR_HEIGHT, CURRENT_LINE_BACKCOLOR);
    Drawing::DrawRect(0, (ctx.CursorPos.Y - ctx.ScrollY) * CHAR_HEIGHT, ClientSize.Width, CHAR_HEIGHT, CURRENT_LINE_BOXCOLOR, false);
    Drawing::DrawRect(0, (ctx.CursorPos.Y - ctx.ScrollY) * CHAR_HEIGHT + 1, ClientSize.Width, CHAR_HEIGHT, CURRENT_LINE_BOXCOLOR, false);
  }

  // ソースコード 描画
  for( auto&& token : ctx.ColorData ) {
    if( token.index < DrawIndexRange.Begin )
      continue;

    if( token.index >= DrawIndexRange.End )
      break;

    auto posY = (token.index - ctx.ScrollY) * CHAR_HEIGHT;
    SetTextColor(hBuffer, token.color);
    TextOut(hBuffer, LINENUM_BAR_WIDTH + token.position * CHAR_WIDTH, posY, &ctx.Source[token.index][0] + token.position, token.length);
  }

  // カーソル
  if( ctx.CursorPos.Y >= DrawIndexRange.Begin && ctx.CursorPos.Y < DrawIndexRange.End )
    Drawing::DrawRect(ctx.CursorPos.X * CHAR_WIDTH + LINENUM_BAR_WIDTH, (ctx.CursorPos.Y - ctx.ScrollY) * CHAR_HEIGHT, 2, CHAR_HEIGHT, RGB(255, 255, 255));

  DrawLinenum();
  DrawScrollBar();


  // バッファからウィンドウにコピー (ダブルバッファリング)
  auto hdc = BeginPaint(hwnd, &ps);
  BitBlt(hdc, 0, 0, ClientSize.Width, ClientSize.Height, hBuffer, 0, 0, SRCCOPY);
  EndPaint(hwnd, &ps);
}