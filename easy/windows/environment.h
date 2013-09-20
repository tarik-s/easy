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
  /*!
   *  @enum environment_variable_target
   */ 

  enum class environment_variable_target
  {
    unknown,
    process,
    machine,
    user
  };
  
  /*!
   *  @class environment_variable
   */

  class environment_variable
    : public easy::safe_bool<environment_variable>
  {
  public:
    environment_variable() EASY_NOEXCEPT;
    environment_variable(environment_variable && v) EASY_NOEXCEPT;
    environment_variable(const c_wstring& _name, error_code_ref ec = nullptr);

    const std::wstring& get_name() const EASY_NOEXCEPT;
    const std::wstring& get_value() const EASY_NOEXCEPT;
    bool set_value(const c_wstring& value, environment_variable_target target, error_code_ref ec = nullptr);

    bool operator !() const EASY_NOEXCEPT;

  private:
    environment_variable(std::wstring && name, std::wstring && value) EASY_NOEXCEPT;
    friend class internal_environment_factory;
  private:
    std::wstring m_name;
    std::wstring m_value;
  };

  /*!
   * @class environment_strings
   */

  class environment_strings
    : boost::noncopyable
  {
  public:
    environment_strings(error_code_ref ec = nullptr);
    ~environment_strings() EASY_NOEXCEPT;

    const wchar_t* data() const EASY_NOEXCEPT;
  private:
    wchar_t* m_pstr;
  };

  /*!
   *  @class environment_variable_enumerator
   */

  class environment_variable_enumerator;
  typedef std::shared_ptr<environment_variable_enumerator> environment_variable_enumerator_ptr;

  class environment_variable_enumerator
    : public enumerable<environment_variable> 
  {
  public:
    static environment_variable_enumerator_ptr create(environment_variable_target target, error_code_ref ec = nullptr);
  };  

}}

#endif