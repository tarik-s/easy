#ifndef EASY_WINDOWS_API_H_INCLUDED
#define EASY_WINDOWS_API_H_INCLUDED

#include <easy/windows/config.h>

#include <Windows.h>

#include <easy/error_code_ref.h>
#include <easy/strings.h>
#include <easy/safe_bool.h>
#include <easy/range.h>


namespace easy {
namespace windows {
namespace api
{
  typedef HANDLE kernel_handle; 

  static const kernel_handle invalid_kernel_handle = nullptr;

  //////////////////////////////////////////////////////////////////////////
  // kernel handle functions

  bool is_valid_kernel_handle(kernel_handle h) EASY_NOEXCEPT;
  bool check_kernel_handle(kernel_handle h, error_code_ref ec = nullptr);
  bool close_handle(kernel_handle h, error_code_ref ec = nullptr);

  // waitables
  class wait_result
    : safe_bool<wait_result>
  {
  public:
    wait_result(DWORD res) EASY_NOEXCEPT
      : m_res(res) {
    }

    int get_index() const  EASY_NOEXCEPT {
      if (is_signalled())
        return m_res - WAIT_OBJECT_0;
      else if (is_abandoned())
        return m_res - WAIT_ABANDONED_0;
      else
        return -1;
    }

    bool is_signalled() const  EASY_NOEXCEPT {
      return is_in_range(m_res, WAIT_OBJECT_0, WAIT_OBJECT_0 + MAXIMUM_WAIT_OBJECTS);
    }

    bool is_abandoned() const  EASY_NOEXCEPT {
      return is_in_range(m_res, WAIT_ABANDONED_0, WAIT_ABANDONED_0 + MAXIMUM_WAIT_OBJECTS);
    }

    bool is_failed() const  EASY_NOEXCEPT {
      return m_res == WAIT_FAILED;
    }

    bool is_timed_out() const  EASY_NOEXCEPT {
      return m_res == WAIT_TIMEOUT;
    }

    bool operator !() const  EASY_NOEXCEPT {
      return !is_signalled();
    }
  private:
    DWORD m_res;
  };

  typedef unsigned wait_timeout;
  static const wait_timeout infinite_wait = INFINITE;

  wait_result wait(kernel_handle h, error_code_ref ec = nullptr);
  wait_result wait_timed(kernel_handle h, wait_timeout timeout, error_code_ref ec = nullptr);

  wait_result wait(kernel_handle* ph, byte count, bool wait_all, error_code_ref ec = nullptr);
  wait_result wait_timed(kernel_handle* ph, byte count, bool wait_all, wait_timeout timeout, error_code_ref ec = nullptr);
  
  
  // event
  enum class event_type  { auto_, manual };
  enum class event_state { set, reset };

  kernel_handle create_event(event_type et, event_state es, error_code_ref ec = nullptr);
  bool set_event(kernel_handle h, error_code_ref ec = nullptr);
  bool reset_event(kernel_handle h, error_code_ref ec = nullptr);

  // process
  typedef DWORD process_id;
  typedef DWORD process_exit_code;

  kernel_handle open_process(unsigned pid, error_code_ref ec = nullptr);
  bool terminate_process(kernel_handle h, process_exit_code code, error_code_ref ec = nullptr);
  process_id get_process_id(kernel_handle h, error_code_ref ec = nullptr);
  process_exit_code get_process_exit_code(kernel_handle h, error_code_ref ec = nullptr);


  //////////////////////////////////////////////////////////////////////////
  // dll functions

  typedef HMODULE dll_handle;
  typedef FARPROC raw_dll_function;

  static const dll_handle invalid_dll_handle = nullptr;

  bool is_valid_dll_handle(dll_handle h) EASY_NOEXCEPT;
  bool check_dll_handle(dll_handle h, error_code_ref ec = nullptr);

  dll_handle load_library(const c_wstring& path, error_code_ref ec = nullptr);
  bool free_library(dll_handle h, error_code_ref ec = nullptr);

  raw_dll_function get_library_proc_address(dll_handle h, const c_string& name, error_code_ref ec = nullptr);
  std::wstring get_module_file_name(dll_handle h, error_code_ref ec = nullptr);


}}}

#endif