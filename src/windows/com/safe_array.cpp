
#include <easy/windows/com/safe_array.h>

namespace easy
{
  namespace windows
  {
    namespace com
    {
      safe_array::safe_array()
        : m_psa(nullptr)
      {

      }

      safe_array::~safe_array()
      {
        if (m_psa) {
          HRESULT hr = ::SafeArrayDestroy(m_psa);
          EASY_ASSERT(SUCCEEDED(hr));
        }
      }

      bool safe_array::operator ! () const
      {
        return !m_psa;
      }

    }
  }
}
