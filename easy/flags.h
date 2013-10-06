/*!
 *  @file   easy/flags.h
 *  @author Sergey Tararay
 *  @date   2013
 */
#ifndef EASY_FLAGS_H_INCLUDED
#define EASY_FLAGS_H_INCLUDED

#include <easy/config.h>
#include <easy/type_traits.h>
#include <easy/stlex/nullptr_t.h>

#include <boost/type_traits.hpp>
#include <boost/optional.hpp>


template<class T>
easy::nullptr_t declare_as_flags(T);

#define EASY_DECLARE_AS_FLAGS(enum_type) \
  inline void declare_as_flags(enum_type) { }

namespace easy
{
  template<class T>
  struct is_flag 
    : boost::mpl::not_<
      boost::is_same<
          decltype(declare_as_flags(T()))
        , nullptr_t
      >
    > {
  };

  //! enum_group
  template<class E1, class E2 = void, class E3 = void, class E4 = void, class E5 = void>
  class enum_group
    : public enum_group<E2, E3, E4, E5>
  {
    EASY_STATIC_ASSERT(std::is_enum<E1>::value, "Only enumerations can be paramerers of enum_group");
  public:
    typedef enum_group<E2, E3, E4, E5> base;

    using base::get;
    using base::set;
    using base::contains;

    enum_group() { }
    enum_group(nullptr_t) { }

    template<class A>
    enum_group(A a, typename boost::enable_if<std::is_enum<A>>::type* = nullptr) {
      set(a);
    }

    template<class A1, class A2>
    enum_group(A1 a1, A2 a2) {
      set(a1);
      set(a2);
    }

    template<class A1, class A2, class A3>
    enum_group(A1 a1, A2 a2, A3 a3) {
      set(a1);
      set(a2);
      set(a3);
    }

    template<class A1, class A2, class A3, class A4>
    enum_group(A1 a1, A2 a2, A3 a3, A4 a4) {
      set(a1);
      set(a2);
      set(a3);
      set(a4);
    }

    template<class A1, class A2, class A3, class A4, class A5>
    enum_group(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) {
      set(a1);
      set(a2);
      set(a3);
      set(a4);
      set(a5);
    }

    void set(E1 v) {
      m_value = v;
    }

    E1 get(E1 def) const {
      return m_value.get_value_or(def);
    }

    bool contains(E1 v) const {
      return m_value && m_value.get() == v;
    }

  private:
    boost::optional<E1> m_value;
  };

  template<>
  class enum_group<void, void, void, void, void>
  {
  public:
    template<class Unknown>
    void set(Unknown) { EASY_STATIC_ASSERT(0, "Unknown enum group mememer"); }
    void get() const;
    void contains() const;
  };

  template<class E1, class E2, class E3, class E4, class E5, class T>
  auto operator << (enum_group<E1, E2, E3, E4, E5>& eg, const T& v) -> decltype(eg)&
  {
    eg.set(v);
    return eg;
  }


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
