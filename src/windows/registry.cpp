
#include <easy/windows/registry.h>

#include <easy/strings/string_conv.h>
#include <easy/windows/error.h>

#include <easy/stlex/make_unique.h>

#include <vector>

namespace easy {
namespace windows 
{

  class reg_key_enumerator_impl
    : public reg_key_enumerator
  {
  public:    
    static const DWORD MAX_KEY_LENGTH = 256;

    reg_key_enumerator_impl(const reg_key& key, error_code_ref ec)
      : m_key(key)
      , m_next_index(-1)
    {
      DWORD max_len = 0;
      DWORD key_count = 0;
      LONG res = ::RegQueryInfoKeyW(m_key.handle(), nullptr, nullptr, nullptr, &key_count, &max_len, 
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);

      ec = make_win_error(res);

      if (!ec) {
        m_buffer.resize(max_len+1);
        m_next_index = 0;
      }
    }

    result_type get_next(error_code_ref ec = nullptr) 
    {
      if (m_next_index == -1)
        return nullptr;

      DWORD len = m_buffer.size();
      LONG res = ::RegEnumKeyExW(m_key.handle(), m_next_index, 
        &m_buffer[0], &len, nullptr, nullptr, nullptr, nullptr);

      if (res == ERROR_SUCCESS) {
        ++m_next_index;
        return std::wstring(m_buffer.data(), len);
      }
      else if (res == ERROR_NO_MORE_ITEMS) {
        m_next_index = -1;
        return nullptr;
      }
      else if (res == ERROR_MORE_DATA) {
        if (m_buffer.size() < MAX_KEY_LENGTH) {
          m_buffer.resize(MAX_KEY_LENGTH);
          return get_next(ec);
        }
      }
      ec = make_win_error(res);
      return nullptr;
    }

  private:
    const reg_key& m_key;
    int m_next_index;
    std::vector<wchar_t> m_buffer;
  };

  //////////////////////////////////////////////////////////////////////////

  reg_key::reg_key()
    : m_key(nullptr)
  {

  }

  reg_key::reg_key(HKEY key, const reg_path& subkey, const reg_open_options& options, error_code_ref ec, void*)
    : m_key(nullptr)
  {
    DWORD disposition = 0;
    HKEY _key = 0;
    std::wstring s = reg_path(subkey).make_preferred().wstring();
    LONG res = (options.open_mode == reg_open_mode::open) 
      ? ::RegOpenKeyExW(key, s.c_str(), 0, (REGSAM)options.access, &_key)
      : ::RegCreateKeyExW(key, s.c_str(), 0, nullptr, (DWORD)options.option, (REGSAM)options.access, nullptr, &_key, &disposition);

    ec = make_win_error(res);
    if (!ec)
      m_key = _key;
  }

  reg_key::reg_key(reg_key && r)
    : m_key(nullptr)
  {
    *this = std::move(r);
  }

  reg_key::reg_key(reg_hive h, const reg_path& subkey, const reg_open_options& options, error_code_ref ec)
    : m_key(nullptr)  
  {
    *this = reg_key((HKEY)h, subkey, options, ec, nullptr);
  }

  reg_key::reg_key(const reg_key& key, const reg_path& subkey, const reg_open_options& options, error_code_ref ec)
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
    ec = make_win_error(res);
    return !ec;
  }

  bool reg_key::delete_key(reg_hive h, const c_wstring& subkey, error_code_ref ec)
  {
    LONG res = ::RegDeleteKeyW((HKEY)h, subkey.c_str());
    ec = make_win_error(res);
    return !ec;
  }

  reg_key_enumerator_ptr reg_key::enum_keys(error_code_ref ec) const
  {
    return std::make_shared<reg_key_enumerator_impl>(*this, ec);
  }




}}