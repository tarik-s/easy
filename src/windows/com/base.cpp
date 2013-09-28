#include <easy/windows/com/base.h>

namespace easy
{
  namespace windows
  {
    namespace com
    {
      scoped_com_initializer::scoped_com_initializer(threading_model model)
        : m_model(model)
        , m_hr(E_FAIL)
      {
        if (m_model == threading_model::free)
          m_hr = ::CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        else if (m_model == threading_model::apartment)
          m_hr = ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        else if (m_model == threading_model::ole)
          m_hr = ::OleInitialize(nullptr);
      }

      scoped_com_initializer::~scoped_com_initializer()
      {
        if (SUCCEEDED(m_hr)) {
          if (m_model == threading_model::ole)
            ::OleUninitialize();
          else
            ::CoUninitialize();
        }
      }

      bool scoped_com_initializer::operator ! () const
      {
        return FAILED(m_hr);
      }

    }
  }
}