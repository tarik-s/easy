
#ifndef EASY_ERROR_CODE_H_INCLUDED
#define EASY_ERROR_CODE_H_INCLUDED

#include <easy/config.h>


#ifdef EASY_OS_WINDOWS
#  include <Windows.h>
#endif


#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>

#include <easy/unspecified_bool.h>



#define ec_optional easy::error_code_ref ec = nullptr
#define ec_arg      easy::error_code_ref ec
#define ec_type     easy::error_code_ref


namespace easy
{
  using boost::system::error_code;
  using boost::system::system_error;
  using boost::system::error_category;

  class error_code_ref
  {
    class _code
    {
    public:
      _code(error_code* ec) EASY_NOEXCEPT
        : m_code(!ec ? m_own_code : *ec) {
      }
      ~_code() {
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
  private:
    error_code_ref(error_code&&);
    error_code_ref& operator=(const error_code_ref&);    
  public:

    // construction

    error_code_ref() EASY_NOEXCEPT
      : m_pcode(nullptr) {
    }

    error_code_ref(nullptr_t) EASY_NOEXCEPT
      : m_pcode(nullptr) {
    }

    error_code_ref(error_code& ec) EASY_NOEXCEPT
      : m_pcode(&ec) {
        clear();
    }

    error_code_ref(error_code* ec) EASY_NOEXCEPT
      : m_pcode(ec) {
        clear();
    }

    error_code_ref(const error_code_ref& ec) EASY_NOEXCEPT
      : m_pcode(ec.m_pcode) {
        clear();
    }

    // assignment

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

    void set_system_error(unsigned code) {
      set(code, boost::system::system_category());
    }

#ifdef EASY_OS_WINDOWS
    void set_last_win_error() {
      set_system_error(::GetLastError());
    }
#endif

    void clear() EASY_NOEXCEPT {
      if (m_pcode)
        m_pcode->clear();
    }

    // obtaining an internal type that should be passed to boost functions 
    // and to be implicitly converted to boost::system::error_code

    _code get() EASY_NOEXCEPT {
      return _code(m_pcode);
    }

    //

    operator unspecified_bool_type() const EASY_NOEXCEPT {
      return is_error() ? unspecified_true() : nullptr;
    }

    bool operator!() const EASY_NOEXCEPT {
      return !is_error();
    }

    bool throwable() const EASY_NOEXCEPT {
      return !m_pcode;
    }

  private:
    bool is_error() const EASY_NOEXCEPT {			
      return m_pcode && !!*m_pcode;
    }
  private:
    error_code* m_pcode;
  };
}

#endif