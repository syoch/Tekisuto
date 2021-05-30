#pragma once

#define  COLOR_BLACK    RGB(0, 0, 0)
#define  COLOR_WHITE    RGB(255, 255, 255)
#define  COLOR_RED      RGB(255, 0, 0)
#define  COLOR_GREEN    RGB(0, 255, 0)
#define  COLOR_BLUE     RGB(0, 0, 255)

#define  CHAR_WIDTH    7    //
#define  CHAR_HEIGHT   14   // 文字サイズ

#define  WINDOW_BACKCOLOR   COLOR_BLACK   // エディタ 編集部分 背景色

#define  WINDOW_WIDTH_MAX   2000   //
#define  WINDOW_HEIGHT_MAX  1200   // 最大ウィンドウサイズ

#define  LINENUM_BAR_WIDTH  64               // 行番号バー 横幅
#define  LINENUM_BACKCOLOR  RGB(30,30,30)    // 行番号バー 背景色

#define  SCROLLBAR_WIDTH        20              // スクロールバー 横幅
#define  SCROLLBAR_BACKCOLOR    RGB(50,50,50)   // スクロールバー 背景色
#define  SCROLLBAR_BAR_COLOR    RGB(80,80,80)   // スクロールバー つまみ 色

#define  CURRENT_LINE_BACKCOLOR  RGB(45,45,45)   // カーソル行 背景色

// タイマー
#define  TIMER_SCROLLBAR    100
//#define  TIMER_COLORING     200 使わない


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