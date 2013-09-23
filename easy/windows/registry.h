/*!
 *  @file   easy/windows/registry.h
 *  @author Sergey Tararay
 *  @date   2013
 */
#ifndef EASY_WINDOWS_REGISTRY_H_INCLUDED
#define EASY_WINDOWS_REGISTRY_H_INCLUDED

#include <easy/windows/config.h>

#include <easy/windows/api/registry_api.h>

#include <easy/types.h>
#include <easy/error_code_ref.h>
#include <easy/strings.h>
#include <easy/range.h>
#include <easy/object.h>

#include <boost/variant.hpp>


namespace easy {
namespace windows 
{



  class reg_value
  {
  public:
    const std::wstring& get_name() const {
      return m_name;
    }
    const reg_value_holder& get_value() const {
      return m_value;
    }
  private:
    std::wstring m_name;
    reg_value_holder m_value;
  };

  typedef std::unique_ptr<enumerable<std::wstring>> reg_key_enumerator;
  typedef std::unique_ptr<enumerable<reg_value>> reg_value_enumerator;

  // 
  struct reg_key_handle_traits
  {
    typedef reg_key_handle object_type;

    static object_type get_invalid_object() EASY_NOEXCEPT {
      return api::invalid_reg_key_handle;
    }
    static bool is_valid(object_type h) EASY_NOEXCEPT {
      return api::is_reg_key_handle_valid(h);
    }
    static bool close_object(object_type h, error_code_ref ec = nullptr) {
      return api::close_reg_key(h, ec);
    }
  };

  template<template<class Traits> class Holder>
  class reg_key_impl
    : public Holder<reg_key_handle_traits>
  {
  public:
    typedef reg_key_handle object_type;

    bool delete_subkey(const reg_path& subkey, error_code_ref ec = nullptr) {
      return api::delete_reg_key(get_object(), subkey, ec);
    }

    bool delete_value(const c_wstring& name, error_code_ref ec = nullptr) {
      return api::delete_reg_value(get_object(), name, ec);
    }

    reg_value_type get_value_type(const c_wstring& name, error_code_ref ec = nullptr) const {
      return api::get_reg_value_type(get_object(), name, ec);
    }

    template<class Value>
    bool set_value(const c_wstring& name, const Value& value, error_code_ref ec = nullptr) {
      return false;
    }

    reg_value get_value(const c_wstring& name, error_code_ref ec = nullptr) const {
      return reg_value();
    }

    reg_key_enumerator enum_keys(error_code_ref ec = nullptr) const {
      return factory::enum_reg_keys(*this, ec);
    }

    reg_value_enumerator enum_values(error_code_ref ec = nullptr) {
      return nullptr;
    }

  protected:
    ~reg_key_impl() { }

    static object_type construct(reg_key_handle h, const reg_path& subkey, reg_open_mode mode, reg_access access, error_code_ref ec) {
      return api::create_reg_key(h, subkey, mode, access, reg_virtualization::enabled, ec);
    }

    static object_type construct(reg_hive hive, const reg_path& subkey, reg_open_mode mode, reg_access access, error_code_ref ec) {
      return construct((reg_key_handle)hive, subkey, mode, access, ec);
    }

    static object_type construct(reg_hive hive, const reg_path& subkey, error_code_ref ec) {
      return construct(hive, subkey, reg_open_mode::open, reg_access::read, ec);
    }
  };

  typedef basic_object<reg_key_impl<scoped_object_holder>> scoped_reg_key;
  typedef basic_object<reg_key_impl<shared_object_holder>> shared_reg_key;

  namespace factory
  {
    reg_key_enumerator enum_reg_keys(const scoped_reg_key::tracker& key, error_code_ref ec = nullptr);
    reg_key_enumerator enum_reg_keys(const shared_reg_key::tracker& key, error_code_ref ec = nullptr);
  }

}}

#endif
