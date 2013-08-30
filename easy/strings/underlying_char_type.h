/*!
 * \file   easy/strings/stringable_fwd.h
 * \author Sergey Tararay
 * \date   04.08.2013
 *
 * FILE DESCRIPTION
 */
#ifndef EASY_STRINGS_UNDERLYING_CHAR_TYPE_H_INCLUDED
#define EASY_STRINGS_UNDERLYING_CHAR_TYPE_H_INCLUDED

#include <easy/config.h>

#include <string>

namespace easy
{
  template<class T>
  struct get_underlying_char_type {
    //typedef void type;
  };

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

#endif
