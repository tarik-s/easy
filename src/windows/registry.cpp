#include <easy/windows/registry.h>

#include <easy/windows/error.h>
#include <easy/windows/dynamic_library.h>

#include <easy/stlex/stlex.h>

#include <easy/strings/conv.h>

#include <easy/lite_buffer.h>

#include <vector>
#include <array>

namespace easy {
namespace windows 
{
  namespace
  {
    template<class RegKeyTracker>
    class reg_key_enumerator_impl
      : public enumerator<std::wstring>
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
          std::wstring res(m_buffer.data(), len);
          while (!res.empty() && !res.back())
            res.pop_back();
          return res;
          //return std::wstring(m_buffer.data(), len);
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

    template<class RegKeyTracker>
    class reg_value_name_enumerator_impl
      : public enumerator<std::wstring>
    {
    public:    
      enum { MAX_REG_VALUE_NAME_LENGTH = 163834 };

      typedef RegKeyTracker tracker_type;

      reg_value_name_enumerator_impl(const tracker_type& key, error_code_ref ec)
        : m_key(key)
        , m_next_index(-1)
      {
        DWORD max_len = 0;
        DWORD value_count = 0;
        LONG res = ::RegQueryInfoKeyW(m_key.get(), nullptr, nullptr, nullptr, nullptr, nullptr,
          nullptr, &value_count, &max_len, nullptr, nullptr, nullptr);

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
        LONG res = ::RegEnumValue(m_key.get(), m_next_index, 
          &m_buffer[0], &len, nullptr, nullptr, nullptr, nullptr);

        if (res == ERROR_SUCCESS) {
          ++m_next_index;
          std::wstring res(m_buffer.data(), len);
          while (!res.empty() && !res.back())
            res.pop_back();
          return res;
        }
        else if (res == ERROR_NO_MORE_ITEMS) {
          m_next_index = -1;
          return nullptr;
        }
        else if (res == ERROR_MORE_DATA) {
          if (m_buffer.size() < MAX_REG_VALUE_NAME_LENGTH) {
            m_buffer.resize(MAX_REG_VALUE_NAME_LENGTH);
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

  namespace api
  {

    bool is_reg_key_handle_valid(reg_key_handle h)
    {
      return h != invalid_reg_key_handle;
    }

    bool check_reg_key_handle(reg_key_handle h, error_code_ref ec)
    {
      if (!is_reg_key_handle_valid(h)) {
        ec = make_win_error(ERROR_INVALID_HANDLE);
        return false;
      }
      return true;
    }

    bool close_reg_key(reg_key_handle h, error_code_ref ec)
    {
      if (!check_reg_key_handle(h, ec))
        return false;

      LSTATUS res = ::RegCloseKey(h);
      ec = make_win_error(res);
      return !ec;
    }

    bool delete_reg_key(reg_key_handle h, const reg_path& subkey, error_code_ref ec)
    {
      if (!check_reg_key_handle(h, ec))
        return false;

      LONG res = ::RegDeleteKeyW(h, reg_path(subkey).make_preferred().c_str());
      ec = make_win_error(res);
      return !ec;
    }

    bool delete_reg_value(reg_key_handle h, const lite_wstring& name, error_code_ref ec)
    {
      if (!check_reg_key_handle(h, ec))
        return false;

      LONG res = ::RegDeleteValueW(h, name.c_str());
      ec = make_win_error(res);
      return !ec;
    }

    reg_key_handle create_reg_key(reg_key_handle h, const reg_path& subkey, const reg_open_params& params, error_code_ref ec)
    {
      if (!check_reg_key_handle(h, ec))
        return invalid_reg_key_handle;

      // normalize registry path
      reg_path s(subkey);
      s.make_preferred();

      DWORD disposition = 0;
      reg_key_handle key = invalid_reg_key_handle;

      REGSAM sam = (REGSAM)params.get(reg_access::read);
      if (params.get(reg_virtualization::enabled) == reg_virtualization::enabled)
        sam |= KEY_WOW64_64KEY;

      LONG res = (params.get(reg_open_mode::open) == reg_open_mode::open)
        ? ::RegOpenKeyExW(h, s.c_str(), 0, sam, &key)
        : ::RegCreateKeyExW(h, s.c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE, sam, nullptr, &key, &disposition);

      ec = make_win_error(res);
      return !ec ? key : invalid_reg_key_handle;
    }

    namespace
    {
      reg_value_kind reg_value_kind_from_winapi_type(DWORD type) {
        switch (type)
        {
        case REG_NONE      : return reg_value_kind::none;
        case REG_SZ        : return reg_value_kind::string;
        case REG_EXPAND_SZ : return reg_value_kind::expand_string;
        case REG_MULTI_SZ  : return reg_value_kind::multi_string;
        case REG_BINARY    : return reg_value_kind::binary;
        case REG_DWORD     : return reg_value_kind::dword;
        case REG_QWORD     : return reg_value_kind::qword;
        }
        return reg_value_kind::unknown;
      }

      DWORD winapi_type_from_reg_value_kind(reg_value_kind type) {
        switch (type)
        {
        case reg_value_kind::none          : return REG_NONE;
        case reg_value_kind::string        : return REG_SZ;
        case reg_value_kind::expand_string : return REG_EXPAND_SZ;
        case reg_value_kind::multi_string  : return REG_MULTI_SZ;
        case reg_value_kind::binary        : return REG_BINARY;
        case reg_value_kind::dword         : return REG_DWORD;
        case reg_value_kind::qword         : return REG_QWORD;
        }
        return REG_NONE;
      }

      DWORD get_reg_value_type_internal(reg_key_handle h, const lite_wstring& name, error_code_ref ec)
      {
        DWORD type = REG_NONE;
        if (!check_reg_key_handle(h, ec))
          return type;

        LONG res = ::RegQueryValueEx(h, name.c_str(), nullptr, &type, nullptr, nullptr);
        ec = make_win_error(res);
        return type;
      }

    }

    reg_value_kind get_reg_value_kind(reg_key_handle h, const lite_wstring& name, error_code_ref ec)
    {
      DWORD type = get_reg_value_type_internal(h, name, ec);
      return reg_value_kind_from_winapi_type(type);
    }

    bool set_reg_value(reg_key_handle h, const lite_wstring& name, reg_value_kind kind, const byte* data_ptr, size_t size, error_code_ref ec)
    {
      if (!check_reg_key_handle(h, ec))
        return false;

      DWORD valtype = winapi_type_from_reg_value_kind(kind);
      if (valtype == REG_NONE) {
        ec = generic_error::invalid_value;
        return false;
      }

      LONG res = ::RegSetValueEx(h, name.c_str(), 0, valtype, data_ptr, size);
      ec = make_win_error(res);
      return !ec;
    }

    bool set_reg_value(reg_key_handle h, const lite_wstring& name, reg_value_kind kind, const reg_value& value, error_code_ref ec)
    {
      switch(kind)
      {
      case reg_value_kind::dword:
        {
          const uint32* p = boost::get<uint32>(&value);
          if (p)
            return set_reg_value_uint32(h, name, *p, ec);
          ec = generic_error::null_ptr;
          return false;
        }
      case reg_value_kind::qword:
        {
          const uint64* p = boost::get<uint64>(&value);
          if (p)
            return set_reg_value_uint64(h, name, *p, ec);
          ec = generic_error::null_ptr;
          return false;
        }
      case reg_value_kind::string:
      case reg_value_kind::expand_string:
        {
          const std::wstring* p = boost::get<std::wstring>(&value);
          if (p) {
            return kind == reg_value_kind::string 
              ? set_reg_value_string(h, name, *p, ec) 
              : set_reg_value_exp_string(h, name, *p, ec);
          }
          ec = generic_error::null_ptr;
          return false;
        }
      case reg_value_kind::multi_string:
        {
          const wstring_list* p = boost::get<wstring_list>(&value);
          if (p) {
            std::vector<wchar_t> buf;
            for (const std::wstring& s: *p) {
//               if (s.empty())
//                 continue;
              buf.insert(buf.end(), s.begin(), s.end());
              buf << L'\0';
            }
            
            buf << L'\0';
            return set_reg_value_multi_string(h, name, lite_buffer<wchar_t>(buf.data(), buf.size()), ec);
          }
          return false;
        }
      case reg_value_kind::binary:
        {
          const byte_vector* p = boost::get<byte_vector>(&value);
          if (p)
            return set_reg_value(h, name, reg_value_kind::binary, p->empty() ? nullptr : p->data(), p->size());
          ec = generic_error::null_ptr;
          return false;
        }
      }
      ec = generic_error::invalid_value;
      return false;
    }

    bool set_reg_value(reg_key_handle h, const lite_wstring& name, const reg_value& value, error_code_ref ec)
    {
      reg_value_kind kind = static_cast<reg_value_kind>(value.which());
      if (!is_any_of(kind, 
        reg_value_kind::dword, reg_value_kind::qword,
        reg_value_kind::string, reg_value_kind::multi_string, 
        reg_value_kind::binary
        ))
        kind = reg_value_kind::unknown;
      return set_reg_value(h, name, kind, value, ec);
    }
    
    bool set_reg_value_uint32(reg_key_handle h, const lite_wstring& name, uint32 value, error_code_ref ec)
    {
      DWORD v = value;
      return set_reg_value(h, name, reg_value_kind::dword, (const byte*)&v, sizeof(v), ec);
    }

    bool set_reg_value_uint64(reg_key_handle h, const lite_wstring& name, uint64 value, error_code_ref ec)
    {
      ULONGLONG v = value;
      return set_reg_value(h, name, reg_value_kind::qword, (const byte*)&v, sizeof(v), ec);
    }

    bool set_reg_value_string(reg_key_handle h, const lite_wstring& name, const lite_wstring& value, error_code_ref ec)
    {
      return set_reg_value(h, name, reg_value_kind::string, (const byte*)value.c_str(), value.size() * sizeof(lite_wstring::value_type), ec);
    }

    bool set_reg_value_exp_string(reg_key_handle h, const lite_wstring& name, const lite_wstring& value, error_code_ref ec)
    {
      return set_reg_value(h, name, reg_value_kind::expand_string, (const byte*)value.c_str(), value.size() * sizeof(lite_wstring::value_type), ec);
    }


    bool set_reg_value_multi_string(reg_key_handle h, const lite_wstring& name, const lite_buffer<wchar_t>& value, error_code_ref ec)
    {
      return set_reg_value(h, name, reg_value_kind::multi_string, (const byte*)value.data(), value.size() * sizeof(lite_buffer<wchar_t>::value_type), ec);
    }

    reg_value get_reg_value(reg_key_handle h, const lite_wstring& name, error_code_ref ec)
    {
      if (!check_reg_key_handle(h, ec))
        return reg_value();

      const std::array<uint32, 5> sizes = { 64, 256, 1024, 16*1024, 1024*1024 };

      DWORD type = REG_NONE;
      
      byte_vector buf(32);
      uint32 nextSizeIndex = 0;
      while (true)
      { 
        // try to get a value
        DWORD size = buf.size();
        LONG res = ::RegQueryValueEx(h, name.c_str(), nullptr, &type, &buf[0], &size);

        if (res == ERROR_MORE_DATA) {
          // there is no enough data, allocate more memory
          if (nextSizeIndex >= sizes.size()) {
            EASY_ASSERT(!"The value size is too large");
          }
          else {
            buf.resize(sizes[nextSizeIndex++]);
            continue;
          }
        }

        if (res != ERROR_SUCCESS) 
        {
          if (res != ERROR_FILE_NOT_FOUND) // it value doesn't exist it's not an error
            ec = make_win_error(res);
          return reg_value();
        }

        buf.resize(size);
        break;
      }

      // We've successfully gotten a raw value, try to convert it
      const reg_value_kind kind = reg_value_kind_from_winapi_type(type);
      switch (kind)
      {
      case easy::windows::reg_value_kind::dword:
        {
          uint32 value = 0;
          EASY_ASSERT(buf.size() == sizeof(value));
          if (buf.size() == sizeof(value))
            std::memcpy(&value, buf.data(), buf.size());
          return value;
        }        
      case easy::windows::reg_value_kind::qword:
        {
          uint64 value = 0;
          EASY_ASSERT(buf.size() == sizeof(value));
          if (buf.size() == sizeof(value))
            std::memcpy(&value, buf.data(), buf.size());
          return value;
        }
      case easy::windows::reg_value_kind::string:
      case easy::windows::reg_value_kind::expand_string:
        {
          EASY_ASSERT(buf.size() % 2 == 0);
          std::wstring res;          
          if (buf.size() > 0 && buf.size() % 2 == 0) 
          {
            res.resize(buf.size() / sizeof(wchar_t));
            std::memcpy(&res.front(), buf.data(), buf.size());

            // remove odd terminating nulls
            while (!res.empty() && !res.back())
              res.pop_back();
          }
          return res;
        }
      case easy::windows::reg_value_kind::multi_string:
        {
          EASY_ASSERT(buf.size() % 2 == 0);
          lite_buffer<wchar_t> char_buf(buf.data(), buf.size());
          if (char_buf.empty() || (char_buf.size() == 1 && !char_buf[0]))
            return wstring_list();

          wstring_list res;
          for (auto it = char_buf.cbegin(); it != char_buf.end(); /*blank*/) {
            lite_wstring s = it;
            it = it + s.length() + 1;
            if (it != char_buf.end()) // do not include the last element
              res << s;
          }
          return res;
        }
      case easy::windows::reg_value_kind::binary:
        {
          return std::move(buf);
        }
      }
      return reg_value();          
    }

    bool flush_reg_key(reg_key_handle h, error_code_ref ec)
    {
      if (!check_reg_key_handle(h, ec))
        return false;

      LONG res = ::RegFlushKey(h);
      ec = make_win_error(res);
      return !ec;
    }

    class raw_reg_key_handle_tracker
    {
    public:
      raw_reg_key_handle_tracker(reg_key_handle key)
        : m_key(key) {
      }
      reg_key_handle get() const {
        return m_key;
      }
    private:
      reg_key_handle m_key;
    };

    reg_item_enumerator enum_reg_sub_keys(reg_key_handle h, error_code_ref ec)
    {
      if (!check_reg_key_handle(h, ec))
        return nullptr;
      return std::make_unique<reg_key_enumerator_impl<raw_reg_key_handle_tracker>>(h, ec);
    }

    reg_item_enumerator enum_reg_value_names(reg_key_handle h, error_code_ref ec)
    {
      if (!check_reg_key_handle(h, ec))
        return nullptr;
      return std::make_unique<reg_value_name_enumerator_impl<raw_reg_key_handle_tracker>>(h, ec);
    }

    std::wstring get_reg_key_name(reg_key_handle h, error_code_ref ec)
    {
      if (!check_reg_key_handle(h, ec))
        return std::wstring();

      scoped_dynamic_library lib(L"ntdll.dll", ec);
      if (ec)
        return std::wstring();

      typedef DWORD (__stdcall *NtQueryKeyType)(
        HANDLE  KeyHandle,
        int KeyInformationClass,
        PVOID  KeyInformation,
        ULONG  Length,
        PULONG  ResultLength);

      enum { KeyNameInformation = 3 };

      #ifndef STATUS_BUFFER_TOO_SMALL
      #  define STATUS_BUFFER_TOO_SMALL ((NTSTATUS)0xC0000023L)
      #endif
      #ifndef STATUS_SUCCESS
      #  define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
      #endif

      NtQueryKeyType func = lib.get_proc_address<NtQueryKeyType>("NtQueryKey", ec);
      if (ec)
        return std::wstring();

      ULONG size = 0;
      DWORD res = func(h, KeyNameInformation, nullptr, 0, &size);
      if (res == STATUS_BUFFER_TOO_SMALL) 
      {
        size += 2;
        std::wstring name;        
        name.resize(size / 2);
        res = func(h, KeyNameInformation, &name[0], size, &size);
        if (res == STATUS_SUCCESS)
          if (name.size() > 1)
            name.erase(0, 2); // remove first 2 symbols (size of the string[4 bytes], is used in kernel mode)
          return trim_terminating_zeros(name);
      }
      ec = make_win_error(res);
      return std::wstring();
    }
}

  
  namespace detail
  {
    reg_item_enumerator enum_reg_sub_keys(const scoped_reg_key::tracker& key, error_code_ref ec)
    {
      return std::make_unique<reg_key_enumerator_impl<scoped_reg_key::tracker>>(key, ec);
    }

    reg_item_enumerator enum_reg_sub_keys(const shared_reg_key::tracker& key, error_code_ref ec)
    {
      return std::make_unique<reg_key_enumerator_impl<shared_reg_key::tracker>>(key, ec);
    }

    reg_item_enumerator enum_reg_value_names(const scoped_reg_key::tracker& key, error_code_ref ec)
    {
      return std::make_unique<reg_value_name_enumerator_impl<scoped_reg_key::tracker>>(key, ec);
    }

    reg_item_enumerator enum_reg_value_names(const shared_reg_key::tracker& key, error_code_ref ec)
    {
      return std::make_unique<reg_value_name_enumerator_impl<shared_reg_key::tracker>>(key, ec);
    }

  }


}}