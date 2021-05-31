#pragma once

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

  auto& get_line() const {
    return app->GetCurrentContext().Source[index];
  }

  auto& operator * () {
    return get_line()[position];
  }
  
  auto match(std::wstring const& str) const {
    auto& line = get_line();

    return position + str.length() <= line.length() && line.substr(position, str.length()) == str;
  }

  auto operator ++ (int) {
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

  auto operator -- (int) {
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

  auto operator += (int n) {
    for( int i = 0; i < n; i++ )
      (*this)++;
  }

  auto check() {
    auto& ctx = app->GetCurrentContext();

    return index < ctx.Source.size() && position <= ctx.Source[index].length();
  }

  auto line_check() {
    return position < get_line().length();
  }
};