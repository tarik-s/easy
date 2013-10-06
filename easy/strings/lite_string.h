/*!
 *  @file   easy/strings/lite_string.h
 *  @author Sergey Tararay
 *  @date   2013
 *
 */
#ifndef EASY_STRINGS_C_STRING_H_INCLUDED
#define EASY_STRINGS_C_STRING_H_INCLUDED

#include <easy/config.h>

#include <easy/stlex/nullptr_t.h>
#include <easy/safe_bool.h>
#include <easy/type_traits.h>

#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/remove_cv.hpp>
#include <boost/mpl/and.hpp>
#include <boost/noncopyable.hpp>

#include <memory>
#include <string>

namespace easy
{
  template<class TChar> class basic_lite_string;
}

template<class TChar>
easy::basic_lite_string<TChar> make_c_string(const TChar* pstr) {
  return easy::basic_lite_string<TChar>(pstr, std::char_traits<TChar>::length(pstr));
}

namespace std
{
  template<class TChar>
  easy::basic_lite_string<TChar> make_c_string(const basic_string<TChar>& str) {
    return easy::basic_lite_string<TChar>(str.c_str(), str.size());
  }
}

namespace easy
{
  template<class TChar>
  class basic_lite_string
    : public safe_bool<basic_lite_string<TChar>>
    , boost::noncopyable
  {
  public:
    typedef basic_lite_string   this_type;
    typedef TChar            char_type;
    typedef char_type        value_type;
    typedef size_t           size_type;          
    typedef const char_type& const_reference;
    typedef const char_type* iterator;
    typedef const char_type* const_iterator;

    typedef std::basic_string<char_type> string_type;
  public:
    
    basic_lite_string() EASY_NOEXCEPT { }

    basic_lite_string(nullptr_t) EASY_NOEXCEPT { }

    basic_lite_string(basic_lite_string && r) EASY_NOEXCEPT {
      *this = std::move(r);
    }

    basic_lite_string(string_type && str)
      : m_holder_ptr(new string_type(std::move(str)))
      , m_str(m_holder_ptr->c_str(), m_holder_ptr->size()) {
    }

    basic_lite_string(const char_type* pstr, size_t size) EASY_NOEXCEPT
      : m_str(size > 0 ? pstr : nullptr, size) {
    }

    basic_lite_string(const_iterator begin, const_iterator end) EASY_NOEXCEPT
      : m_str(begin, end) {
    }

    template<class TStr>
    basic_lite_string(const TStr& str, 
      typename boost::enable_if<
        boost::is_same<
          typename underlying_char_type<
            typename boost::remove_cv<TStr>::type
          >::type,
          char_type
        >        
      >::type* = nullptr) 
    {
      *this = this_type(make_c_string(str));
    }

    const_iterator begin() const EASY_NOEXCEPT {
      return c_str();
    }

    const_iterator end() const EASY_NOEXCEPT {
      return c_str() + size();
    }

    const_iterator cbegin() const EASY_NOEXCEPT {
      return begin();
    }

    const_iterator cend() const EASY_NOEXCEPT {
      return end();
    }

    bool empty() const EASY_NOEXCEPT {
      return !data() || size() == 0;
    }

    size_t size() const EASY_NOEXCEPT {
      return m_str.size;
    }

    const char_type* c_str() const EASY_NOEXCEPT {
      return m_str.pstr;
    }

    size_t length() const EASY_NOEXCEPT {
      return size();
    }

    const char_type* data() const EASY_NOEXCEPT {
      return c_str();
    }

    const_reference operator[] (size_t index) const {
      return c_str()[index];
    }

    const_reference at(size_t index) const {
      return c_str()[index];
    }

    operator string_type() const {
      if (empty())
        return string_type();
      else if (m_holder_ptr)
        return *m_holder_ptr;
      else
        return string_type(c_str(), length());
    }

    bool operator !() const {
      return empty();
    }

    int compare(const this_type& r) const
    {
      if (!*this && !r)
        return 0;

      if (length() != r.length())
        return length() > r.length() ? 1 : -1;

      return std::char_traits<char_type>::compare(cbegin(), r.cend(), length());
    }

  private:
    basic_lite_string& operator = (basic_lite_string && r) EASY_NOEXCEPT {
      m_str = r.m_str;
      m_holder_ptr = std::move(r.m_holder_ptr);
      return *this;
    }

  private:
    struct sized_str {
      sized_str(const char_type* pstr = nullptr, size_t size = 0) 
        : pstr(pstr), size(size) {
      }
      sized_str(const char_type* begin, const char_type* end)
        : pstr(), size()
      {
        EASY_ASSERT(begin <= end);
        if (begin < end) {
          pstr = begin;
          size = end - begin;
        }
      }
      char_type const* pstr;
      size_type size;
    };
  private:
    std::unique_ptr<string_type> m_holder_ptr; // Must be at the first position
    sized_str m_str;
  };


  //!
  template<class Char, class Traits>
  std::basic_ostream<Char, Traits>& operator << (std::basic_ostream<Char, Traits>& os, const basic_lite_string<Char>& s)
  {
    if (s)
      os.write(s.data(), s.length());
    return os;
  }

  //////////////////////////////////////////////////////////////////////////

  //!
  template<class Char>
  bool operator == (const basic_lite_string<Char>& s1, const basic_lite_string<Char>& s2)
  {
    return s1.compare(s2) == 0;
  }

  template<class Char>
  bool operator < (const basic_lite_string<Char>& s1, const basic_lite_string<Char>& s2)
  {
    return s1.compare(s2) != 0;
  }

  template<class Char>
  bool operator <= (const basic_lite_string<Char>& s1, const basic_lite_string<Char>& s2)
  {
    return s1.compare(s2) <= 0;
  }

  template<class Char>
  bool operator != (const basic_lite_string<Char>& s1, const basic_lite_string<Char>& s2)
  {
    return s1.compare(s2) != 0;
  }

  template<class Char>
  bool operator > (const basic_lite_string<Char>& s1, const basic_lite_string<Char>& s2)
  {
    return s1.compare(s2) > 0;
  }

  template<class Char>
  bool operator >= (const basic_lite_string<Char>& s1, const basic_lite_string<Char>& s2)
  {
    return s1.compare(s2) >= 0;
  }

  //////////////////////////////////////////////////////////////////////////

  template<class Char, class Value>
  typename boost::enable_if<
    is_convertible_but_not_same<Value, basic_lite_string<Char>>, 
    bool
  >::type operator == (const basic_lite_string<Char>& s1, const Value& s2)
  {
    return s1.compare(s2) == 0;
  }

  template<class Char, class Value>
  typename boost::enable_if<
    is_convertible_but_not_same<Value, basic_lite_string<Char>>, 
    bool
    >::type operator == (const Value& s1, const basic_lite_string<Char>& s2)
  {
    return s2 == s1;
  }

  template<class Char, class Value>
  typename boost::enable_if<
    is_convertible_but_not_same<Value, basic_lite_string<Char>>, 
    bool
    >::type operator != (const basic_lite_string<Char>& s1, const Value& s2)
  {
    return s1.compare(s2) != 0;
  }

  template<class Char, class Value>
  typename boost::enable_if<
    is_convertible_but_not_same<Value, basic_lite_string<Char>>, 
    bool
    >::type operator != (const Value& s1, const basic_lite_string<Char>& s2)
  {
    return s2 != s1;
  }

  template<class Char, class Value>
  typename boost::enable_if<
    is_convertible_but_not_same<Value, basic_lite_string<Char>>, 
    bool
    >::type operator < (const basic_lite_string<Char>& s1, const Value& s2)
  {
    return s1.compare(s2) < 0;
  }

  template<class Char, class Value>
  typename boost::enable_if<
    is_convertible_but_not_same<Value, basic_lite_string<Char>>, 
    bool
    >::type operator < (const Value& s1, const basic_lite_string<Char>& s2)
  {
    return s2 > s1;
  }

  template<class Char, class Value>
  typename boost::enable_if<
    is_convertible_but_not_same<Value, basic_lite_string<Char>>, 
    bool
    >::type operator <= (const basic_lite_string<Char>& s1, const Value& s2)
  {
    return s1.compare(s2) <= 0;
  }

  template<class Char, class Value>
  typename boost::enable_if<
    is_convertible_but_not_same<Value, basic_lite_string<Char>>, 
    bool
    >::type operator <= (const Value& s1, const basic_lite_string<Char>& s2)
  {
    return s2 >= s1;
  }

  template<class Char, class Value>
  typename boost::enable_if<
    is_convertible_but_not_same<Value, basic_lite_string<Char>>, 
    bool
    >::type operator > (const basic_lite_string<Char>& s1, const Value& s2)
  {
    return s1.compare(s2) > 0;
  }

  template<class Char, class Value>
  typename boost::enable_if<
    is_convertible_but_not_same<Value, basic_lite_string<Char>>, 
    bool
    >::type operator > (const Value& s1, const basic_lite_string<Char>& s2)
  {
    return s2 < s1;
  }

  template<class Char, class Value>
  typename boost::enable_if<
    is_convertible_but_not_same<Value, basic_lite_string<Char>>, 
    bool
    >::type operator >= (const basic_lite_string<Char>& s1, const Value& s2)
  {
    return s1.compare(s2) >= 0;
  }

  template<class Char, class Value>
  typename boost::enable_if<
    is_convertible_but_not_same<Value, basic_lite_string<Char>>, 
    bool
    >::type operator >= (const Value& s1, const basic_lite_string<Char>& s2)
  {
    return s2 <= s1;
  }

  //////////////////////////////////////////////////////////////////////////




  typedef basic_lite_string<char>    lite_string;

#ifdef EASY_HAS_WCAHR
  typedef basic_lite_string<wchar_t> lite_wstring;
#endif
  
}

//////////////////////////////////////////////////////////////////////////

template<class Value, class Char>
typename boost::enable_if<
  easy::is_convertible_but_not_same<Value, easy::basic_lite_string<Char>>,
  bool
  >::type operator == (const Value& s1, const Char* s2)
{
  return easy::basic_lite_string<Char>(s1).compare(s2) == 0;
}

template<class Value, class Char>
typename boost::enable_if<
  easy::is_convertible_but_not_same<Value, easy::basic_lite_string<Char>>,
  bool
  >::type operator == (const Char* s1, const Value& s2)
{
  return easy::basic_lite_string<Char>(s1).compare(s2) == 0;
}

template<class Value, class Char>
typename boost::enable_if<
  easy::is_convertible_but_not_same<Value, easy::basic_lite_string<Char>>,
  bool
  >::type operator != (const Value& s1, const Char* s2)
{
  return easy::basic_lite_string<Char>(s1).compare(s2) != 0;
}

template<class Value, class Char>
typename boost::enable_if<
  easy::is_convertible_but_not_same<Value, easy::basic_lite_string<Char>>,
  bool
  >::type operator != (const Char* s1, const Value& s2)
{
  return easy::basic_lite_string<Char>(s1).compare(s2) != 0;
}

template<class Value, class Char>
typename boost::enable_if<
  easy::is_convertible_but_not_same<Value, easy::basic_lite_string<Char>>,
  bool
  >::type operator < (const Value& s1, const Char* s2)
{
  return easy::basic_lite_string<Char>(s1).compare(s2) < 0;
}

template<class Value, class Char>
typename boost::enable_if<
  easy::is_convertible_but_not_same<Value, easy::basic_lite_string<Char>>,
  bool
  >::type operator < (const Char* s1, const Value& s2)
{
  return easy::basic_lite_string<Char>(s1).compare(s2) < 0;
}

template<class Value, class Char>
typename boost::enable_if<
  easy::is_convertible_but_not_same<Value, easy::basic_lite_string<Char>>,
  bool
  >::type operator <= (const Value& s1, const Char* s2)
{
  return easy::basic_lite_string<Char>(s1).compare(s2) <= 0;
}

template<class Value, class Char>
typename boost::enable_if<
  easy::is_convertible_but_not_same<Value, easy::basic_lite_string<Char>>,
  bool
  >::type operator <= (const Char* s1, const Value& s2)
{
  return easy::basic_lite_string<Char>(s1).compare(s2) <= 0;
}

template<class Value, class Char>
typename boost::enable_if<
  easy::is_convertible_but_not_same<Value, easy::basic_lite_string<Char>>,
  bool
  >::type operator > (const Value& s1, const Char* s2)
{
  return easy::basic_lite_string<Char>(s1).compare(s2) > 0;
}

template<class Value, class Char>
typename boost::enable_if<
  easy::is_convertible_but_not_same<Value, easy::basic_lite_string<Char>>,
  bool
  >::type operator > (const Char* s1, const Value& s2)
{
  return easy::basic_lite_string<Char>(s1).compare(s2) > 0;
}

template<class Value, class Char>
typename boost::enable_if<
  easy::is_convertible_but_not_same<Value, easy::basic_lite_string<Char>>,
  bool
  >::type operator >= (const Value& s1, const Char* s2)
{
  return easy::basic_lite_string<Char>(s1).compare(s2) >= 0;
}

template<class Value, class Char>
typename boost::enable_if<
  easy::is_convertible_but_not_same<Value, easy::basic_lite_string<Char>>,
  bool
  >::type operator >= (const Char* s1, const Value& s2)
{
  return easy::basic_lite_string<Char>(s1).compare(s2) >= 0;
}

#endif
