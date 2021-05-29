#pragma once

struct EditContext {
  std::vector<std::wstring>   Source;
  Point    CursorPos;
};

class Application {
  std::vector<EditContext>  Contexts;
  int CurrentIndex = 0;
  
  HWND      hwnd;
  WNDCLASS  winc;
  MSG       msg;
  WNDPROC   wndProc;
  HBITMAP   hBitmap;
  HDC       hBuffer;
  HINSTANCE hInstance;
  std::wstring className;
  
  Size      WindowSize = { 600, 400 };
  
  EditContext& GetCurrentContext() const;
  
  void ForceRedraw();
  void DrawEditor();
  
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