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

#include <memory>
#include <array>

#include <boost/noncopyable.hpp>


namespace easy {
namespace windows
{
  struct shared_tag { };
  struct scoped_tag { };
  struct waitable_tag { };
  //////////////////////////////////////////////////////////////////////////

  template<class Traits>
  class scoped_handle_holder
    : protected Traits
    , boost::noncopyable
    , public scoped_tag
  {
  public:
    typedef Traits traits_type;
    typedef typename traits_type::handle_type handle_type;  
    
    explicit scoped_handle_holder(handle_type h = traits_type::get_invalid_handle()) EASY_NOEXCEPT
      : m_handle(h) {
    }

    ~scoped_handle_holder() EASY_NOEXCEPT {
      reset_handle();
    }

    handle_type get_handle() const EASY_NOEXCEPT {
      return m_handle;
    }

    handle_type release_handle() EASY_NOEXCEPT {
      handle_type h = m_handle;
      m_handle = traits_type::get_invalid_handle();
      return h;
    }

    void reset_handle(handle_type h = traits_type::get_invalid_handle()) EASY_NOEXCEPT {
      if (h != m_handle) {
        error_code ec;
        traits_type::close_handle(m_handle, ec);
        EASY_ASSERT(!ec);
        m_handle = h;
      }
    }

    bool operator ! () const EASY_NOEXCEPT {
      return !traits_type::is_valid(m_handle);
    }
  protected:
    void swap_impl(scoped_handle_holder& r) EASY_NOEXCEPT {
      std::swap(m_handle, r.m_handle);
    }
  private:
    handle_type m_handle;
  };

  //////////////////////////////////////////////////////////////////////////

  template<class Traits>
  class shared_handle_holder
    : protected Traits
    , public shared_tag
  {
  public:
    typedef Traits traits_type;
    typedef typename traits_type::handle_type handle_type;  

    explicit shared_handle_holder(handle_type h = traits_type::get_invalid_handle())
    {
      if (is_valid(h))
        m_impl = std::make_shared<holder_type>(h);
    }

    handle_type get_handle() const EASY_NOEXCEPT {
      return m_impl ? m_impl->get_handle() : traits_type::get_invalid_handle();
    }

    void reset_handle(handle_type h = traits_type::get_invalid_handle()) {
      if (h != get_handle()) {
        if (m_impl)
          m_impl->reset_handle(h);
        else
          *this = shared_handle_holder(h);
      }
    }

    bool operator ! () const EASY_NOEXCEPT {
      return !m_impl || !*m_impl;
    }
  protected:
    void swap_impl(shared_handle_holder& r) EASY_NOEXCEPT {
      m_impl.swap(r);
    }
  private:
    typedef scoped_handle_holder<traits_type> holder_type;
    typedef std::shared_ptr<holder_type> holder_ptr;
  private:
    holder_ptr m_impl;
  };
  
  //////////////////////////////////////////////////////////////////////////

  template<class Impl>
  class basic_handle
    : public Impl
    , public safe_bool<basic_handle<Impl>>
  {
  public:
    typedef Impl                              base;
    typedef basic_handle<Impl>                this_type;   
    typedef typename base::traits_type        traits_type;
    typedef typename traits_type::handle_type handle_type; 

  public:
    basic_handle() EASY_NOEXCEPT {
    }

    basic_handle(nullptr_t) EASY_NOEXCEPT {
    }

    explicit basic_handle(handle_type h)
    {
      reset_handle(h);
    }

    template<class A1>
    basic_handle(A1 && a1, error_code_ref ec = nullptr)
    {
      reset_handle(base::construct(
        std::forward<A1>(a1), 
        ec));
    }

    template<class A1, class A2>
    basic_handle(A1 && a1, A2 && a2, error_code_ref ec = nullptr)
    {
      reset_handle(base::construct(
        std::forward<A1>(a1),
        std::forward<A2>(a2),
        ec));
    }

    template<class A1, class A2, class A3>
    basic_handle(A1 && a1, A2 && a2, A3 && a3, error_code_ref ec = nullptr)
    {
      reset_handle(base::construct(
        std::forward<A1>(a1),
        std::forward<A2>(a2),
        std::forward<A3>(a3),
        ec));
    }

    basic_handle(this_type && r) EASY_NOEXCEPT
    {
      *this = std::forward<this_type>(r);
    }

    basic_handle& operator = (this_type && r) EASY_NOEXCEPT {
      if (this != &r)
        swap(r);
      return *this;
    }

    basic_handle& operator = (nullptr_t) EASY_NOEXCEPT {
      reset_handle();
      return *this;
    }

    void swap(this_type& r) EASY_NOEXCEPT {
      swap_impl(r);
    }
  };

  //////////////////////////////////////////////////////////////////////////

  struct kernel_object_handle_traits
  {
    typedef api::kernel_handle handle_type;

    static handle_type get_invalid_handle() EASY_NOEXCEPT {
      return api::invalid_kernel_handle;
    }
    static bool is_valid(handle_type h) EASY_NOEXCEPT {
      return api::is_valid_kernel_handle(h);
    }
    static bool close_handle(handle_type h, error_code_ref ec = nullptr) {
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
    return api::wait(v.get_handle(), ec);
  }

  template<class T>
  wait_result wait_timed(const T& v, wait_timeout timeout, error_code_ref ec = nullptr) {
    return api::wait_timed(v.get_handle(), timeout, ec);
  }

  //////////////////////////////////////////////////////////////////////////
  
  using api::event_type;
  using api::event_state;

  template<template<class Traits> class Holder>
  class event_impl
    : public Holder<kernel_object_handle_traits>
    , waitable_tag
  {
  public:
    typedef api::kernel_handle handle_type;

    bool set(error_code_ref ec = nullptr) {
      return api::set_event(get_handle(), ec);
    }

    bool reset(error_code_ref ec = nullptr) {
      return api::reset_event(get_handle(), ec);
    }
  protected:
    ~event_impl() { }

    static handle_type construct(event_type et, event_state es, error_code_ref ec) {
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
    typedef api::kernel_handle handle_type;

    process_id get_id(error_code_ref ec = nullptr) const {
      return api::get_process_id(get_handle(), ec);
    }

    process_exit_code get_exit_code(error_code_ref ec = nullptr) {
      return get_process_exit_code(get_handle(), ec);
    }
    
  protected:
    ~process_impl() { }

    static handle_type construct(uint pid, error_code_ref ec) {
      return api::open_process(pid, ec);
    }
  };

  //////////////////////////////////////////////////////////////////////////

  struct dynamic_library_handle_traits
  {
    typedef api::dll_handle handle_type;
    typedef FARPROC raw_function_type;

    static const handle_type get_invalid_handle() {
      return api::invalid_dll_handle;
    }

    static bool is_valid(handle_type h) EASY_NOEXCEPT {
      return api::is_valid_dll_handle(h);
    }

    static bool close_handle(handle_type h, error_code_ref ec = nullptr) {
      return api::free_library(h, ec);
    }
  };

  template<template<class Traits> class Holder>
  class dynamic_library_impl
    : public Holder<dynamic_library_handle_traits>
  {
  public:
    typedef dynamic_library_handle_traits::handle_type       handle_type;
    typedef dynamic_library_handle_traits::raw_function_type raw_function_type;

    std::wstring get_file_path(error_code_ref ec = nullptr) const {
      return api::get_module_file_name(get_handle(), ec);
    }

    raw_function_type get_proc_address(const c_string& name, error_code_ref ec = nullptr) const {
      return api::get_library_proc_address(get_handle(), name, ec);
    }

    template<class Function>
    Function get_proc_address(const c_string& name, error_code_ref ec = nullptr) const {
      return static_cast<Function>(get_proc_address(name, ec));
    }
    
  protected:
    ~dynamic_library_impl() { }

    static handle_type construct(const c_wstring& path, error_code_ref ec) {
      return api::load_library(path, ec);
    }
  };
  //////////////////////////////////////////////////////////////////////////

  typedef basic_handle<kernel_handle_impl<scoped_handle_holder>>  scoped_handle;
  typedef basic_handle<kernel_handle_impl<shared_handle_holder>>  shared_handle;

  typedef basic_handle<event_impl<scoped_handle_holder>> scoped_event;
  typedef basic_handle<event_impl<shared_handle_holder>> shared_event;

  typedef basic_handle<process_impl<scoped_handle_holder>> scoped_process;
  typedef basic_handle<process_impl<shared_handle_holder>> shared_process;

  typedef basic_handle<dynamic_library_impl<scoped_handle_holder>> scoped_dynamic_library;
  

}}

#endif