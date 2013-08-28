/*!
 * \file   easy/strings/stringable_fwd.h
 * \author Sergey Tararay
 * \date   04.08.2013
 *
 * FILE DESCRIPTION
 */
#ifndef EASY_STRINGABLE_FWD_H_INCLUDED
#define EASY_STRINGABLE_FWD_H_INCLUDED

#include <easy/config.h>

namespace easy
{
  template<class T>
  struct get_underlying_char_type {
    typedef void type;
  };
}

#endif