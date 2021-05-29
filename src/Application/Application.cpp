#include "../main.h"

#define  CLASS_NAME  TEXT("TEKISUTO")

static Application* _app_instance;

LRESULT CALLBACK WndProc_Wrap(HWND, UINT, WPARAM, LPARAM);

EditContext& Application::GetCurrentContext() const {
  return const_cast<EditContext&>(Contexts[CurrentIndex]);
}

// Application コンストラクタ
Application::Application(std::wstring const& class_name)
  :className(class_name) {
  Contexts.resize(1);
  
  for(auto&& L: {
    L"#include <iostream>",
    L"",
    L"int main(int argc, char** argv) {",
    L"  std::cout << \"Hello, World!\\n\"",
    L"}"
  } ) {
    GetCurrentContext().Source.emplace_back(L);
  }
  
  _app_instance = this;
}

Application::~Application()
  = default;

ATOM  Application::Register(HINSTANCE hInstance) {
  this->hInstance = hInstance;
  
  winc.style          = CS_HREDRAW | CS_VREDRAW;
  winc.lpfnWndProc    = WndProc_Wrap;
  winc.cbClsExtra     = winc.cbWndExtra	= 0;
  winc.hInstance      = hInstance;
  winc.hIcon          = LoadIcon(NULL , IDI_APPLICATION);
  winc.hCursor        = LoadCursor(NULL , IDC_ARROW);
  winc.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
  winc.lpszMenuName	  = TEXT("TEKISUTO_MENUBAR");
  winc.lpszClassName  = className.c_str();
  
  return RegisterClass(&winc);
}

bool Application::Create() {
  hwnd = CreateWindow(
    className.c_str(), TEXT("Tekisuto"),
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT,
    WindowSize.Width, WindowSize.Height,
    NULL, NULL,
    hInstance, NULL
  );
  
  return hwnd != NULL;
}

void  Application::Show() {
  ShowWindow(hwnd, SW_SHOW);
}

WPARAM  Application::RunMainLoop() {
  while( GetMessage(&msg, NULL, 0, 0) ) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  
  return msg.wParam;
}

Application* Application::GetInstance() {
  return _app_instance;
}