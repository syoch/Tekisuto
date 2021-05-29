#pragma once

#define  CHAR_WIDTH    7    //
#define  CHAR_HEIGHT   14   // 文字サイズ

#define  WINDOW_BACKCOLOR   RGB(30, 30, 30)    // エディタ 編集部分 背景色

#define  WINDOW_WIDTH_MAX   2000   //
#define  WINDOW_HEIGHT_MAX  1200   // 最大ウィンドウサイズ

#define  LINENUM_BAR_WIDTH  64               // 行番号バー 横幅
#define  LINENUM_BACKCOLOR  RGB(80,80,80)    // 行番号バー 背景色

struct Size {
  int Width = 0;
  int Height = 0;
};

struct Point {
  int X = 0;
  int Y = 0;
};

struct EditContext {
  std::vector<std::wstring>   Source;
  Point    CursorPos;
  
  bool   IsMouseDown = false;
};

class Application {
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
  
  
  
  Size   WindowSize = { 600, 400 };    // ウィンドウサイズ
  Size   ClientSize;                   // クライアントサイズ
  
  EditContext& GetCurrentContext() const;
  
  void ForceRedraw();
  void DrawEditor();
  
  void UpdateWindowInfo();
  Point UpdateMouseInfo();
  
  LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
  friend LRESULT CALLBACK WndProc_Wrap(HWND, UINT, WPARAM, LPARAM);
  
public:
  Application(std::wstring const& class_name);
  ~Application();
  
  ATOM  Register(HINSTANCE hInstance);
  bool  Create();
  void  Show();
  
  WPARAM  RunMainLoop();
  
  static Application* GetInstance();
  
};