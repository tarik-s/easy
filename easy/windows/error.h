/*!
*  @file   easy/windows/error.h
*  @author Sergey Tararay
*  @date   2013
*/
#ifndef EASY_WINDOWS_ERROR_H_INCLUDED
#define EASY_WINDOWS_ERROR_H_INCLUDED

#include <easy/windows/config.h>

#include <easy/error_handling.h>

#include <Windows.h>

namespace easy 
{
  namespace windows 
  {
    /*!
    * Creates system specific error_code from @b DWORD returned by @b GetLastError or other API functions
    * @sa make_last_win_error
    */ 
    inline error_code make_win_error(DWORD code) {
      return error_code(code, boost::system::system_category());
    }

    /*! 
    * Creates system specific error_code using last windows error
    * @sa make_win_error
    */
    inline error_code make_last_win_error() {
      return make_win_error(::GetLastError());
    }


  }}

#endif