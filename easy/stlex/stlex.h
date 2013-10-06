/*!
 *  @file   easy/stlex/stlex.h
 *  @author Sergey Tararay
 *  @date   2013
 *
 */
#ifndef EASY_STLEX_STLEX_H_INCLUDED
#define EASY_STLEX_STLEX_H_INCLUDED

#include <easy/stlex/nullptr_t.h>
#include <easy/stlex/make_unique.h>

#include <vector>

namespace std
{
  template<class T, class U>
  vector<T>& operator << (vector<T>& vect, const U& v) {
    vect.push_back(v);
    return vect;
  }
}

#endif