
#include <easy/windows/dynamic_library.h>
#include <easy/windows/error.h>

namespace easy 
{
  namespace windows
  {
    namespace api
    {
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

      dll_handle load_library(const lite_wstring& path, error_code_ref ec)
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

      raw_dll_function get_library_proc_address(dll_handle h, const lite_string& name, error_code_ref ec)
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

    }
  }
}