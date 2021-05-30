#include "main.h"

struct SourceIterator {
  Application* app;
  int index, position;
  int value;
  
  SourceIterator(Application* app)
    :app(app), index(0), position(0), value(0)
  {
  }
  
  auto& get_line() const{
    return app->GetCurrentContext().Source[index];
  }
  
  auto& operator * () {
    return get_line()[position];
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
  
  bool check() {
    auto& ctx = app->GetCurrentContext();
    
    return index < ctx.Source.size() && position <= ctx.Source[index].length();
  }
  
  bool line_check() {
    return position < get_line().length();
  }
};

#define  TOKEN_PREPROCESS   RGB(157,126,98)
#define  TOKEN_NUMBER       RGB(250,150,0)
#define  TOKEN_IDENT        RGB(255,255,255)
#define  TOKEN_STRING       RGB(150,150,150)
#define  TOKEN_TYPEWORD     RGB(240,130,240)
#define  TOKEN_KEYWORD      RGB(100,200,255)

#define  _MAKE(cond)  \
  while( cond ) tok.length++, it++


void Application::SourceColoring() {
  auto& ctx = GetCurrentContext();
  
  SourceIterator it = Application::GetInstance();
  ctx.ColorData.clear();
  
  while( it.check() ) {
    auto c = *it;
    Token tok { };
    tok.index = it.index;
    tok.position = it.position;
    
    // proprocess
    if( c == '#' ) {
      tok.color = TOKEN_PREPROCESS;
      _MAKE( it.line_check() && *it != '\n' );
    }
    
    // number
    else if( isdigit(c) ) {
      tok.color = TOKEN_NUMBER;
      _MAKE( it.check() && (isalnum(*it) || *it == '.') );
    }
    
    // identifier
    else if( isalpha(c) || c == '_' ) {
      tok.color = TOKEN_IDENT;
      _MAKE( it.line_check() && (isalnum(*it) || *it == '_') );
    }
    
    // string / char
    else if( c == '\'' || c == '"' ) {
      auto ch = c;
      tok.color = TOKEN_STRING;
      it++;
      tok.length++;
      _MAKE(it.check() && *it != ch );
      it++;
      tok.length++;
    }
    
    // other
    else {
      tok.color = COLOR_WHITE;
      _MAKE(it.check()&&(!isalnum(*it)));
    }
    
    
    
    ctx.ColorData.emplace_back(tok);
    
    it++;
  }
}