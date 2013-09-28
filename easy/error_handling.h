/*!
*  @file   easy/error_handling.h
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
  // Important usings from boost
  using boost::system::error_code;
  using boost::system::system_error;
  using boost::system::error_category;

  //! Generic errors we meet on every step
  enum class generic_error
  {
    ok = 0        ,  //!< all is OK
    null_ptr      ,  //!< null ptr
    invalid_value ,  //!< invalid value
    unexpected    ,  //!< unexpected
    bad_cast      ,  //!< bad_cast
  };

  //! Error category which describes generic errors
  class generic_error_category
    : public error_category
  {
  public:
    const char* name() const EASY_NOEXCEPT;
    std::string message(int ev) const EASY_FINAL;
  };

  //! Creates error_code object from generic_error value
  error_code make_error_code(generic_error e) EASY_NOEXCEPT;

  namespace detail
  {
    class error_code_holder
    {
    public:
      error_code_holder(error_code* ec) EASY_NOEXCEPT;
      ~error_code_holder();
      operator error_code& () EASY_NOEXCEPT;
      operator error_code* () EASY_NOEXCEPT;
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
    error_code_ref(error_code&&); // = delete
    error_code_ref& operator=(const error_code_ref&); // = delete
  public:

    //! Initializes throwable
    error_code_ref() EASY_NOEXCEPT;

    //! Initializes throwable
    error_code_ref(nullptr_t) EASY_NOEXCEPT;

    //! Creates wrapper over existing error_code
    error_code_ref(error_code& ec) EASY_NOEXCEPT;

    //! Creates wrapper over existing error_code or create a throwable one
    error_code_ref(error_code* ec) EASY_NOEXCEPT;

    //! Copy constructor. Makes a copy and null the code
    error_code_ref(const error_code_ref& ec) EASY_NOEXCEPT;

    //! Assign operator
    error_code_ref& operator = (const error_code& ec);

    //! Assign operator
    template<class CodeT>
    error_code_ref& operator = (const CodeT& code) {
      return (*this = error_code(code));
    }

    //! @{
    /*! Assign code and category */
    void assign(int code, const error_category& cat);
    void set(int code, const error_category& cat);
    //! @}

    //! Sets an error specific to the current OS
    void set_system_error(int code);

    //! Create the internal code
    void clear() EASY_NOEXCEPT;

    //! Returns a special error_code holder which can be passed to function what use error_code.
    //!
    //! Obtaining an internal type that should be passed to boost functions 
    //! and to be implicitly converted to boost::system::error_code
    detail::error_code_holder get() EASY_NOEXCEPT;

    //! Returns true if error_code is throwable
    bool is_throwable() const EASY_NOEXCEPT;

    //! Returns true is object contains an error code
    bool is_error() const EASY_NOEXCEPT;

    //! Makes it possible to use operator explicit_bool
    bool operator !() const EASY_NOEXCEPT;

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
