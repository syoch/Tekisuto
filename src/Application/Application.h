#pragma once

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