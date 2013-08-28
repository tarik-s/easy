/*!
 * \file   easy/strings/detail/stringable_stock.inl
 * \author Sergey Tararay
 * \date   04.08.2013
 *
 * FILE DESCRIPTION
 */
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
  template<> struct get_underlying_char_type<char*> { typedef char type; };
  template<> struct get_underlying_char_type<const char*> { typedef char type; };
  template<> struct get_underlying_char_type<volatile char*> { typedef char type; };
  template<> struct get_underlying_char_type<const volatile char*> { typedef char type; };
  template<> struct get_underlying_char_type<char[]> { typedef char type; };
  template<size_t TSize> struct get_underlying_char_type<char[TSize]> { typedef char type; };

  template<> struct get_underlying_char_type<std::string> { typedef char type; };

#ifdef EASY_HAS_WCAHR
  template<> struct get_underlying_char_type<wchar_t*> { typedef wchar_t type; };
  template<> struct get_underlying_char_type<const wchar_t*> { typedef wchar_t type; };
  template<> struct get_underlying_char_type<volatile wchar_t*> { typedef wchar_t type; };
  template<> struct get_underlying_char_type<const volatile wchar_t*> { typedef wchar_t type; };
  template<> struct get_underlying_char_type<wchar_t[]> { typedef wchar_t type; };
  template<size_t TSize> struct get_underlying_char_type<wchar_t[TSize]> { typedef wchar_t type; };

  template<>
  struct get_underlying_char_type<std::wstring> { typedef wchar_t type; };
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