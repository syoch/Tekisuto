#pragma once

#include <algorithm>
#include <fstream>
#include <cstring>
#include <codecvt>
#include <utility>
#include <chrono>
#include <thread>
#include <locale>
#include <string>
#include <vector>
#include <memory>
#include <mutex>

#define  UNICODE
#include <windows.h>

#include "Drawing.h"
#include "Application/Application.h"
#include "resource.h"

template <class... Args>
std::wstring format(std::wstring fmt, Args... args) {
  wchar_t buf[1000];
  wsprintf(buf, fmt.c_str(), args...);
  return buf;
}