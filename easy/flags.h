#ifndef EASY_FLAGS_H_INCLUDED
#define EASY_FLAGS_H_INCLUDED

#include <easy/config.h>

#include <type_traits>

#include <boost/type_traits.hpp>


namespace easy
{

  template<class TFlag>
  class flags
  {
    EASY_STATIC_ASSERT(boost::is_enum<TFlag>::value, "Only enumerations can be considered as flags");
  public:
    // typedefs

    typedef TFlag flag_type;
    typedef typename std::underlying_type<flag_type>::type underlying_type;

    // constructors

    flags() 
      : m_val() {
    }

    flags(flag_type f) {
      *this = f;
    }

    // assignment

    flags& operator = (flag_type f) {
      m_val = static_cast<underlying_type>(f);
      return *this;
    }

    bool operator !() const {
      return !m_val;
    }

  private:
    underlying_type m_val;
  };

}

#endif