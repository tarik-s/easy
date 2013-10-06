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

  //! Workaround to eliminate pitfalls of operator bool
  template <typename Derived> 
  class safe_bool
  {
    void true_type() { }
  public:
    //! explicit_bool type
    typedef void (safe_bool<Derived>::*explicit_bool) ();

    //! operator explicit_bool 
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

  /*!
   * @{
   * @brief Compares between safe_bool<T> and bool
   */
  template <class T, class Bool>
  typename boost::enable_if<
    boost::is_same<Bool, bool>,
    bool
  >::type operator == (const safe_bool<T>& lhs, Bool b) 
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
  /*! @} */


  /*!
   * @{
   * @brief Disables comparison between safe_bool<T> and safe_bool<U>
   */
  template <class T, class U> 
  bool operator == (const safe_bool<T>& lhs, const safe_bool<U>& rhs) 
  {
    EASY_STATIC_ASSERT(0, "safe_bool is not comparable to other safe_bool");
    return false;
  }

  template <class T, class U> 
  bool operator != (const safe_bool<T>& lhs, const safe_bool<U>& rhs) 
  {
    return !(lhs == rhs);
  }

  /*! @} */

  template<class Stream, class T>
  Stream& operator << (Stream& s, const safe_bool<T>& b)
  {
    EASY_STATIC_ASSERT(0, "safe_bool cannot be written to any stream");
    return s;
  }
}

#endif