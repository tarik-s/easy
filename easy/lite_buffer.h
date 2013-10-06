#ifndef EASY_LITE_BUFFER_H_INCLUDED
#define EASY_LITE_BUFFER_H_INCLUDED

#include <easy/config.h>

#include <easy/stlex/nullptr_t.h>
#include <easy/safe_bool.h>

#include <boost/noncopyable.hpp>

#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_integral.hpp>

namespace easy
{

  template<class T>
  class lite_buffer
    : public safe_bool<lite_buffer<T>>
    , public boost::noncopyable
  {
    EASY_STATIC_ASSERT(std::is_integral<T>::value, "lite_buffer can be built only over an integral value sequence");
  public:

    typedef T      value_type;
    typedef size_t size_type;

    typedef const value_type& const_reference;
    typedef const value_type* iterator;
    typedef const value_type* const_iterator;

    lite_buffer() EASY_NOEXCEPT
      : m_ptr(nullptr)
      , m_size() {
    }

    lite_buffer(nullptr_t) EASY_NOEXCEPT
      : m_ptr(nullptr)
      , m_size() {
    }

    lite_buffer(const value_type* ptr, size_type size) EASY_NOEXCEPT
      : m_ptr(ptr)
      , m_size(size) 
    {
      EASY_ASSERT(m_size > 0 || ptr);
    }

    template<class U>
    lite_buffer(const U* ptr, size_type size, typename boost::enable_if<boost::is_integral<U>>::type* = nullptr) EASY_NOEXCEPT
      : m_ptr(reinterpret_cast<const value_type*>(ptr))
      , m_size( (size * sizeof(U)) / sizeof(value_type) )
    {
      EASY_ASSERT(size * sizeof(U) == m_size * sizeof(value_type)); // slicing check
    }

    const value_type* data() const EASY_NOEXCEPT {
      return m_ptr;
    }

    size_type size() const EASY_NOEXCEPT {
      return m_size;
    }

    bool empty() const EASY_NOEXCEPT {
      return m_size == 0 || !m_ptr;
    }

    const_reference operator[] (size_t index) const {
      EASY_ASSERT(index < m_size);
      return m_ptr[index];
    }

    const_iterator begin() const EASY_NOEXCEPT {
      return m_ptr;
    }

    const_iterator end() const EASY_NOEXCEPT {
      return m_ptr + m_size;
    }

    const_iterator cbegin() const EASY_NOEXCEPT {
      return begin();
    }

    const_iterator cend() const EASY_NOEXCEPT {
      return end();
    }

    bool operator ! () const EASY_NOEXCEPT {
      return empty();
    }

  private:
    const value_type* m_ptr;
    size_type m_size;
  };

}

#endif