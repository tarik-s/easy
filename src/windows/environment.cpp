
#include <easy/windows/environment.h>
#include <easy/windows/error.h>
#include <easy/windows/registry.h>

#include <easy/types.h>
#include <easy/scope.h>

#include <boost/algorithm/string/split.hpp>

#include <windows.h>
#include <array>

namespace easy {
namespace windows 
{
  namespace 
  {
    class environment_strings
    {
    public:
      environment_strings(error_code_ref ec) 
        : m_pstr(::GetEnvironmentStringsW())
      {
        if (!m_pstr)
          ec = make_last_win_error();
      }
      ~environment_strings() EASY_NOEXCEPT {
        if (m_pstr) {
          BOOL res = ::FreeEnvironmentStringsW(m_pstr);
          EASY_ASSERT(res);
        }
      }

      LPWCH data() const EASY_NOEXCEPT {
        return m_pstr;
      }
    private:
      LPWCH m_pstr;
    };

    bool get_env_var(const c_wstring& name, std::wstring& value, error_code_ref ec) 
    {
      if (value.empty())
        value.resize(256);

      DWORD len = ::GetEnvironmentVariableW(name.c_str(), &value[0], value.size());
      if (len > 0) {
        bool ok = len <= value.size();
        value.resize(len);        
        return ok ? true : get_env_var(name, value, ec);
      }
      ec = make_last_win_error();
      return false;
    }
  }


  environment_variable::environment_variable()
  {

  }

  environment_variable::environment_variable(environment_variable && v)
    : m_name(std::move(v.m_name))
    , m_value(std::move(v.m_value))
  {

  }

  environment_variable::environment_variable(std::wstring && _name, std::wstring && _value)
    : m_name(std::move(_name))
    , m_value(std::move(_value)) 
  {

  }

  environment_variable::environment_variable(const c_wstring& _name, error_code_ref ec)
    : m_name(_name)
  {
    if (!get_env_var(_name, m_value, ec))
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
  
  bool environment_variable::set_value(const c_wstring& _value, target _target, error_code_ref ec)
  {
    if (m_name.empty())
      return false;

    if (_target == target::process) {
      if (::SetEnvironmentVariableW(m_name.c_str(), _value.c_str())) {
        m_value = _value;
        return true;
      }
    }
    ec = make_last_win_error();
    return false;
  }




  //////////////////////////////////////////////////////////////////////////

  class environment_variable_iterator_impl
  {
  public:
    typedef environment_variable_target target;

    environment_variable_iterator_impl(target _target) EASY_NOEXCEPT
      : m_target(_target)  {
    }

    virtual ~environment_variable_iterator_impl() EASY_NOEXCEPT {

    }

    virtual bool increment() = 0;

    target get_target() const EASY_NOEXCEPT {
      return target::process;
    }

    environment_variable& deref() EASY_NOEXCEPT {
      return m_var;
    }

    bool empty() const EASY_NOEXCEPT {
      return !m_var;
    }
  protected:
    void set_var(std::wstring && _name, std::wstring && _value) EASY_NOEXCEPT {
      m_var = environment_variable(std::move(_name), std::move(_value));
    }

  private:
    const target m_target;
    environment_variable m_var;
  };

  class process_environment_variable_iterator_impl
    : public environment_variable_iterator_impl
  {
  public:
    process_environment_variable_iterator_impl(error_code_ref ec)
      : environment_variable_iterator_impl(target::process)
      , m_strs(ec)
      , m_pstr(m_strs.data())  {
    }

    bool increment() EASY_FINAL {
      const c_wstring s(m_pstr);
      if (!s)
        return false;
      m_pstr += s.length() + 1;

      if (s[0] == L'=')
        return increment();
      wstring_list parts;
      boost::algorithm::split(parts, s, [](wchar_t sep) -> bool { return sep == L'='; });
      if (parts.size() != 2) {
        EASY_ASSERT(!"Incorrect variable");
        return increment();
      }
      set_var(std::move(parts[0]), std::move(parts[1]));
      return true;
    }

  private:
    environment_strings m_strs;
    LPWCH m_pstr;
  };


  class reg_environment_variable_iterator_impl
    : public environment_variable_iterator_impl
  {
  public:
    reg_environment_variable_iterator_impl(target _target, error_code_ref ec) 
      : environment_variable_iterator_impl(_target) 
      , m_key(make_key(_target, ec))
    {

    }

    bool increment() EASY_FINAL {
      return false;
    }

  private:
    static reg_key make_key(target _target, error_code_ref ec) {
      reg_open_options options;
      options << reg_access::read;

      if (_target == target::machine)
        return reg_key(reg_hive::hklm, L"System\\CurrentControlSet\\Control\\Session Manager\\Environment", options, ec);
      else if (_target == target::user)
        return reg_key(reg_hive::hkcu, L"Environment", options, ec);

      return reg_key();
    }
  private:
    reg_key m_key;
    reg_value_iterator m_it;
  };

  environment_variable_iterator::environment_variable_iterator()
  {

  }

  environment_variable_iterator::environment_variable_iterator(environment_variable_iterator && r)
    : m_pimpl(std::move(r.m_pimpl))
  {

  }

  environment_variable_iterator::environment_variable_iterator(target _target, error_code_ref ec)
  {
    if (_target == target::process)
      m_pimpl = std::make_shared<process_environment_variable_iterator_impl>(ec);
    else
      m_pimpl = std::make_shared<reg_environment_variable_iterator_impl>(_target, ec);    
  }

  environment_variable_iterator::~environment_variable_iterator()
  {

  }

  void environment_variable_iterator::increment()
  {
    EASY_TEST_BOOL(m_pimpl);
    error_code ec;
    if (!m_pimpl->increment())
      m_pimpl.reset();
  }

  bool environment_variable_iterator::equal(const environment_variable_iterator& other) const
  {
    const bool empty1 = !m_pimpl || m_pimpl->empty();
    const bool empty2 = !other.m_pimpl || other.m_pimpl->empty();
    return (m_pimpl == other.m_pimpl || empty1 == empty2);
  }

  const environment_variable& environment_variable_iterator::dereference() const
  {
    EASY_TEST_BOOL(m_pimpl);
    return m_pimpl->deref();
  }

  environment_variable_iterator::target environment_variable_iterator::get_target() const
  {
    return m_pimpl ? m_pimpl->get_target() : target::unknown;
  }

  namespace environment
  {
    environment_variable_iterator enum_variables(target _target, error_code_ref ec)
    {
      return environment_variable_iterator(_target, ec);
    }

    environment_variable_range get_variables(target _target, error_code_ref ec)
    {
      return boost::make_iterator_range(enum_variables(_target, ec), environment_variable_iterator());
    }

  }


}}