#ifndef EASY_C_STRING_H_INCLUDED
#define EASY_C_STRING_H_INCLUDED

#include <easy/config.h>
#include <easy/strings/stringable_fwd.h>

#include <memory>
#include <string>

namespace easy
{
  template<class TChar>
  class basic_c_string
  {
    basic_c_string(const basic_c_string&);
    basic_c_string& operator = (const basic_c_string&);

  public:
    typedef basic_c_string   this_type;
    typedef TChar            char_type;
    typedef char_type        value_type;
    typedef size_t           size_type;          
    typedef const char_type& const_reference;    
    typedef const char_type* const_iterator;

    typedef std::basic_string<char_type> string_type;
  public:
    
    basic_c_string() EASY_NOEXCEPT { }

    basic_c_string(nullptr_t) EASY_NOEXCEPT { }

    basic_c_string(basic_c_string && r) EASY_NOEXCEPT {
      *this = r;
    }

    basic_c_string(const char_type* pstr, size_t size)
      : m_str(size > 0 ? pstr : nullptr, size) {
    }

    template<class TStr>
    basic_c_string(const TStr& str) {
      EASY_STATIC_ASSERT(
        is_stringable<typename boost::remove_cv<TStr>::type>::value,
        "The 'TStr' is not a stringable type"
      );
      *this = this_type(make_c_string<char_type>(str));
    }

    const_iterator begin() const EASY_NOEXCEPT {
      return c_str();
    }

    const_iterator end() const EASY_NOEXCEPT {
      return c_str() + size();
    }

    const_iterator cbegin() const EASY_NOEXCEPT {
      begin();
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

  private:
    basic_c_string& operator = (basic_c_string && r) EASY_NOEXCEPT {
      m_str = r.m_str;
      m_holder_ptr.swap(r.m_holder_ptr);
      return *this;
    }

    basic_c_string(string_type && str)
      : m_holder_ptr(new string_type(std::move(str)))
      , m_str(m_holder_ptr->c_str(), m_holder_ptr->size()) {
    }
  private:
    struct sized_str {
      sized_str(const char_type* pstr = nullptr, size_t size = 0) 
        : pstr(pstr), size(size) {
      }
      char_type const* pstr;
      size_type size;
    };
  private:
    std::unique_ptr<string_type> m_holder_ptr; // Must be at the first position
    sized_str m_str;
  }; 


  typedef basic_c_string<char> c_string;
#ifdef EASY_HAVE_WCAHR
  typedef basic_c_string<wchar_t> c_wstring;
#endif
  
}

#include <easy/strings/detail/stringable_stock.inl>

#endif