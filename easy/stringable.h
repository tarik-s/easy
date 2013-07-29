#ifndef EASY_STRINGABLE_H_INCLUDED
#define EASY_STRINGABLE_H_INCLUDED

#include <easy/config.h>
#include <easy/stringable_fwd.h>

namespace easy
{

  template<class T>
  struct is_stringable 
    : boost::is_base_of<string_tag, T> {
  };




  
}

#endif