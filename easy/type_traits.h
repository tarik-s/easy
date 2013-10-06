/*!
 *  @file   easy/types.h
 *  @author Sergey Tararay
 *  @date   2013
 */
#ifndef EASY_TYPE_TRAITS_H_INCLUDED
#define EASY_TYPE_TRAITS_H_INCLUDED

#include <easy/config.h>

#include <type_traits>
#include <boost/type_traits.hpp>

#include <string>

namespace easy
{
  //! underlying_type
  template<class T>
  struct underlying_type
#ifdef EASY_HAS_UNDERLYING_TYPE
    : std::underlying_type<T> 
#else
    : std::enable_if<true, int>
#endif
  {
  };
  
  //! underlying_char_type
  template<class T>
  struct underlying_char_type {
    //typedef void type;
  };

  template<> struct underlying_char_type<char*> { typedef char type; };
  template<> struct underlying_char_type<const char*> { typedef char type; };
  template<> struct underlying_char_type<volatile char*> { typedef char type; };
  template<> struct underlying_char_type<const volatile char*> { typedef char type; };
  template<> struct underlying_char_type<char[]> { typedef char type; };
  template<size_t TSize> struct underlying_char_type<char[TSize]> { typedef char type; };

  template<> struct underlying_char_type<std::string> { typedef char type; };

#ifdef EASY_HAS_WCAHR
  template<> struct underlying_char_type<wchar_t*> { typedef wchar_t type; };
  template<> struct underlying_char_type<const wchar_t*> { typedef wchar_t type; };
  template<> struct underlying_char_type<volatile wchar_t*> { typedef wchar_t type; };
  template<> struct underlying_char_type<const volatile wchar_t*> { typedef wchar_t type; };
  template<> struct underlying_char_type<wchar_t[]> { typedef wchar_t type; };
  template<size_t TSize> struct underlying_char_type<wchar_t[TSize]> { typedef wchar_t type; };

  template<>
  struct underlying_char_type<std::wstring> { typedef wchar_t type; };
#endif

  template<class From, class To>
  struct is_convertible_but_not_same
    : boost::mpl::and_<
        boost::is_convertible<From, To>,
        boost::mpl::not_<boost::is_same<From, To>>
    >
  {
    
  };

}


#endif