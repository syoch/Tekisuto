#include "main.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  Application app(L"TEKISUTO");
  
  if( !app.Register(hInstance) )
    return -1;
  
  if( !app.Create() )
    return -1;
  
  app.Show();
  app.RunMainLoop();
  
  return 0;
}