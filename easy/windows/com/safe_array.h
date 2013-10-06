#ifndef EASY_WINDOWS_COM_SAFE_ARRAY_H_INCLUDED
#define EASY_WINDOWS_COM_SAFE_ARRAY_H_INCLUDED

#include <easy/windows/com/config.h>

#include <easy/windows/com/variant_type.h>

#include <easy/flags.h>
#include <easy/safe_bool.h>

#include <Windows.h>


namespace easy
{
  namespace windows
  {
    namespace com
    {
      enum class safe_array_feature
      {
        none         = 0,
        auto_        = FADF_AUTO,
        static_      = FADF_STATIC,
        embedded     = FADF_EMBEDDED,
        fixed_size   = FADF_FIXEDSIZE,
        record       = FADF_RECORD,
        have_id      = FADF_HAVEIID,
        have_vartype = FADF_HAVEVARTYPE,
        bstr         = FADF_BSTR,
        unknown      = FADF_UNKNOWN,
        dispatch     = FADF_DISPATCH,
        variant      = FADF_VARIANT,
        reserved     = FADF_RESERVED
      };
      EASY_DECLARE_AS_FLAGS(safe_array_feature);


      class safe_array
        : public safe_bool<safe_array>
      {
      public:
        safe_array();

        ~safe_array();

        bool operator ! () const;
      private:
        SAFEARRAY* m_psa;
      };

      class safe_array_builder
      {
      public:

      private:
        
      };

    }
  }
}


#endif