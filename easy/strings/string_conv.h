/*!
 * \file   easy/strings/string_conv.h
 * \author Sergey Tararay
 * \date   04.08.2013
 *
 * FILE DESCRIPTION
 */
#ifndef EASY_STRINGS_STRING_CONV_H
#define EASY_STRINGS_STRING_CONV_H

#include <easy/config.h>
#include <easy/strings/c_string.h>
#include <easy/error_code_ref.h>

namespace easy
{
#ifdef EASY_HAS_WCAHR
  utf16_string utf8_to_utf16(const c_utf8_string& s, error_code_ref ec = nullptr);

  utf8_string utf16_to_utf8(const c_utf16_string& s, error_code_ref ec = nullptr);
#endif
}

#endif