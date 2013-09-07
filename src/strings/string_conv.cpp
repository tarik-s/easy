#include <easy/strings/string_conv.h>

#ifdef EASY_OS_WINDOWS

#include <easy/windows/error.h>
#include <Windows.h>

#endif

namespace easy
{
  namespace 
  {
#ifdef EASY_OS_WINDOWS
    utf16_string utf8_to_utf16_impl(const c_utf8_string& s, error_code_ref ec)
    {
      // first find the size
      int size = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, s.c_str(), -1, nullptr, 0);
      if (size == 0) {
        ec = windows::make_last_win_error();
        return utf16_string();
      }

      // this length includes the terminating null
      utf16_string result(size-1, L'\0');

      // now call again to format the string
      int res_size = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, s.c_str(), -1, &result.front(), size);
      if (res_size == size)
        return result;

      ec = windows::make_last_win_error();
      return utf16_string();
    }

    utf8_string utf16_to_utf8_impl(const c_utf16_string& s, error_code_ref ec)
    {
#if EASY_WIN_VERSION < EASY_WINDOWS_VISTA
      const DWORD flags = 0;
#else
      const DWORD flags = WC_ERR_INVALID_CHARS;
#endif
      // first find the size
      int size = ::WideCharToMultiByte(CP_UTF8, flags, s.c_str(), -1,  nullptr, 0, nullptr, nullptr);
      if (size == 0) {
        ec = windows::make_last_win_error();
        return utf8_string();
      }

      // this length includes the terminating null
      utf8_string result(size-1, '\0');

      // now call again to format the string
      int res_size = ::WideCharToMultiByte(CP_UTF8, flags, s.c_str(), -1, &result.front(), size, nullptr, nullptr);
      if (res_size == size)
        return result;

      ec = windows::make_last_win_error();
      return utf8_string();
    }
#else

#endif

  }

  utf16_string utf8_to_utf16(const c_utf8_string& s, error_code_ref ec)
  {
    if (!s) // an empty string is not an error
      return utf16_string();
    return utf8_to_utf16_impl(s, ec);
  }

  utf8_string utf16_to_utf8(const c_utf16_string& s, error_code_ref ec)
  {
    if (!s) // an empty string is not an error
      return utf8_string();
    return utf16_to_utf8_impl(s, ec);
  }

}

