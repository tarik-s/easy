#ifndef EASY_STRINGABLE_STOCK_H_INCLUDED
#define EASY_STRINGABLE_STOCK_H_INCLUDED

#ifndef EASY_C_STRING_H_INCLUDED
#  error "You must include c_string.h before including this file"
#endif

#include <easy/config.h>
#include <easy/strings/stringable_fwd.h>
#include <easy/strings/c_string.h>


namespace easy
{
  template<> struct is_stringable<char*> : boost::true_type { };
  template<> struct is_stringable<const char*> : boost::true_type { };
  template<> struct is_stringable<volatile char*> : boost::true_type { };
  template<> struct is_stringable<const volatile char*> : boost::true_type { };
  template<> struct is_stringable<char[]> : boost::true_type { };
  template<size_t TSize> struct is_stringable<char[TSize]> : boost::true_type { };

  template<> struct is_stringable<std::string> : boost::true_type { };

#ifdef EASY_HAVE_WCAHR
  template<> struct is_stringable<wchar_t*> : boost::true_type { };
  template<> struct is_stringable<const wchar_t*> : boost::true_type { };
  template<> struct is_stringable<volatile wchar_t*> : boost::true_type { };
  template<> struct is_stringable<const volatile wchar_t*> : boost::true_type { };
  template<> struct is_stringable<wchar_t[]> : boost::true_type { };
  template<size_t TSize> struct is_stringable<wchar_t[TSize]> : boost::true_type { };

  template<>
  struct is_stringable<std::wstring> : boost::true_type { };
#endif
}



template<class TChar>
easy::basic_c_string<TChar> make_c_string(const TChar* pstr) {
  return easy::basic_c_string<TChar>(pstr, std::char_traits<TChar>::length(pstr));
}

namespace std
{
  template<class TChar>
  easy::basic_c_string<TChar> make_c_string(const basic_string<TChar>& str) {
    return easy::basic_c_string<TChar>(str.c_str(), str.size());
  }
}


#endif