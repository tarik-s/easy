/*!
 *  @file   easy/safe_bool.h
 *  @author Sergey Tararay
 *  @date   2013
 */
#ifndef EASY_SAFE_BOOL_H_INCLUDED
#define EASY_SAFE_BOOL_H_INCLUDED

#include <easy/config.h>

#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/mpl/not.hpp>

namespace easy
{

  /*!
   * @class safe_bool
   * @brief workaround to eliminate pitfalls of operator bool
   */

  template <typename Derived> 
  class safe_bool
  {
    void true_type() { }
  public:
    /// explicit_bool 
    typedef void (safe_bool<Derived>::*explicit_bool) ();

    /// default operator explicit_bool 
    operator explicit_bool() const {
      return static_cast<const Derived*>(this)->operator!() ? nullptr : explicit_true();
    }
  protected:
    safe_bool() {
      EASY_STATIC_ASSERT((boost::mpl::not_<boost::is_convertible<Derived, int>>::value), 
        "Derived class cannot be convertible to any type converible to int");
    }
    ~safe_bool() {}
    
    static explicit_bool explicit_true() {
      return &true_type;
    }

    static explicit_bool explicit_false() {
      return nullptr;
    }
  };

  /**
   *  Comparison with bool
   */
  
  template <class T, class Bool>
  typename boost::enable_if<
    boost::is_same<Bool, bool>,
    bool
  >::type operator == (const safe_bool<T>& lhs, bool b) 
  {
    return !!lhs == b;
  }

  template <class T, class Bool>
  typename boost::enable_if<
    boost::is_same<Bool, bool>,
    bool
  >::type operator!=(const safe_bool<T>& lhs, Bool b) 
  {
    return !(lhs == b);
  }

  template <class Bool, class T> 
  typename boost::enable_if<
    boost::is_same<Bool, bool>, 
    bool
  >::type operator==(Bool b, const safe_bool<T>& rhs) 
  {
    return rhs == b; 
  }

  template <class Bool, class T> 
  typename boost::enable_if<
    boost::is_same<Bool, bool>, 
    bool
  >::type operator != (Bool b, const safe_bool<T>& rhs)
  {
    return !(b == rhs);
  }

  /**
   *  Comparison with safe_bool is disabled
   */

  template <class T, class U> 
  bool operator == (const safe_bool<T>& lhs, const safe_bool<U>& rhs) {
    EASY_STATIC_ASSERT(0, "safe_bool is not comparable to other safe_bool");
    return false;
  }

  template <class T, class U> 
  bool operator != (const safe_bool<T>& lhs, const safe_bool<U>& rhs) {
    return !(lhs == rhs);
  }
}

#endif