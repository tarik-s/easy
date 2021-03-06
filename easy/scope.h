/*!
 *  @file   easy/scope.h
 *  @author Sergey Tararay
 *  @date   2013
 */
#ifndef EASY_SCOPE_H_INCLUDED
#define EASY_SCOPE_H_INCLUDED

#include <easy/config.h>
#include <easy/stlex/nullptr_t.h>

#include <exception>
#include <functional>

#include <boost/noncopyable.hpp>

namespace easy
{
  //! Scoped Call
  class scoped_call
    : boost::noncopyable
  {
  public:

    //! Call type
    typedef std::function<void()> call_type;

    //! Scoped call condition
    enum class condition 
    { 
      none,    //!< None
      exit,    //!< Exit
      success, //!< Success
      failure  //!< Failure
    };

    scoped_call() EASY_NOEXCEPT
      : m_cond(condition::none) {
    }

    scoped_call(nullptr_t) EASY_NOEXCEPT
      : m_cond(condition::none) {
    }

    scoped_call(call_type f, condition cond) EASY_NOEXCEPT
      : m_cond(cond)
      , m_func(std::move(f)) 
    {
      EASY_ASSERT(!!m_func || m_cond == condition::none);
    }

    scoped_call(scoped_call && r) EASY_NOEXCEPT {
      *this = std::move(r);
    }

    scoped_call& operator = (scoped_call && r) EASY_NOEXCEPT {
      if (&r != this) { // just in case
        m_cond = r.m_cond;
        m_func = std::move(r.m_func);
        EASY_ASSERT(!!m_func || m_cond == condition::none);
      }
      return *this;
    }

    ~scoped_call() EASY_NOEXCEPT {
      if (m_cond == condition::none)
        return;

      if (m_cond == condition::exit || std::uncaught_exception() == (m_cond == condition::failure))
        m_func();
    }

  private:
    condition m_cond;
    call_type m_func;
  };
  
  //! Scope
  template<scoped_call::condition Cond>
  class scope
    : boost::noncopyable
  {
  public:
    typedef scoped_call::call_type call_type;

    scope(call_type f) EASY_NOEXCEPT
      : m_call(f, Cond) {
    }
  private:
    scoped_call m_call;
  };

  //! Scope Exit
  typedef scope<scoped_call::condition::exit> scope_exit;

  //! Scope Success
  typedef scope<scoped_call::condition::success> scope_success;

  //! Scope Failure
  typedef scope<scoped_call::condition::failure> scope_failure;

}

#endif
