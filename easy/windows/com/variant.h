#ifndef EASY_WINDOWS_COM_VARIANT_H_INCLUDED
#define EASY_WINDOWS_COM_VARIANT_H_INCLUDED

#include <easy/windows/com/config.h>
#include <easy/safe_bool.h>

#include <easy/windows/com/variant_type.h>

#include <Windows.h>

namespace easy
{
  namespace windows
  {
    namespace com
    {

      //! com_variant
      class com_variant
        : public safe_bool<com_variant>
      {
      public:
        //!
        com_variant();

        //!
        com_variant(const com_variant& r);

        //!
        com_variant(com_variant && r);

        //!
        ~com_variant();

        variant_type type() const EASY_NOEXCEPT;

        void swap(com_variant& r) EASY_NOEXCEPT;

        bool operator ! () const EASY_NOEXCEPT;

      private:

        void init();

      private:
        VARIANT m_var;
      };
    }
  }
}

#endif