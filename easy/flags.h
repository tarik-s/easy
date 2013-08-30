/*!
 * \file   easy/flags.h
 * \author Sergey Tararay
 * \date   04.08.2013
 *
 * FILE DESCRIPTION
 */
#ifndef EASY_FLAGS_H_INCLUDED
#define EASY_FLAGS_H_INCLUDED

#include <easy/config.h>
#include <easy/detail/flags_detail.h>

#include <easy/stlex/type_traits.h>
#include <boost/type_traits.hpp>


template<class T>
easy::flags_detail::not_flags declare_as_flags(T);

#define EASY_DECLARE_AS_FLAGS(enum_type) \
  inline void declare_as_flags(enum_type) { }

namespace easy
{
  template<class T>
  struct is_flag 
    : boost::mpl::not_<
      boost::is_same<
          decltype(declare_as_flags(T()))
        , easy::flags_detail::not_flags
      >
    > {
  };
}

//////////////////////////////////////////////////////////////////////////

template<class T>
typename boost::enable_if<easy::is_flag<T>, T
>::type operator ~ (T flag) {
  return static_cast<T>(~static_cast<typename easy::underlying_type<T>::type>(flag));
}

//////////////////////////////////////////////////////////////////////////

template<class T>
typename boost::enable_if<easy::is_flag<T>, T
>::type& operator &= (T& flag1, T flag2) {
  return flag1 = (flag1 & flag2);
}

template<class T>
typename boost::enable_if<easy::is_flag<T>, T
>::type operator & (T flag1, T flag2) {
  return static_cast<T>(
    static_cast<typename easy::underlying_type<T>::type>(flag1) &
    static_cast<typename easy::underlying_type<T>::type>(flag2)
  );
}

//////////////////////////////////////////////////////////////////////////

template<class T>
typename boost::enable_if<easy::is_flag<T>, T
>::type& operator |= (T& flag1, T flag2) {
  return flag1 = (flag1 | flag2);
}

template<class T>
typename boost::enable_if<easy::is_flag<T>, T
>::type operator | (T flag1, T flag2) {
  return static_cast<T>(
    static_cast<typename easy::underlying_type<T>::type>(flag1) |
    static_cast<typename easy::underlying_type<T>::type>(flag2)
    );
}

//////////////////////////////////////////////////////////////////////////

template<class T>
typename boost::enable_if<easy::is_flag<T>, T
>::type& operator ^= (T& flag1, T flag2) {
  return flag1 = (flag1 ^ flag2);
}

template<class T>
typename boost::enable_if<easy::is_flag<T>, T
>::type operator ^ (T flag1, T flag2) {
  return static_cast<T>(
    static_cast<typename easy::underlying_type<T>::type>(flag1) ^
    static_cast<typename easy::underlying_type<T>::type>(flag2)
    );
}


#endif
