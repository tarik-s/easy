#include <easy/windows/api.h>
#include <easy/windows/error.h>
#include <vector>

#ifdef max
#  undef max
#endif

namespace easy {
namespace windows {
namespace api 
{

  /*
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
  */

  std::wstring get_environment_variable(const c_wstring& name, error_code_ref ec)
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

  //////////////////////////////////////////////////////////////////////////

  bool is_kernel_handle_valid(kernel_handle h)
  {
    return h != invalid_kernel_handle && h != INVALID_HANDLE_VALUE;
  }

  bool check_kernel_handle(kernel_handle h, error_code_ref ec)
  {
    if (!is_kernel_handle_valid(h)) {
      ec = make_win_error(ERROR_INVALID_HANDLE);
      return false;
    }
    return true;
  }

  bool close_handle(kernel_handle h, error_code_ref ec)
  {
    if (is_kernel_handle_valid(h)) {
      if (!::CloseHandle(h))
        ec = make_last_win_error();
    }
    return !ec;
  }

  kernel_handle create_event(event_type et, event_state es, error_code_ref ec)
  {
    const LPSECURITY_ATTRIBUTES sa = nullptr;
    const BOOL manual_reset = (et == event_type::manual);
    const BOOL initial_state = (es == event_state::set);
    const LPCWSTR pname = /*name ? name.c_str() :*/ nullptr;
    kernel_handle h = ::CreateEventW(sa, manual_reset, initial_state, pname);
    auto error_code = make_last_win_error();
    if (h) {
      if (error_code.value() == ERROR_ALREADY_EXISTS && !ec.is_throwable())
        ec = error_code;
    }
    else
      ec = error_code;      
    return h;
  }

  bool set_event(kernel_handle h, error_code_ref ec)
  {
    if (check_kernel_handle(h, ec))
      return false;

    if (!::SetEvent(h)) {
      ec = make_last_win_error();
      return false;
    }
    return true;
  }

  bool reset_event(kernel_handle h, error_code_ref ec)
  {
    if (check_kernel_handle(h, ec))
      return false;

    if (!::ResetEvent(h)) {
      ec = make_last_win_error();
      return false;
    }
    return true;
  }

  kernel_handle open_process(unsigned pid, error_code_ref ec)
  {
    auto res = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!res) {
      ec = make_last_win_error();
    }
    return res;
  }
  
  bool terminate_process(kernel_handle h, process_exit_code code, error_code_ref ec)
  {
    if (check_kernel_handle(h, ec)) {
      UINT exit_code = code;
      BOOL res = ::TerminateProcess(h, exit_code);
      if (res)
        return true;
      ec = make_last_win_error();
    }
    return false;
  }
  
  process_id get_process_id(kernel_handle h, error_code_ref ec)
  {
    DWORD id = 0;
    if (check_kernel_handle(h, ec)) {
      id = ::GetProcessId(h);
      ec = make_last_win_error();
    }
    return id;
  }

  process_exit_code get_process_exit_code(kernel_handle h, error_code_ref ec)
  {
    DWORD code = std::numeric_limits<DWORD>::max();
    if (api::check_kernel_handle(h, ec)) {
      if (!::GetExitCodeProcess(h, &code))
        ec = make_last_win_error();
      else if (code == STILL_ACTIVE && !ec.is_throwable())
        ec = make_win_error(STILL_ACTIVE);
    }
    return code;   
  }
  
  //////////////////////////////////////////////////////////////////////////

  bool is_dll_handle_valid(dll_handle h)
  {
    return h != invalid_dll_handle;
  }

  bool check_dll_handle(dll_handle h, error_code_ref ec)
  {
    if (!is_dll_handle_valid(h)) {
      ec = make_win_error(ERROR_INVALID_HANDLE);
      return false;
    }
    return true;
  }

  dll_handle load_library(const c_wstring& path, error_code_ref ec)
  {
    dll_handle h = ::LoadLibraryW(path.c_str());
    if (!is_dll_handle_valid(h))
      ec = make_last_win_error();
    return h;
  }

  bool free_library(dll_handle h, error_code_ref ec)
  {
    if (is_dll_handle_valid(h)) {
      if (!::FreeLibrary(h)) {
        ec = make_last_win_error();
        return false;
      }
    }
    return true;
  }

  raw_dll_function get_library_proc_address(dll_handle h, const c_string& name, error_code_ref ec)
  {
    raw_dll_function addr = nullptr;
    if (api::check_dll_handle(h, ec)) {
      addr = ::GetProcAddress(h, name.c_str());
      if (!addr)
        ec = make_last_win_error();
    }
    return addr;
  }

  std::wstring get_module_file_name(dll_handle h, error_code_ref ec)
  {
    if (api::check_dll_handle(h, ec)) {
      std::vector<wchar_t> buffer;
      buffer.resize(256);
      if (::GetModuleFileNameW(h, &buffer[0], buffer.size())) {
        return std::wstring(buffer.data());
      }
    }
    return std::wstring();    
  }

  wait_result wait(kernel_handle h, error_code_ref ec)
  {
    return wait_timed(h, infinite_wait, ec);
  }

  wait_result wait(kernel_handle* ph, byte count, bool wait_all, error_code_ref ec)
  {
    return wait_timed(ph, count, wait_all, infinite_wait, ec);
  }

  wait_result wait_timed(kernel_handle h, wait_timeout timeout, error_code_ref ec)
  {
    DWORD res = ::WaitForSingleObject(h, timeout);
    if (res == WAIT_FAILED)
      ec = make_last_win_error();
    return res;
  }

  easy::windows::api::wait_result wait_timed(kernel_handle* ph, byte count, bool wait_all, wait_timeout timeout, error_code_ref ec)
  {
    DWORD res = ::WaitForMultipleObjects(count, ph, wait_all, timeout);
    if (res == WAIT_FAILED)
      ec = make_last_win_error();
    return res;
  }

  //////////////////////////////////////////////////////////////////////////

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

  bool delete_reg_value(reg_key_handle h, const c_wstring& name, error_code_ref ec)
  {
    if (!check_reg_key_handle(h, ec))
      return false;

    LONG res = ::RegDeleteValueW(h, name.c_str());
    ec = make_win_error(res);
    return !ec;
  }

  reg_key_handle create_reg_key(reg_key_handle h, const reg_path& subkey, reg_open_mode mode, 
    reg_access access, reg_virtualization virt, error_code_ref ec)
  {
    if (!check_reg_key_handle(h, ec))
      return invalid_reg_key_handle;
    
    // normalize registry path
    reg_path s(subkey);
    s.make_preferred();

    DWORD disposition = 0;
    reg_key_handle key = invalid_reg_key_handle;

    REGSAM sam = (REGSAM)access;
    if (virt == reg_virtualization::enabled)
      sam |= KEY_WOW64_64KEY;

    LONG res = (mode == reg_open_mode::open) 
      ? ::RegOpenKeyExW(h, s.c_str(), 0, sam, &key)
      : ::RegCreateKeyExW(h, s.c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE, sam, nullptr, &key, &disposition);

    ec = make_win_error(res);
    return !ec ? key : invalid_reg_key_handle;
  }

  namespace
  {
    reg_value_type reg_value_type_from_winapi_type(DWORD type) {
      switch (type)
      {
      case REG_NONE      : return reg_value_type::none;
      case REG_SZ        : return reg_value_type::string;
      case REG_EXPAND_SZ : return reg_value_type::expand_string;
      case REG_MULTI_SZ  : return reg_value_type::multi_string;
      case REG_BINARY    : return reg_value_type::binary;
      case REG_DWORD     : return reg_value_type::dword;
      case REG_QWORD     : return reg_value_type::qword;
      }
      return reg_value_type::unknown;
    }

    DWORD winapi_type_from_reg_value_type(reg_value_type type) {
      switch (type)
      {
      case reg_value_type::none          : return REG_NONE;
      case reg_value_type::string        : return REG_SZ;
      case reg_value_type::expand_string : return REG_EXPAND_SZ;
      case reg_value_type::multi_string  : return REG_MULTI_SZ;
      case reg_value_type::binary        : return REG_BINARY;
      case reg_value_type::dword         : return REG_DWORD;
      case reg_value_type::qword         : return REG_QWORD;
      }
      return REG_NONE;
    }

    DWORD get_reg_value_type_internal(reg_key_handle h, const c_wstring& name, error_code_ref ec)
    {
      DWORD type = REG_NONE;
      if (!check_reg_key_handle(h, ec))
        return type;
      
      LONG res = ::RegQueryValueEx(h, name.c_str(), nullptr, &type, nullptr, nullptr);
      ec = make_win_error(res);
      return type;
    }
  
  }

  reg_value_type get_reg_value_type(reg_key_handle h, const c_wstring& name, error_code_ref ec)
  {
    DWORD type = get_reg_value_type_internal(h, name, ec);
    return reg_value_type_from_winapi_type(type);
  }

  bool set_reg_value(reg_key_handle h, const c_wstring& name, reg_value_type type, const byte* data_ptr, size_t size, error_code_ref ec)
  {
    if (!check_reg_key_handle(h, ec))
      return false;

    DWORD valtype = winapi_type_from_reg_value_type(type);
    if (valtype == REG_NONE) {
      ec = generic_error::invalid_value;
      return false;
    }

    LONG res = ::RegSetValueEx(h, name.c_str(), 0, valtype, data_ptr, size);
    ec = make_win_error(res);
    return !ec;
  }

  bool set_reg_value_uint32(reg_key_handle h, const c_wstring& name, uint32 value, error_code_ref ec)
  {
    DWORD v = value;
    return set_reg_value(h, name, reg_value_type::dword, (const byte*)&v, sizeof(v), ec);
  }

  bool set_reg_value_uint64(reg_key_handle h, const c_wstring& name, uint64 value, error_code_ref ec)
  {
    ULONGLONG v = value;
    return set_reg_value(h, name, reg_value_type::qword, (const byte*)&v, sizeof(v), ec);
  }

  bool set_reg_value_string(reg_key_handle h, const c_wstring& name, const c_wstring& value, error_code_ref ec)
  {
    return set_reg_value(h, name, reg_value_type::string, (const byte*)value.c_str(), value.size() * sizeof(c_wstring::value_type), ec);
  }

  bool set_reg_value_exp_string(reg_key_handle h, const c_wstring& name, const c_wstring& value, error_code_ref ec)
  {
    return set_reg_value(h, name, reg_value_type::expand_string, (const byte*)value.c_str(), value.size() * sizeof(c_wstring::value_type), ec);
  }




}}}
