#ifndef EASY_SAFE_BOOL_H_INCLUDED
#define EASY_SAFE_BOOL_H_INCLUDED

#include <easy/config.h>

namespace easy
{
  template <typename Derived> 
  class safe_bool
  {
    void true_type() { }
  public:
    typedef void (safe_bool<Derived>::*explicit_bool) ();

    operator explicit_bool() const {
      return static_cast<const Derived*>(this)->operator!() ? nullptr : explicit_true();
    }
  protected:
    ~safe_bool() {}
    
    static explicit_bool explicit_true() {
      return &true_type;
    }

    static explicit_bool explicit_false() {
      return nullptr;
    }
  };
  
  template <typename T> 
  bool operator==(const safe_bool<T>& lhs, bool b) 
  {
    return !!lhs == b;
  }

  template <typename T> 
  bool operator==(bool b, const safe_bool<T>& rhs) 
  {
    return b == !!rhs;    
  }

  template <typename T> 
  bool operator != (bool b, const safe_bool<T>& rhs) 
  {
    return !(b == rhs);
  }

  template <typename T> 
  bool operator!=(const safe_bool<T>& lhs, bool b) 
  {
    return !(lhs == b);
  }

  template <typename T, typename U> 
  bool operator==(const safe_bool<T>& lhs,const safe_bool<U>& rhs) {
    EASY_STATIC_ASSERT(0, "This classes do not support comparison");
    return false;
  }

  template <typename T,typename U> 
  bool operator!=(const safe_bool<T>& lhs,const safe_bool<U>& rhs) {
    return !(lhs == rhs);
  }
}

#endif