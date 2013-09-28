
#include <easy/windows/environment.h>
#include <easy/windows/error.h>
#include <easy/windows/registry.h>
#include <easy/windows/api.h>

#include <easy/types.h>
#include <easy/scope.h>
#include <easy/stlex/make_unique.h>

#include <boost/algorithm/string/split.hpp>

#include <windows.h>
#include <array>

namespace easy {
namespace windows 
{
  namespace
  {
    const wchar_t* machine_key = L"System/CurrentControlSet/Control/Session Manager/Environment";
    const wchar_t* user_key = L"Environment";

    static scoped_reg_key make_env_reg_key(environment_variable_target target, reg_access access, error_code_ref ec) 
    {
      reg_open_params params(access);
      if (target == environment_variable_target::machine)
        return scoped_reg_key(reg_hive::hklm, machine_key, params, ec);
      else if (target == environment_variable_target::user)
        return scoped_reg_key(reg_hive::hkcu, user_key, params, ec);

      ec = generic_error::invalid_value;
      return nullptr;
    }
  }

  class internal_environment_factory
  {
  public:
    static environment_variable create_variable(std::wstring && name, std::wstring && value, environment_variable_target target) {
      return environment_variable(std::forward<std::wstring>(name), std::forward<std::wstring>(value), target);
    }
  };

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
    : public enumerator<environment_variable>
  {
  public:
    process_environment_variable_enumerator_impl(error_code_ref ec)
      : m_strs(ec)
      , m_pstr(m_strs.data())  {
    }

    result_type get_next(error_code_ref ec) EASY_FINAL {
      const lite_wstring s(m_pstr);
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
      return internal_environment_factory::create_variable(std::move(parts[0]), std::move(parts[1]), environment_variable_target::process);
    }

  private:
    environment_strings m_strs;
    const wchar_t* m_pstr;
  };

  //////////////////////////////////////////////////////////////////////////

  class reg_environment_variable_enumerator_impl
    : public enumerator<environment_variable>
  {
  public:
    reg_environment_variable_enumerator_impl(environment_variable_target target, error_code_ref ec) 
      : m_target(target)
      , m_key(make_env_reg_key(target, reg_access::read, ec))
    {      
      if (!ec)
        m_enum = m_key.enum_values(ec);
    }

    result_type get_next(error_code_ref ec) EASY_FINAL 
    {
      if (!m_enum) {
        ec = generic_error::null_ptr;
        return nullptr;
      }

      auto opt_name = m_enum->get_next(ec);
      if (!opt_name || opt_name->empty())
        return nullptr;
      std::wstring name = *opt_name;
      std::wstring value = m_key.get_value<std::wstring>(name, L"", ec);
      if (ec)
        return nullptr;

      if (value.empty())  // value can be set to empty manually, skip it
        return get_next(ec);

      return internal_environment_factory::create_variable(std::move(name), std::move(value), m_target);
    }

  private:
    environment_variable_target m_target;
    scoped_reg_key m_key;
    reg_item_enumerator m_enum;
  };


  //////////////////////////////////////////////////////////////////////////

  environment_variable::environment_variable()
    : m_target(environment_variable_target::unknown)
  {

  }

  environment_variable::environment_variable(nullptr_t)
    : m_target(environment_variable_target::unknown)
  {

  }

  environment_variable::environment_variable(environment_variable && v)
    : m_target(v.m_target)
    , m_name(std::move(v.m_name))
    , m_value(std::move(v.m_value))
  {
    v.m_target = environment_variable_target::unknown;
  }

  environment_variable::environment_variable(std::wstring && name, std::wstring && value, environment_variable_target target)
    : m_target(target)
    , m_name(std::move(name))
    , m_value(std::move(value)) 
  {

  }

  environment_variable::environment_variable(const lite_wstring& name, environment_variable_target target, error_code_ref ec)
    : m_target(target)
    , m_name(name)
  {
    if (m_target == environment_variable_target::process) {
      m_value = api::get_environment_variable(name, ec);
    }
    else {
      scoped_reg_key key = make_env_reg_key(target, reg_access::read, ec);
      if (!ec) {
        auto val = key.get_value<std::wstring>(name, ec);
        if (!ec && val) {
          m_value = *val;
        }
      }
    }

    if (m_value.empty()) // a value cannot be empty
      ec = generic_error::invalid_value;

    if (ec) // reset the variable if an error occurred
      *this = environment_variable();
  }

  environment_variable::environment_variable(const lite_wstring& name, error_code_ref ec)
    : m_target(environment_variable_target::unknown)
  {
    *this = environment_variable(name, environment_variable_target::process, ec);
  }

  const std::wstring& environment_variable::get_name() const
  {
    return m_name;
  }

  const std::wstring& environment_variable::get_value() const
  {
    return m_value;
  }

  std::wstring environment_variable::get_expanded_value(error_code_ref ec) const
  {
    return api::expand_environment_variable(m_value, ec);
  }

  environment_variable_target environment_variable::get_target() const
  {
    return m_target;
  }
  
  bool environment_variable::set_value(const lite_wstring& value, error_code_ref ec)
  {
    environment_variable var = create(m_name, value, m_target, ec);
    if (!ec && var) {
      *this = var;
      return true;
    }
    return false;
  }

  bool environment_variable::operator ! () const
  {
    return m_name.empty();
  }

  bool environment_variable::exists(const lite_wstring& name, environment_variable_target target)
  {
    error_code ec;
    environment_variable var(name, target, ec);
    return !!var;
  }

  environment_variable_enumerator environment_variable::enum_variables(environment_variable_target target, error_code_ref ec)
  {
    if (target == environment_variable_target::process)
      return std::make_unique<process_environment_variable_enumerator_impl>(ec);
    else
      return std::make_unique<reg_environment_variable_enumerator_impl>(target, ec);
  }

  environment_variable_enumerator environment_variable::enum_variables(error_code_ref ec)
  {
    return enum_variables(environment_variable_target::process, ec);
  }

  environment_variable environment_variable::create(const lite_wstring& name, const lite_wstring& value, 
    environment_variable_target target, error_code_ref ec)
  {
    if (!name || target == environment_variable_target::unknown) {
      ec = generic_error::invalid_value;
      return nullptr;
    }

    if (target == environment_variable_target::process) {
      if (!::SetEnvironmentVariableW(name.c_str(), value.c_str())) {
        ec = make_last_win_error();
        return nullptr;
      }
    }
    else {
      scoped_reg_key key = make_env_reg_key(target, reg_access::all, ec);
      if (ec)
        return nullptr;

      bool ok = false;
      if (!value)
        ok = key.delete_value(name, ec);
      else 
        ok = key.set_value(name, value, ec);

      if (ec)
        return nullptr;
    }

    return internal_environment_factory::create_variable(std::wstring(name), std::wstring(value), target);
  }

  environment_variable environment_variable::create(const lite_wstring& name, const lite_wstring& value, error_code_ref ec)
  {
    return create(name, value, environment_variable_target::process, ec);
  }

  namespace api
  {
    std::wstring get_environment_variable(const lite_wstring& name, error_code_ref ec)
    {
      if (name.empty()) {
        ec = generic_error::invalid_value;
        return std::wstring();
      }
      std::wstring result(64, wchar_t());
      while (DWORD len = ::GetEnvironmentVariableW(name.c_str(), &result[0], result.size())) {
        bool ok = len <= result.size();
        result.resize(len);
        if (ok)
          return result;
      }
      ec = make_last_win_error();
      return std::wstring();
    }

    std::wstring expand_environment_variable(const lite_wstring& value, error_code_ref ec)
    {
      std::wstring res;
      while (DWORD len = ::ExpandEnvironmentStringsW(value.c_str(), res.empty() ? nullptr : &res[0], res.size()))
      {
        if (len > res.size()) {
          res.resize(len);
          continue;
        }

        while (!res.empty() && !res.back())
          res.pop_back();

        return res;
      }
      ec = make_last_win_error();
      return std::wstring();
    }

  }

}}