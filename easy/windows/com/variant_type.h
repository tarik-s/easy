#ifndef EASY_WINDOWS_COM_VARIANT_TYPE_H_INCLUDED
#define EASY_WINDOWS_COM_VARIANT_TYPE_H_INCLUDED

#include <easy/windows/com/config.h>

#include <Windows.h>

namespace easy
{
  namespace windows
  {
    namespace com
    {

      //! variant_type
      enum class variant_type
      {
        empty      = VT_EMPTY,
        null_      = VT_NULL,

        bool_      = VT_BOOL,
        pointer    = VT_PTR,
        void_      = VT_VOID,

        int1       = VT_I1,
        int2       = VT_I2,
        int4       = VT_I4,
        int8       = VT_I8,
        int_       = VT_INT,
        int_ptr    = VT_INT_PTR,
        
        uint1      = VT_UI1,
        uint2      = VT_UI2,
        uint4      = VT_UI4,
        uint8      = VT_UI8,
        uint       = VT_UINT,
        uint_ptr   = VT_UINT_PTR,

        real4      = VT_R4,
        real8      = VT_R8,

        decimal    = VT_DECIMAL,
        currency   = VT_CY,

        str        = VT_LPSTR,
        wstr       = VT_LPWSTR,
        bstr       = VT_BSTR,

        date       = VT_DATE,
        filetime   = VT_FILETIME,

        error      = VT_ERROR,

        variant    = VT_VARIANT,
        unknown    = VT_UNKNOWN,
        dispatch   = VT_DISPATCH,
        hresult    = VT_HRESULT,

        safe_array = VT_SAFEARRAY,
        carray     = VT_CARRAY,

        user_defined = VT_USERDEFINED,
        record       = VT_RECORD,

        blob         = VT_BLOB,
        stream       = VT_STREAM,
        storage      = VT_STORAGE,

        streamed_object = VT_STREAMED_OBJECT,
        stored_object   = VT_STORED_OBJECT,
        blob_object     = VT_BLOB_OBJECT,

        clipboard_format = VT_CF,
        class_id         = VT_CLSID,
        versioned_stream = VT_VERSIONED_STREAM,

        vector           = VT_VECTOR,
        array            = VT_ARRAY,
        by_ref           = VT_BYREF
      };



    }
  }
}

#endif