
#include <easy/windows/registry.h>

#include <easy/strings/string_conv.h>
#include <easy/windows/error.h>

namespace easy {
namespace windows {
namespace registry { 

  reg_key::reg_key()
    : m_key(nullptr)
  {

  }

  reg_key::reg_key(HKEY key, const c_wstring& subkey, const reg_open_options& options, error_code_ref ec, void*)
    : m_key(nullptr)
  {
    DWORD disposition = 0;
    LONG res = (options.open_mode == reg_open_mode::open) 
      ? ::RegOpenKeyExW(key, subkey.c_str(), 0, (REGSAM)options.access, &m_key)
      : ::RegCreateKeyExW(key, subkey.c_str(), 0, nullptr, (DWORD)options.option, (REGSAM)options.access, nullptr, &m_key, &disposition);

    ec = win_error(res);
  }

  reg_key::reg_key(reg_key && r)
    : m_key(nullptr)
  {
    *this = std::move(r);
  }

  reg_key::reg_key(reg_hive h, const c_wstring& subkey, const reg_open_options& options, error_code_ref ec)
    : m_key(nullptr)  
  {
    *this = reg_key((HKEY)h, subkey, options, ec, nullptr);
  }

  reg_key::reg_key(const reg_key& key, const c_wstring& subkey, const reg_open_options& options, error_code_ref ec)
    : m_key(nullptr)
  {
    *this = reg_key(key.handle(), subkey, options, ec, nullptr);
  }

  reg_key& reg_key::operator=(reg_key && r)
  {
    if (this != &r)
      std::swap(m_key, r.m_key);
    return *this;
  }

  reg_key::~reg_key() EASY_NOEXCEPT
  {
    if (m_key) {
      LSTATUS res = ::RegCloseKey(m_key);
      EASY_ASSERT(res == ERROR_SUCCESS);
    }
  }

  bool reg_key::delete_key(const c_wstring& subkey, error_code_ref ec)
  {
    LONG res = ::RegDeleteKeyW(m_key, subkey.c_str());
    ec = win_error(res);
    return !ec;
  }

  bool reg_key::delete_key(reg_hive h, const c_wstring& subkey, error_code_ref ec)
  {
    LONG res = ::RegDeleteKeyW((HKEY)h, subkey.c_str());
    ec = win_error(res);
    return !ec;
  }





}}}