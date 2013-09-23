
#include <easy/windows/registry.h>

#include <easy/strings/string_conv.h>
#include <easy/windows/error.h>

#include <easy/stlex/make_unique.h>

#include <vector>

namespace easy {
namespace windows 
{
  namespace
  {
    template<class RegKeyTracker>
    class reg_key_enumerator_impl
      : public enumerable<std::wstring>
    {
    public:    
      enum { MAX_REG_KEY_LENGTH = 256 };

      typedef RegKeyTracker tracker_type;

      reg_key_enumerator_impl(const tracker_type& key, error_code_ref ec)
        : m_key(key)
        , m_next_index(-1)
      {
        DWORD max_len = 0;
        DWORD key_count = 0;
        LONG res = ::RegQueryInfoKeyW(m_key.get(), nullptr, nullptr, nullptr, &key_count, &max_len, 
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
        LONG res = ::RegEnumKeyExW(m_key.get(), m_next_index, 
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
          if (m_buffer.size() < MAX_REG_KEY_LENGTH) {
            m_buffer.resize(MAX_REG_KEY_LENGTH);
            return get_next(ec);
          }
        }
        ec = make_win_error(res);
        return nullptr;
      }

    private:
      tracker_type m_key;
      int m_next_index;
      std::vector<wchar_t> m_buffer;
    };

  }


  namespace factory
  {
    reg_key_enumerator factory::enum_reg_keys(const scoped_reg_key::tracker& key, error_code_ref ec)
    {
      return std::make_unique<reg_key_enumerator_impl<scoped_reg_key::tracker>>(key, ec);
    }

    reg_key_enumerator factory::enum_reg_keys(const shared_reg_key::tracker& key, error_code_ref ec)
    {
      return std::make_unique<reg_key_enumerator_impl<shared_reg_key::tracker>>(key, ec);
    }
  }


}}