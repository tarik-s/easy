#include <easy/windows/synchronization.h>

namespace easy {
namespace windows
{
  critical_section::critical_section()
  {
    ::InitializeCriticalSection(&m_cs);
  }

  critical_section::~critical_section()
  {
    ::DeleteCriticalSection(&m_cs);
  }

  void critical_section::lock()
  {
    ::EnterCriticalSection(&m_cs);
  }

  void critical_section::unlock()
  {
    ::LeaveCriticalSection(&m_cs);
  }



}}