/*!
 *  @file   easy/strings/conv.h
 *  @author Sergey Tararay
 *  @date   2013
 *
 */
#ifndef EASY_STRINGS_CONV_H
#define EASY_STRINGS_CONV_H

#include <easy/config.h>
#include <easy/strings/lite_string.h>
#include <easy/error_handling.h>

namespace easy
{
#ifdef EASY_HAS_WCAHR
  std::wstring utf8_to_utf16(const lite_string& s, error_code_ref ec = nullptr);

  std::string utf16_to_utf8(const lite_wstring& s, error_code_ref ec = nullptr);
#endif
}

#endif