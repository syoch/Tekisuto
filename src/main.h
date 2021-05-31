#pragma once

#include <algorithm>
#include <fstream>
#include <cstring>
#include <codecvt>
#include <utility>
#include <chrono>
//#include <thread>
#include <locale>
#include <string>
#include <vector>
#include <memory>
//#include <mutex>

#define  UNICODE
#include <windows.h>

// メニューバー ID
#define  MENUBAR_FILE_OPEN            10000
#define  MENUBAR_FILE_OPTIONS         10100
#define  MENUBAR_FILE_EXIT            10200
#define  MENUBAR_DEBUG_NEW_CONTEXT    10300

// トークンの色
#define  TOKEN_PREPROCESS         RGB(157,126,98)    // プリプロセッサ
#define  TOKEN_NUMBER             RGB(250,150,0)     // 数字
#define  TOKEN_IDENT              RGB(255,255,255)   // 識別子
#define  TOKEN_STRING             RGB(200,200,200)   // 文字列
#define  TOKEN_TYPEWORD           RGB(240,130,240)   // 予約語 1
#define  TOKEN_KEYWORD            RGB(100,200,255)   // 予約語 2
#define  TOKEN_COMMENT            RGB(40,200,40)     // コメント
#define  TOKEN_DEFAULT            RGB(255,255,255)   // デフォルト

// カラー
#define  COLOR_BLACK              RGB(0, 0, 0)
#define  COLOR_WHITE              RGB(255, 255, 255)
#define  COLOR_RED                RGB(255, 0, 0)
#define  COLOR_GREEN              RGB(0, 255, 0)
#define  COLOR_BLUE               RGB(0, 0, 255)

// 文字サイズ
#define  CHAR_WIDTH               7
#define  CHAR_HEIGHT              14

// ウィンドウ 背景色
#define  WINDOW_BACKCOLOR         COLOR_BLACK

// 最大ウィンドウサイズ
#define  WINDOW_WIDTH_MAX         2000
#define  WINDOW_HEIGHT_MAX        1200

// 行番号
#define  LINENUM_BAR_WIDTH        64               // 横幅
#define  LINENUM_BACKCOLOR        RGB(30,30,30)    // 背景色

// スクロールバー
#define  SCROLLBAR_WIDTH          20              // 横幅
#define  SCROLLBAR_BACKCOLOR      RGB(50,50,50)   // 背景色
#define  SCROLLBAR_BAR_COLOR      RGB(80,80,80)   // つまみ 色

#define  CURRENT_LINE_BACKCOLOR   RGB(30,30,30)   // カーソル行 背景色
#define  CURRENT_LINE_BOXCOLOR    RGB(50,50,50)   // カーソル行 四角形の色

// タイマー ID
#define  TIMER_SCROLLBAR          100
//#define  TIMER_COLORING           200


struct Size {
  int Width = 0;
  int Height = 0;
};

struct Point {
  int X = 0;
  int Y = 0;
};

struct Range {
  int Begin = 0;
  int End = 0;
};

struct Token {
  //wchar_t*   stptr;
  int        length;
  int        index;
  int        position;
  COLORREF   color;
};

struct EditContext {
  std::vector<std::wstring>   Source;
  Point    CursorPos;

  std::vector<Token>  ColorData;

  int  ScrollY = 0;
  int  ScrollBar_Pos_Real = 0;
  int  ScrollBar_Pos_Draw = 0;

  bool   IsMouseDown = false;
};

class Application {
public:
  std::vector<EditContext>  Contexts;
  int CurrentIndex = 0;

  HWND      hwnd;          // ウィンドウハンドル
  WNDCLASS  winc;          // ウィンドウクラス
  MSG       msg;           // メッセージ
  WNDPROC   wndProc;       // ウィンドウプロシージャ
  HINSTANCE hInstance;     // アプリケーションのインスタンスハンドル
  std::wstring className;  // クラス名

  // ダブルバッファリング
  HBITMAP   hBitmap;       // ビットマップ
  HDC       hBuffer;       // ビットマップ バッファ

  // ウィンドウ情報
  WINDOWINFO  WindowInfo;

  // マウス座標
  Point   MousePos;         // 画面上
  Point   MousePos_Client;  // クライアント上

  int barSize;
  bool IsScrolling = false; // スクロールバーを操作中か

  Size   WindowSize = { 600, 400 };    // ウィンドウサイズ
  Size   ClientSize;                   // クライアントサイズ

  // 描画の開始から終了まで
  Range  DrawIndexRange;

  bool IsScrollTimerLocked = false;

  EditContext& GetCurrentContext() const;

  void ForceRedraw();

  void DrawLinenum();
  void DrawScrollBar();
  void DrawEditor();

  void UpdateWindowInfo();
  void UpdateMouseInfo();
  void UpdateCursorPos();

  void SourceColoring();

  void CheckScrollY(int& y);
  void CheckScrollBarPos(int& pos);

  LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
  friend LRESULT CALLBACK WndProc_Wrap(HWND, UINT, WPARAM, LPARAM);

  Application(std::wstring const& class_name);
  ~Application();

  ATOM  Register(HINSTANCE hInstance);
  bool  Create();
  void  Show();

  WPARAM  RunMainLoop();

  static Application* GetInstance();

};

class SourceIterator {
public:
  Application* app;
  int index = 0;
  int position = 0;
  int value = 0;

  SourceIterator(Application* app)
    :app(app)
  {
  }

  std::wstring& get_line() const {
    return app->GetCurrentContext().Source[index];
  }

  wchar_t& operator * () {
    return get_line()[position];
  }
  
  bool match(std::wstring const& str) const {
    auto& line = get_line();

    return position + str.length() <= line.length() && line.substr(position, str.length()) == str;
  }

  void operator ++ (int) {
    if( position == get_line().length() ) {
      position++;

      if( index < app->GetCurrentContext().Source.size() - 1 ) {
        index++;
        position = 0;
        value++;
      }
    }
    else {
      position++;
      value++;
    }
  }

  void operator -- (int) {
    if( position == 0 ) {
      if( index >= 1 ) {
        index--;
        position = get_line().length();
        value--;
      }
    }
    else {
      position--;
      value--;
    }
  }

  void operator += (int n) {
    for( int i = 0; i < n; i++ )
      (*this)++;
  }

  bool check() {
    auto& ctx = app->GetCurrentContext();

    return index < ctx.Source.size() && position <= ctx.Source[index].length();
  }

  bool line_check() {
    return position < get_line().length();
  }
};

template <class... Args>
std::wstring format(std::wstring fmt, Args... args) {
  wchar_t buf[1000];
  wsprintf(buf, fmt.c_str(), args...);
  return buf;
}



namespace Drawing {
  void SetTarget(HDC hdc);

  void DrawPixel(int x, int y, COLORREF color);
  COLORREF GetPixel(int x, int y);

  void DrawRect(int x, int y, int width, int height, COLORREF color, bool fill = true);
  void DrawLine(int x1, int y1, int x2, int y2, COLORREF color);
  void DrawString(std::wstring const& str, int x, int y, COLORREF forecolor, COLORREF backcolor = RGB(0, 0, 0), bool transparent = false);
  void DrawChar(wchar_t ch, int x, int y, COLORREF color);
}


