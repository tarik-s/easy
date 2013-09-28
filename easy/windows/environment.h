/*!
 *  @file   easy/windows/environment.h
 *  @author Sergey Tararay
 *  @date   2013
 */
#ifndef EASY_WINDOWS_ENVIRONMENT_H_INCLUDED
#define EASY_WINDOWS_ENVIRONMENT_H_INCLUDED

#include <easy/windows/config.h>

#include <easy/safe_bool.h>
#include <easy/strings.h>
#include <easy/error_code_ref.h>
#include <easy/range.h>

#include <boost/noncopyable.hpp>

namespace easy {
namespace windows 
{
  // forward declarations
  class environment_variable;
  class environment_strings;

  //! Enumerates environment variables
  typedef std::unique_ptr<enumerator<environment_variable>> environment_variable_enumerator; 

  //! Environment variable target
  enum class environment_variable_target
  {
    unknown,    //!< Unknown target
    process,    //!< Process
    machine,    //!< Machine
    user        //!< Current user
  };

  //! Environment strings
  class environment_strings
    : boost::noncopyable
  {
  public:
    //! @brief Constructor. Gets environment variables of the current process
    //! @sa error_code_ref
    environment_strings(error_code_ref ec = nullptr);

    //! Destructor. Frees allocated resources
    ~environment_strings() EASY_NOEXCEPT;

    //! Returns a string representation of all environment variables
    const wchar_t* data() const EASY_NOEXCEPT;
  private:
    wchar_t* m_pstr;
  };
  
  //! Environment variable
  class environment_variable
    : public easy::safe_bool<environment_variable>
  {
  public:
    //! Default constructor. Creates an empty variable
    environment_variable() EASY_NOEXCEPT;

    //! Creates an empty variable
    environment_variable(nullptr_t) EASY_NOEXCEPT;

    //! Move constructor
    environment_variable(environment_variable && v) EASY_NOEXCEPT;

    //! Opens existing variable 
    environment_variable(const lite_wstring& name, environment_variable_target target, error_code_ref ec = nullptr);

    //! Opens existing process environment variable
    environment_variable(const lite_wstring& name, error_code_ref ec = nullptr);

    //! Returns a variable name
    const std::wstring& get_name() const EASY_NOEXCEPT;

    //! Returns a variable value
    const std::wstring& get_value() const EASY_NOEXCEPT;

    //! Returns a variable origin
    environment_variable_target get_target() const EASY_NOEXCEPT;

    //! Sets variable value depending on target
    bool set_value(const lite_wstring& value, error_code_ref ec = nullptr);

    //! Makes the variable convertible to explicit bool
    bool operator ! () const EASY_NOEXCEPT;

    //! Check if the specified variable exists
    static bool exists(const lite_wstring& name, environment_variable_target target = environment_variable_target::process);

    //! Creates a variable
    static environment_variable create(const lite_wstring& name, const lite_wstring& value,
      environment_variable_target target, error_code_ref ec = nullptr);

    //! Creates a variable
    static environment_variable create(const lite_wstring& name, const lite_wstring& value, error_code_ref ec = nullptr);

    //! Creates environment variable enumerator
    static environment_variable_enumerator enum_variables(environment_variable_target target , error_code_ref ec = nullptr);

    //! Creates process environment variable enumerator
    static environment_variable_enumerator enum_variables(error_code_ref ec = nullptr);

  private:
    environment_variable(std::wstring && name, std::wstring && value, environment_variable_target target) EASY_NOEXCEPT;
    friend class internal_environment_factory;
  private:
    environment_variable_target m_target;
    std::wstring m_name;
    std::wstring m_value;
  };

}}

#endif