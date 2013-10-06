
#include <easy/windows/com/variant.h>

namespace easy
{
  namespace windows
  {
    namespace com
    {
      com_variant::com_variant()
      {
        init();
      }

      com_variant::com_variant(const com_variant& r)
      {
        init();
        HRESULT hr = ::VariantCopy(&m_var, &r.m_var);
      }

      com_variant::com_variant(com_variant && r)
      {
        init();
        r.swap(*this);
      }

      bool com_variant::operator ! () const
      {
        return m_var.vt == VT_EMPTY;
      }

      com_variant::~com_variant()
      {
        ::VariantClear(&m_var);
      }

      void com_variant::swap(com_variant& r)
      {
        std::swap(m_var, r.m_var);
      }

      void com_variant::init()
      {
        std::memset(&m_var, 0, sizeof(VARIANT));
        ::VariantInit(&m_var);

      }

      variant_type com_variant::type() const
      {
        return (variant_type)m_var.vt;
      }

    }
  }
}