#ifndef EASY_WINDOWS_COM_BASE_H_INCLUDED
#define EASY_WINDOWS_COM_BASE_H_INCLUDED

#include <easy/windows/com/config.h>

#include <easy/safe_bool.h>
#include <easy/error_handling.h>

#include <boost/noncopyable.hpp>

#include <Windows.h>

namespace easy
{
  namespace windows
  {
    namespace com
    {
      //!
      enum class threading_model
      {
        free,      //!< free-threaded
        apartment, //!< apartment-threaded
        ole        //!< OLE
      };

      //!
      class scoped_com_initializer
        : public easy::safe_bool<scoped_com_initializer>
        , boost::noncopyable
      {
      public:
        explicit scoped_com_initializer(threading_model model);
        ~scoped_com_initializer();

        bool operator ! () const;
      private:
        threading_model m_model;
        HRESULT m_hr;
      };


    }
  }
}


#endif