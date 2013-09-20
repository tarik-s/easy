
#include <easy/windows/environment.h>
#include <easy/windows/error.h>
#include <easy/windows/registry.h>
#include <easy/windows/api.h>

#include <easy/types.h>
#include <easy/scope.h>

#include <boost/algorithm/string/split.hpp>

#include <windows.h>
#include <array>

namespace easy {
namespace windows 
{

  class internal_environment_factory
  {
  public:
    static environment_variable create_variable(std::wstring && name, std::wstring && value) {
      return environment_variable(std::forward<std::wstring>(name), std::forward<std::wstring>(value));
    }
  };

  //////////////////////////////////////////////////////////////////////////

  environment_variable::environment_variable()
  {

  }

  environment_variable::environment_variable(environment_variable && v)
    : m_name(std::move(v.m_name))
    , m_value(std::move(v.m_value))
  {

  }

  environment_variable::environment_variable(std::wstring && name, std::wstring && value)
    : m_name(std::move(name))
    , m_value(std::move(value)) 
  {

  }

  environment_variable::environment_variable(const c_wstring& name, error_code_ref ec)
    : m_name(name)
    , m_value(api::get_environment_variable(name, ec))
  {
    if (ec) // reset the variable if an error occurred
      *this = environment_variable();
  }

  const std::wstring& environment_variable::get_name() const
  {
    return m_name;
  }

  const std::wstring& environment_variable::get_value() const
  {
    return m_value;
  }
  
  bool environment_variable::set_value(const c_wstring& value, environment_variable_target target, error_code_ref ec)
  {
    if (m_name.empty())
      return false;

    if (target == environment_variable_target::process) {
      if (::SetEnvironmentVariableW(m_name.c_str(), value.c_str())) {
        m_value = value;
        return true;
      }
    }
    ec = make_last_win_error();
    return false;
  }

  bool environment_variable::operator ! () const
  {
    return m_name.empty();
  }


  //////////////////////////////////////////////////////////////////////////


  environment_strings::environment_strings(error_code_ref ec) 
    : m_pstr(::GetEnvironmentStringsW())
  {
    if (!m_pstr)
      ec = make_last_win_error();
  }

  environment_strings::~environment_strings() EASY_NOEXCEPT
  {
    if (m_pstr) {
      BOOL res = ::FreeEnvironmentStringsW(m_pstr);
      EASY_ASSERT(res);
    }
  }

  const wchar_t* environment_strings::data() const EASY_NOEXCEPT
  {
    return m_pstr;
  }


  //////////////////////////////////////////////////////////////////////////


  class process_environment_variable_enumerator_impl
    : public environment_variable_enumerator
  {
  public:
    process_environment_variable_enumerator_impl(error_code_ref ec)
      : m_strs(ec)
      , m_pstr(m_strs.data())  {
    }

    result_type get_next(error_code_ref ec) EASY_FINAL {
      const c_wstring s(m_pstr);
      if (!s)
        return false;
      m_pstr += s.length() + 1;

      if (s[0] == L'=')
        return get_next(ec);

      wstring_list parts;
      boost::algorithm::split(parts, s, [](wchar_t sep) -> bool { return sep == L'='; });
      if (parts.size() != 2) {
        EASY_ASSERT(!"Incorrect variable");
        return get_next(ec);
      }
      return internal_environment_factory::create_variable(std::move(parts[0]), std::move(parts[1]));
    }

  private:
    environment_strings m_strs;
    const wchar_t* m_pstr;
  };

  //////////////////////////////////////////////////////////////////////////

  class reg_environment_variable_enumerator_impl
    : public environment_variable_enumerator
  {
  public:
    reg_environment_variable_enumerator_impl(environment_variable_target target, error_code_ref ec) 
      : m_key(make_key(target, ec))
    {      
    }

    result_type get_next(error_code_ref ec) EASY_FINAL {
      return false;
    }

  private:
    static reg_key make_key(environment_variable_target target, error_code_ref ec) {
      reg_open_options options;
      options << reg_access::read;

      if (target == environment_variable_target::machine)
        return reg_key(reg_hive::hklm, L"System/CurrentControlSet/Control/Session Manager/Environment", options, ec);
      else if (target == environment_variable_target::user)
        return reg_key(reg_hive::hkcu, L"Environment", options, ec);

      ec = generic_error::invalid_value;
      return reg_key();
    }
  private:
    reg_key m_key;
    
  };

  environment_variable_enumerator_ptr environment_variable_enumerator::create(environment_variable_target target, error_code_ref ec)
  {
    if (target == environment_variable_target::process)
      return std::make_shared<process_environment_variable_enumerator_impl>(ec);
    else
      return std::make_shared<reg_environment_variable_enumerator_impl>(target, ec);
  }

}}