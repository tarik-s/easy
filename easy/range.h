#ifndef EASY_RANGE_H_INCLUDED
#define EASY_RANGE_H_INCLUDED

#include <easy/config.h>

namespace easy
{
  template<class T>
  bool is_in_range(T val, T begin, T end) {
    return val >= begin && val < end;
  }
}

#endif