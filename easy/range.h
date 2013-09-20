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

  template<class Value>
  struct enumerable
  {
    typedef Value value_type;
    typedef boost::optional<value_type> result_type;

    virtual ~enumerable() { }
    virtual result_type get_next(error_code_ref ec = nullptr) = 0;
  };

  template<class Enumerable>
  class forward_iterator_base
    : public boost::iterator_facade<
      forward_iterator_base<Enumerable>, 
      const typename Enumerable::value_type, 
      boost::forward_traversal_tag
    >
  {
  public:
    typedef Enumerable enum_type;
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

    forward_iterator_base(enum_ptr r) EASY_NOEXCEPT
      : m_pimpl(std::move(r)) 
    {
      if (m_pimpl)
        m_op_value = m_pimpl->get_next();
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
}

#endif