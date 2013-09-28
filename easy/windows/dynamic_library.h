#ifndef EASY_WINDOWS_DYNAMIC_LIBRARY_H_INCLUDED
#define EASY_WINDOWS_DYNAMIC_LIBRARY_H_INCLUDED

#include <easy/windows/config.h>

#include <easy/types.h>
#include <easy/error_handling.h>
#include <easy/strings.h>
#include <easy/range.h>
#include <easy/object.h>
#include <easy/flags.h>

#include <boost/filesystem/path.hpp>

#include <Windows.h>

namespace easy
{
  namespace windows
  {
    //! dll handle
    typedef HMODULE dll_handle;

    //! 
    typedef FARPROC raw_dll_function;

    namespace api
    {
      static const dll_handle invalid_dll_handle = nullptr;

      bool is_dll_handle_valid(dll_handle h) EASY_NOEXCEPT;
      bool check_dll_handle(dll_handle h, error_code_ref ec = nullptr);

      dll_handle load_library(const lite_wstring& path, error_code_ref ec = nullptr);
      bool free_library(dll_handle h, error_code_ref ec = nullptr);

      raw_dll_function get_library_proc_address(dll_handle h, const lite_string& name, error_code_ref ec = nullptr);
      std::wstring get_module_file_name(dll_handle h, error_code_ref ec = nullptr);
    }

    
    struct dynamic_library_handle_traits
    {
      typedef dll_handle object_type;

      static object_type get_invalid_object() {
        return api::invalid_dll_handle;
      }

      static bool is_valid(object_type obj) EASY_NOEXCEPT {
        return api::is_dll_handle_valid(obj);
      }

      static bool close_object(object_type obj, error_code_ref ec = nullptr) {
        return api::free_library(obj, ec);
      }
    };

    template<template<class Traits> class Holder>
    class dynamic_library_impl
      : public Holder<dynamic_library_handle_traits>
    {
    public:
      typedef dll_handle object_type;

      std::wstring get_file_path(error_code_ref ec = nullptr) const {
        return api::get_module_file_name(get_object(), ec);
      }

      raw_dll_function get_proc_address(const lite_string& name, error_code_ref ec = nullptr) const {
        return api::get_library_proc_address(get_object(), name, ec);
      }

      template<class Function>
      Function get_proc_address(const lite_string& name, error_code_ref ec = nullptr) const {
        return reinterpret_cast<Function>(get_proc_address(name, ec));
      }

    protected:
      ~dynamic_library_impl() { }

      static object_type construct(const lite_wstring& path, error_code_ref ec) {
        return api::load_library(path, ec);
      }
    };

    typedef basic_object<dynamic_library_impl<scoped_object_holder>> scoped_dynamic_library;
    typedef basic_object<dynamic_library_impl<shared_object_holder>> shared_dynamic_library;


  }
}

#endif