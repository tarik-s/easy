/*!
 * \file   easy/stlex/make_unique.h
 * \author Sergey Tararay
 * \date   24.08.2013
 *
 * FILE DESCRIPTION
 */
#ifndef EASY_STLEX_MAKE_UNIQUE_INCLUDED_H
#define EASY_STLEX_MAKE_UNIQUE_INCLUDED_H

#include <easy/config.h>

#ifdef EASY_HAS_MAKE_UNIQUE 

// will be implemented in C++14

#else // EASY_HAS_MAKE_UNIQUE 

#include <type_traits>
#include <memory>

namespace std
{
  template<class T>
  unique_ptr<T> make_unique() {
    return unique_ptr<T>(new T);
  }

  template<class T, class A1>
  unique_ptr<T> make_unique(A1 && a1) {
    return unique_ptr<T>(new T(
      forward<A1>(a1)
    ));
  }

  template<class T, class A1, class A2>
  unique_ptr<T> make_unique(A1 && a1, A2 && a2) {
    return unique_ptr<T>(new T(
      forward<A1>(a1), 
      forward<A2>(a2)
    ));
  }

  template<class T, class A1, class A2, class A3>
  unique_ptr<T> make_unique(A1 && a1, A2 && a2, A3 && a3) {
    return unique_ptr<T>(new T(
      forward<A1>(a1), 
      forward<A2>(a2),
      forward<A3>(a3)
    ));
  }
}

#endif // EASY_HAS_MAKE_UNIQUE 


#endif