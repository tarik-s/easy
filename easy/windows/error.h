#ifndef EASY_WINDOWS_ERROR_H_INCLUDED
#define EASY_WINDOWS_ERROR_H_INCLUDED

#include <easy/windows/config.h>

#include <easy/error_code_ref.h>

#include <Windows.h>

namespace easy {
namespace windows 
{
  inline error_code win_error(DWORD code) {
    return error_code(code, boost::system::system_category());
  }

  inline error_code last_win_error() {
    return win_error(::GetLastError());
  }


}}

#endif