/*!
 *  @file   easy/range.h
 *  @author Sergey Tararay
 *  @date   2013
 */
#ifndef EASY_RANGE_H_INCLUDED
#define EASY_RANGE_H_INCLUDED

#include <easy/config.h>

#include <easy/error_code_ref.h>

#include <boost/iterator/iterator_facade.hpp>
#include <boost/optional.hpp>


#include <type_traits>


namespace easy
{
  template<class T>
  bool is_in_range(T val, T begin, T end) {
    return val >= begin && val < end;
  }

  /*!
   * @{
   * @brief Check the value is equal to at least one of the others
   */
  template<class A>
  bool is_any_of(A v, A a1, A a2) {
    return v == a1 || v == a2;
  }

  template<class A>
  bool is_any_of(A v, A a1, A a2, A a3) {
    return v == a1 || v == a2 || v == a3;
  }

  template<class A>
  bool is_any_of(A v, A a1, A a2, A a3, A a4) {
    return v == a1 || v == a2 || v == a3 || v == a4;
  }

  template<class A>
  bool is_any_of(A v, A a1, A a2, A a3, A a4, A a5) {
    return v == a1 || v == a2 || v == a3 || v == a4 || v == a5;
  }

  template<class A>
  bool is_any_of(A v, A a1, A a2, A a3, A a4, A a5, A a6) {
    return v == a1 || v == a2 || v == a3 || v == a4 || v == a5 || v == a6;
  }

  /*! @} */

  //! Base enumerator class
  template<class Value>
  struct enumerator
  {
    //! Value type to be enumerated
    typedef Value value_type;

    //! Type returning by enumerator
    typedef boost::optional<value_type> result_type;

    //! Virtual destructor. 
    virtual ~enumerator() { }

    //! Main enumeration function. Must be implemented in derived class.
    virtual result_type get_next(error_code_ref ec = nullptr) = 0;
  };

  namespace detail
  {

    template<class Value>
    class forward_iterator_base
      : public boost::iterator_facade<
        forward_iterator_base<Value>, 
        const Value,
        boost::forward_traversal_tag
      >
    {
    public:
      typedef enumerator<Value> enum_type;
      typedef std::shared_ptr<enum_type> enum_ptr;

      forward_iterator_base() EASY_NOEXCEPT { }

      forward_iterator_base(forward_iterator_base && r) EASY_NOEXCEPT
        : m_pimpl(std::move(r.m_pimpl)) 
        , m_op_value(std::move(r.m_op_value))
      {
      }

      forward_iterator_base& operator = (forward_iterator_base && r) EASY_NOEXCEPT {
        if (&r != this) {
          m_pimpl = std::move(r.m_pimpl);
          m_op_value = std::move(r.m_op_value);
        }
        return *this;
      }

      forward_iterator_base(enum_ptr && r, error_code_ref ec)
        : m_pimpl(std::forward<enum_ptr>(r)) 
      {
        if (m_pimpl)
          m_op_value = m_pimpl->get_next(ec);
        if (!m_op_value)
          m_pimpl.reset();
      }

    private:
      friend class boost::iterator_core_access;

      void increment() {
        EASY_TEST_BOOL(m_pimpl && m_op_value);
        error_code ec;
        m_op_value = m_pimpl->get_next(ec);
        EASY_ASSERT(!ec);
        if (!m_op_value)
          m_pimpl.reset();
      }

      bool equal(const forward_iterator_base& other) const EASY_NOEXCEPT {
        if (m_pimpl == other.m_pimpl)
          return true;
        if ((!m_op_value || !m_pimpl) && (!other.m_op_value || !other.m_pimpl))
          return true;
        return false;
      }

      reference dereference() const {
        EASY_TEST_BOOL(m_pimpl && m_op_value);
        return m_op_value.get();
      }

    private:
      typedef typename enum_type::result_type result_type;
    private:
      enum_ptr m_pimpl;
      result_type m_op_value;
    };

    template<class Value>
    class range
      : boost::noncopyable
    {
    public:
      typedef forward_iterator_base<Value> iterator_type;

      range(typename iterator_type::enum_ptr && v, error_code_ref ec)
        : m_begin(std::forward<iterator_type::enum_ptr>(v), ec) {
      }

      range(range && r)
        : m_begin(std::forward<iterator_type>(r.m_begin)) {
      }

      iterator_type begin() const {
        return m_begin;
      }

      iterator_type end() const {
        return iterator_type();
      }
    private:
      iterator_type m_begin;
    };

  }


  //! Creates iterator range from any class derived from enumerator<T>, and makes it possible to use the range in for loop.
  template<class Value>
  detail::range<Value> make_range(std::unique_ptr<enumerator<Value>> && enum_ptr, error_code_ref ec = nullptr)
  {
    return detail::range<Value>(std::forward<std::unique_ptr<enumerator<Value>>>(enum_ptr), ec);
  }

}

#endif