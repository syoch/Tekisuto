#include "main.h"

static auto Punctuaters = {
  L"::",
  L"&=",
  L"|=",
  L"^=",
  L"+=",
  L"-=",
  L"*=",
  L"/=",
  L"%=",
  L">>",
  L"<<",
  L"==",
  L"!=",
  L">=",
  L"<=",
  L"&&",
  L"||",
  L"!",
  L"~",
  L"&",
  L"|",
  L">",
  L"<",
  L"=",
  L"+",
  L"-",
  L"*",
  L"/",
  L"%",
  L"(",
  L")",
  L"[",
  L"]",
  L"{",
  L"}",
  L",",
  L".",
  L";",
  L":",
};

#define  TOKEN_PREPROCESS   RGB(157,126,98)    // プリプロセッサ
#define  TOKEN_NUMBER       RGB(250,150,0)     // 数字
#define  TOKEN_IDENT        RGB(255,255,255)   // 識別子
#define  TOKEN_STRING       RGB(200,200,200)   // 文字列
#define  TOKEN_TYPEWORD     RGB(240,130,240)   // 予約語 1
#define  TOKEN_KEYWORD      RGB(100,200,255)   // 予約語 2
#define  TOKEN_COMMENT      RGB(40,200,40)     // コメント

#define  _MAKE(cond)  \
  while( cond ) tok.length++, it++

static auto _Wtypewords = {
  L"volatile",
  L"private",
  L"public",
  L"extern",
  L"size_t",
  L"static",
  L"string",
  L"struct",
  L"double",
  L"inline",
  L"float",
  L"class",
  L"short",
  L"const",
  L"enum",
  L"auto",
  L"char",
  L"bool",
  L"void",
  L"long",
  L"int",
  L"asm",
};

static auto _Wkeywords = {
  L"reinterpret_cast",
  L"dynamic_cast",
  L"static_cast",
  L"const_cast",
  L"constexpr",
  L"namespace",
  L"template",
  L"typename",
  L"continue",
  L"operator",
  L"typedef",
  L"default",
  L"private",
  L"nullptr",
  L"public",
  L"return",
  L"delete",
  L"switch",
  L"using",
  L"while",
  L"break",
  L"catch",
  L"throw",
  L"false",
  L"true",
  L"else",
  L"case",
  L"NULL",
  L"try",
  L"for",
  L"new",
  L"if",
  L"do",
};

void Application::SourceColoring() {
  auto& ctx = GetCurrentContext();

  SourceIterator it = Application::GetInstance();
  ctx.ColorData.clear();

  while( it.check() ) {
    auto c = *it;
    Token tok = { };
    tok.index = it.index;
    tok.position = it.position;

    // proprocess
    if( c == '#' ) {
      tok.color = TOKEN_PREPROCESS;
    REjmpPrpp:
      _MAKE(it.line_check() && *it != '\n');

      if( it.get_line()[it.get_line().length() - 1] == '\\' ) {
        ctx.ColorData.emplace_back(tok);
        it++;
        tok.length = 0;
        tok.index = it.index;
        tok.position = it.position;
        if( it.check() ) goto REjmpPrpp;
      }
    }

    // number
    else if( isdigit(c) ) {
      tok.color = TOKEN_NUMBER;
      _MAKE(it.check() && (isalnum(*it) || *it == '.'));
    }

    // identifier
    else if( isalpha(c) || c == '_' ) {
      tok.color = TOKEN_IDENT;
      _MAKE(it.line_check() && (isalnum(*it) || *it == '_'));

      // find keywords
      std::wstring ws = ctx.Source[tok.index].substr(tok.position, tok.length);

      for( auto&& T : _Wtypewords ) {
        if( ws == T ) {
          tok.color = TOKEN_TYPEWORD;
          goto Atomjmp;
        }
      }

      for( auto&& T : _Wkeywords ) {
        if( ws == T ) {
          tok.color = TOKEN_KEYWORD;
          goto Atomjmp;
        }
      }

    Atomjmp:;
    }

    // string / char
    else if( c == '\'' || c == '"' ) {
      auto ch = c;
      tok.color = TOKEN_STRING;
      it++;
      tok.length++;

      while( it.check() ) {
        if( *it == ch ) {
          it++;
          tok.length++;
          break;
        }
        else if( !it.line_check() ) {
          ctx.ColorData.emplace_back(tok);
          it++;
          tok.length = 0;
          tok.index = it.index;
          tok.position = it.position;
        }
        else {
          tok.length++;
          it++;
        }
      }
    }

    // line comment
    else if( c == '/' ) {
      it++;
      if( it.check() && *it == '/' ) {
        it--;
      REjmp:
        tok.color = TOKEN_COMMENT;
        _MAKE(it.line_check());

        if( it.get_line()[it.get_line().length() - 1] == '\\' ) {
          ctx.ColorData.emplace_back(tok);
          it++;
          tok.length = 0;
          tok.index = it.index;
          tok.position = it.position;
          if( it.check() ) goto REjmp;
        }
      }
      else if( it.check() && *it == '*' ) { // block comment
        it--;
        tok.color = TOKEN_COMMENT;

        tok.length = 2;
        ctx.ColorData.emplace_back(tok);
        tok.length = 0;
        it++; it++;

        while( 1 ) {
          if( !it.check() )
            break;
          
          while( it.line_check() ) {
            if( *it == '*' ) {
              it++; if( it.line_check() && *it == '/' ) {
                it++;
                tok.length = it.position;
                ctx.ColorData.emplace_back(tok);
                goto ENDjmp;
              }
              else it--;
            }
            tok.length++;
            it++;
          }

          tok.length = it.get_line().length();
          ctx.ColorData.emplace_back(tok);
          it++;
          tok.length = 0;
          tok.index = it.index;
          tok.position = it.position;
        }
      ENDjmp:;
      }
      else {
        it--; goto OTHERjmp;
      }
    }

    // other
    else if( c > ' ' ) {
    OTHERjmp:
      tok.color = COLOR_WHITE;
      for( std::wstring X : Punctuaters ) {
        if( it.match(X) ) {
          tok.length += X.length();
          it += X.length();
          goto _Zm;
        }
      }
      tok.length = 1;
      it++;
    _Zm:;
    }
    else
      it++;



    ctx.ColorData.emplace_back(tok);

  }
}