/*!
 *  @file   easy/windows/handle.h
 *  @author Sergey Tararay
 *  @date   2013
 */
#ifndef EASY_WINDOWS_HANDLE_H_INCLUDED
#define EASY_WINDOWS_HANDLE_H_INCLUDED

#include <easy/windows/config.h>

#include <easy/windows/api.h>

#include <easy/safe_bool.h>
#include <easy/stlex/nullptr_t.h>
#include <easy/types.h>
#include <easy/object.h>

#include <memory>
#include <array>

#include <boost/noncopyable.hpp>


namespace easy {
namespace windows
{
//   struct shared_tag { };
//   struct scoped_tag { };

  struct waitable_tag { };

  struct kernel_object_handle_traits
  {
    typedef api::kernel_handle object_type;

    static object_type get_invalid_object() EASY_NOEXCEPT {
      return api::invalid_kernel_handle;
    }
    static bool is_valid(object_type h) EASY_NOEXCEPT {
      return api::is_kernel_handle_valid(h);
    }
    static bool close_object(object_type h, error_code_ref ec = nullptr) {
      return api::close_handle(h, ec);
    }
  };

  template<template<class Traits> class Holder>
  class kernel_handle_impl
    : public Holder<kernel_object_handle_traits> 
  {
  };


  using api::wait_result;
  using api::wait_timeout;

  template<class T>
  wait_result wait(const T& v, error_code_ref ec = nullptr) {
    return api::wait(v.get_object(), ec);
  }

  template<class T>
  wait_result wait_timed(const T& v, wait_timeout timeout, error_code_ref ec = nullptr) {
    return api::wait_timed(v.get_object(), timeout, ec);
  }

  //////////////////////////////////////////////////////////////////////////
  
  using api::event_type;
  using api::event_state;

  template<template<class Traits> class Holder>
  class event_impl
    : public Holder<kernel_object_handle_traits>
    , public waitable_tag
  {
  public:
    typedef api::kernel_handle object_type;

    bool set(error_code_ref ec = nullptr) {
      return api::set_event(get_object(), ec);
    }

    bool reset(error_code_ref ec = nullptr) {
      return api::reset_event(get_object(), ec);
    }
  protected:
    ~event_impl() { }

    static object_type construct(event_type et, event_state es, error_code_ref ec) {
      return api::create_event(et, es, ec);
    }
  };

  //////////////////////////////////////////////////////////////////////////

  using api::process_id;
  using api::process_exit_code;

  template<template<class Traits> class Holder>
  class process_impl
    : public Holder<kernel_object_handle_traits>
    , public waitable_tag
  {
  public:
    typedef api::kernel_handle object_type;

    process_id get_id(error_code_ref ec = nullptr) const {
      return api::get_process_id(get_object(), ec);
    }

    process_exit_code get_exit_code(error_code_ref ec = nullptr) {
      return get_process_exit_code(get_object(), ec);
    }
    
  protected:
    ~process_impl() { }

    static object_type construct(uint pid, error_code_ref ec) {
      return api::open_process(pid, ec);
    }
  };

  //////////////////////////////////////////////////////////////////////////

  struct dynamic_library_handle_traits
  {
    typedef api::dll_handle object_type;
    typedef FARPROC raw_function_type;

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
    typedef dynamic_library_handle_traits::object_type       object_type;
    typedef dynamic_library_handle_traits::raw_function_type raw_function_type;

    std::wstring get_file_path(error_code_ref ec = nullptr) const {
      return api::get_module_file_name(get_object(), ec);
    }

    raw_function_type get_proc_address(const c_string& name, error_code_ref ec = nullptr) const {
      return api::get_library_proc_address(get_object(), name, ec);
    }

    template<class Function>
    Function get_proc_address(const c_string& name, error_code_ref ec = nullptr) const {
      return static_cast<Function>(get_proc_address(name, ec));
    }
    
  protected:
    ~dynamic_library_impl() { }

    static object_type construct(const c_wstring& path, error_code_ref ec) {
      return api::load_library(path, ec);
    }
  };
  //////////////////////////////////////////////////////////////////////////

  typedef basic_object<kernel_handle_impl<scoped_object_holder>>  scoped_handle;
  typedef basic_object<kernel_handle_impl<shared_object_holder>>  shared_handle;

  typedef basic_object<event_impl<scoped_object_holder>> scoped_event;
  typedef basic_object<event_impl<shared_object_holder>> shared_event;

  typedef basic_object<process_impl<scoped_object_holder>> scoped_process;
  typedef basic_object<process_impl<shared_object_holder>> shared_process;

  typedef basic_object<dynamic_library_impl<scoped_object_holder>> scoped_dynamic_library;
  

}}

#endif