/*!
* \file   easy/error_code_ref.h
* \author Sergey Tararay
* \date   04.08.2013
*
* FILE DESCRIPTION
*/
#ifndef EASY_ERROR_CODE_REF_H_INCLUDED
#define EASY_ERROR_CODE_REF_H_INCLUDED

// Dependencies

#include <easy/config.h>
#include <easy/stlex/nullptr_t.h>

#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>

#include <easy/unspecified_bool.h>
#include <easy/detail/error_code_ref_detail.h>

namespace easy
{
  using boost::system::error_code;
  using boost::system::system_error;
  using boost::system::error_category;

  //! generic_error
  enum class generic_error
  {
    null_ptr
  };

  /*!
  * \class error_code_ref
  * \headerfile error_code_ref.h <easy/error_code_ref.h>
  * \brief error_code_ref class that is used to indicate conversion errors
  */
  class error_code_ref
  {
  private:
    error_code_ref(error_code&&);
    error_code_ref& operator=(const error_code_ref&);    
  public:

    /*!
    * Default constructor
    */
    error_code_ref() EASY_NOEXCEPT
      : m_pcode(nullptr) {
    }

    /*!
    * Constructor which creates throwable error_code_ref
    */
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

    //!
    //! Assignment .................
    //!
    error_code_ref& operator = (const error_code& ec) {
      if (!m_pcode) {
        if (ec)
          throw system_error(ec);
      } else {
        *m_pcode = ec;
      }      
      return *this;
    }

    //!
    //! Assignment .................
    //!
    template<class CodeT>
    error_code_ref& operator = (const CodeT& code) {
      return (*this = error_code(code));
    }

    //!
    //! assign code 
    //!
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

    bool is_error() const EASY_NOEXCEPT {			
      return m_pcode && !!*m_pcode;
    }
  private:
    error_code* m_pcode;
  };
}

#endif
