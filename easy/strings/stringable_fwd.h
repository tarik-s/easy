#ifndef EASY_STRINGABLE_FWD_H_INCLUDED
#define EASY_STRINGABLE_FWD_H_INCLUDED

#include <easy/config.h>

#include <boost/type_traits/is_base_of.hpp>
#include <boost/type_traits/remove_cv.hpp>

namespace easy
{
  struct stringable_tag { };

  template<class T>
  struct is_stringable
    : boost::is_base_of<stringable_tag, T> {
  };
  
}

#endif