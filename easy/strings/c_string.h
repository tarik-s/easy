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
    
    basic_c_string() EASY_NOEXCEPT
      : m_pstr(nullptr)
      , m_size(0) {
    }

    basic_c_string(nullptr_t) EASY_NOEXCEPT
      : m_pstr(nullptr)
      , m_size(0) {
    }

    basic_c_string(basic_c_string && r) EASY_NOEXCEPT
      : m_pstr(nullptr)
      , m_size(0)
    {
      *this = r;
    }

    basic_c_string(const TChar* pstr, size_t size)
      : m_pstr(size > 0 ? pstr : nullptr)
      , m_size(size) {
    }

    template<class TStr>
    basic_c_string(const TStr& str) {
      EASY_STATIC_ASSERT(
        is_stringable<typename boost::remove_cv<TStr>::type>::value,
        "The type specified is not a stringable type"
      );
      *this = this_type(make_c_string<char_type>(str));
    }

    const_iterator begin() const {
      return m_pstr;
    }

    const_iterator end() const {
      return m_pstr + m_size;
    }

    const_iterator cbegin() const {
      begin();
    }

    const_iterator cend() const {
      return end();
    }

    bool empty() const EASY_NOEXCEPT {
      return !m_pstr || m_size == 0;
    }

    size_t size() const EASY_NOEXCEPT {
      return m_size;
    }

    size_t length() const EASY_NOEXCEPT {
      return m_size;
    }

    const char_type* c_str() const EASY_NOEXCEPT {
      return m_pstr;
    }

    const char_type* data() const EASY_NOEXCEPT {
      return m_pstr;
    }

    const_reference operator[] (size_t index) const {
      return m_pstr[index];
    }

    const_reference at(size_t index) const {
      return m_pstr[index];
    }

  private:
    basic_c_string& operator = (basic_c_string && r) EASY_NOEXCEPT {
      m_pstr = r.m_pstr;
      m_size = r.m_size;
      m_holder_ptr.swap(r.m_holder_ptr);
      return *this;
    }

    basic_c_string(string_type && str)
      : m_holder_ptr(new string_type(std::move(str)))
      , m_pstr(m_holder_ptr->c_str())
      , m_size(m_holder_ptr->size()) {
    }
  private:
    std::unique_ptr<string_type> m_holder_ptr; // Must be at the first position
    TChar const* m_pstr;
    size_t m_size;    
  }; 


  typedef basic_c_string<char> c_string;
#ifdef EASY_HAVE_WCAHR
  typedef basic_c_string<wchar_t> c_wstring;
#endif
  
}

#include <easy/strings/detail/stringable_stock.inl>

#endif