/*!
*  @file   easy/error_code_ref.h
*  @author Sergey Tararay
*  @date   2013
*/
#ifndef EASY_ERROR_CODE_REF_H_INCLUDED
#define EASY_ERROR_CODE_REF_H_INCLUDED

// Dependencies

#include <easy/config.h>
#include <easy/stlex/nullptr_t.h>
#include <easy/safe_bool.h>

#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <boost/type_traits/integral_constant.hpp>


namespace easy
{
  //! Important usings from boost
  using boost::system::error_code;
  using boost::system::system_error;
  using boost::system::error_category;

  //! generic errors we meet on every step
  enum class generic_error
  {
    ok = 0        ,  /*!< all is OK */
    null_ptr      ,  /*!< null ptr */
    invalid_value    /*!< invalid value */
  };

  //! Error category which describes generic errors
  class generic_error_category
    : public error_category
  {
  public:
    const char* name() const EASY_NOEXCEPT;
    std::string message(int ev) const EASY_FINAL;
  };

  //! Creates error_code object from enum value
  error_code make_error_code(generic_error e) EASY_NOEXCEPT;

  namespace detail
  {
    class error_code_holder
    {
    public:
      error_code_holder(error_code* ec) EASY_NOEXCEPT
        : m_code(!ec ? m_own_code : *ec) {
      }
      ~error_code_holder() {
        if (m_own_code) {
          if (!std::uncaught_exception())
            throw system_error(m_own_code);
          EASY_ASSERT(!"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
        }
      }
      operator error_code& () EASY_NOEXCEPT {
        return m_code;
      }
      operator error_code* () EASY_NOEXCEPT {
        return &m_code;
      }
    private:
      error_code m_own_code;
      error_code& m_code;
    };
  }

  //! Class is used to hold error codes
  class error_code_ref
    : public safe_bool<error_code_ref>
  {
  private:
    error_code_ref(error_code&&);
    error_code_ref& operator=(const error_code_ref&);    
  public:

    //! Initializes throwable
    error_code_ref() EASY_NOEXCEPT
      : m_pcode(nullptr) {
    }

    //! Initializes throwable
    error_code_ref(nullptr_t) EASY_NOEXCEPT
      : m_pcode(nullptr) {
    }

    /*!
    * Constructor .................
    */
    error_code_ref(error_code& ec) EASY_NOEXCEPT
      : m_pcode(&ec) {
        clear();
    }

    /*!
    * Constructor .................
    */
    error_code_ref(error_code* ec) EASY_NOEXCEPT
      : m_pcode(ec) {
        clear();
    }

    /*!
    * Constructor .................
    */
    error_code_ref(const error_code_ref& ec) EASY_NOEXCEPT
      : m_pcode(ec.m_pcode) {
        clear();
    }

    error_code_ref& operator = (const error_code& ec) {
      if (!m_pcode) {
        if (ec)
          throw system_error(ec);
      } else {
        *m_pcode = ec;
      }      
      return *this;
    }

    template<class CodeT>
    error_code_ref& operator = (const CodeT& code) {
      return (*this = error_code(code));
    }

    void assign(int code, const error_category& cat) {
      *this = error_code(code, cat);
    }

    void set(int code, const error_category& cat) {
      assign(code, cat);
    }

    void set_system_error(int code) {
      set(code, boost::system::system_category());
    }

    void clear() EASY_NOEXCEPT {
      if (m_pcode)
        m_pcode->clear();
    }

    // obtaining an internal type that should be passed to boost functions 
    // and to be implicitly converted to boost::system::error_code

    detail::error_code_holder get() EASY_NOEXCEPT {
      return detail::error_code_holder(m_pcode);
    }

    bool is_throwable() const EASY_NOEXCEPT {
      return !m_pcode;
    }

    bool is_error() const EASY_NOEXCEPT {			
      return m_pcode && !!*m_pcode;
    }

    bool operator !() const EASY_NOEXCEPT {
      return !is_error();
    }

  private:
    error_code* m_pcode;
  };
}

namespace boost {
  namespace system {
    template<>
    struct is_error_code_enum<easy::generic_error>
      : boost::true_type {
    };
  }
}

#endif
