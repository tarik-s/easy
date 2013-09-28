/*!
 *  @file   easy/strings.h
 *  @author Sergey Tararay
 *  @date   2013
 */
#ifndef EASY_STRINGS_H_INCLUDED
#define EASY_STRINGS_H_INCLUDED


#include <easy/strings/underlying_char_type.h>
#include <easy/strings/lite_string.h>
#include <easy/strings/string_conv.h>

namespace easy
{
  template<class Char>
  std::basic_string<Char>& trim_terminating_zeros(std::basic_string<Char>& s) 
  {
    while (!s.empty() && !s.back())
      s.pop_back();
    return s;
  }
}

#endif
