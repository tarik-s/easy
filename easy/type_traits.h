/*!
 * \file   easy/type_traits.h
 * \author Sergey Tararay
 * \date   04.08.2013
 *
 * FILE DESCRIPTION
 */
#ifndef EASY_TYPE_TRAITS_H
#define EASY_TYPE_TRAITS_H

#include <easy/config.h>
#include <type_traits>

namespace easy
{
#ifdef EASY_HAS_UNDERLYING_TYPE

  template<class T>
  struct underlying_type
    : std::underlying_type<T> {
  };

#else

  template<class T>
  struct underlying_type {
    typedef int type;
  };

#endif
}

#endif