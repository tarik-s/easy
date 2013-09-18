/*!
 *  @file   easy/stlex/type_traits.h
 *  @author Sergey Tararay
 *  @date   2013
 *
 */
#ifndef EASY_STLEX_TYPE_TRAITS_H_INCLUDED
#define EASY_STLEX_TYPE_TRAITS_H_INCLUDED

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