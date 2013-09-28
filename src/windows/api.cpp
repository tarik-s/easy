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



}}}
