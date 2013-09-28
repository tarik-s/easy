/*!
*  @file   easy/windows/registry.h
*  @author Sergey Tararay
*  @date   2013
*/
#ifndef EASY_WINDOWS_REGISTRY_H_INCLUDED
#define EASY_WINDOWS_REGISTRY_H_INCLUDED

#include <easy/windows/config.h>

#include <easy/types.h>
#include <easy/error_code_ref.h>
#include <easy/strings.h>
#include <easy/range.h>
#include <easy/object.h>
#include <easy/flags.h>

#include <boost/filesystem/path.hpp>
#include <boost/variant.hpp>


#include <Windows.h>

namespace easy 
{
  template<>
  struct is_object_handle<HKEY> 
    : boost::true_type {
  };

  namespace windows 
  {
    //! Registry key handle (HKEY)
    typedef HKEY reg_key_handle;

    //! Registry path
    typedef boost::filesystem::path reg_path;

    //! Registry hive. List of the system predefined roots
    enum class reg_hive
    {
      hkey_classes_root                = (int)HKEY_CLASSES_ROOT,
      hkey_current_user                = (int)HKEY_CURRENT_USER,
      hkey_local_machine               = (int)HKEY_LOCAL_MACHINE,
      hkey_users                       = (int)HKEY_USERS,
      hkey_performance_data            = (int)HKEY_PERFORMANCE_DATA,
      hkey_current_config              = (int)HKEY_CURRENT_CONFIG,
      hkey_dyn_data                    = (int)HKEY_DYN_DATA,
      hkcu = hkey_current_user,
      hklm = hkey_local_machine,
    };

    inline reg_key_handle get_object_handle(reg_hive hive) {
      return (reg_key_handle)hive;
    }

    //! Types which can be saved in the registry
    enum class reg_value_kind
    {
      unknown      = -2,
      none         = -1,
      dword        =  0,
      qword,
      string,
      expand_string,
      multi_string,
      binary
    };

    //! Access a key is opened with
    enum class reg_access {
      read  = KEY_READ,
      write = KEY_WRITE,
      all   = KEY_ALL_ACCESS
    };

    //! Open mode
    enum class reg_open_mode 
    {
      open,
      create
    };

    //! Registry virtualization
    enum class reg_virtualization
    {
      enabled,
      disabled
    };

    //! Registry Open/Create compound params
    typedef enum_group<reg_access, reg_open_mode, reg_virtualization> reg_open_params;

    //! Registry value holder type
    typedef boost::variant<
      uint32,
      uint64,
      std::wstring,
      wstring_list,
      byte_vector
    > reg_value;

    //! enumerates over keys and values names
    typedef std::unique_ptr<enumerator<std::wstring>> reg_item_enumerator;

    namespace detail
    {
      template<reg_value_kind Kind>
      struct reg_value_converter
      {
        
        typedef std::wstring out_type;
      };
    }

    namespace api
    {
      static const reg_key_handle invalid_reg_key_handle = nullptr;

      bool is_reg_key_handle_valid(reg_key_handle h) EASY_NOEXCEPT;
      bool check_reg_key_handle(reg_key_handle h, error_code_ref ec = nullptr);

      bool close_reg_key(reg_key_handle h, error_code_ref ec = nullptr);
      bool delete_reg_key(reg_key_handle h, const reg_path& subkey, error_code_ref ec = nullptr);
      bool delete_reg_value(reg_key_handle h, const lite_wstring& name, error_code_ref ec = nullptr);
      bool flush_reg_key(reg_key_handle h, error_code_ref ec = nullptr);

      reg_key_handle create_reg_key(reg_key_handle h, const reg_path& subkey, const reg_open_params& params, error_code_ref ec = nullptr);

      reg_value_kind get_reg_value_kind(reg_key_handle h, const lite_wstring& name, error_code_ref ec = nullptr);

      bool set_reg_value(reg_key_handle h, const lite_wstring& name, reg_value_kind kind, const byte* data_ptr, size_t size, error_code_ref ec = nullptr);
      bool set_reg_value(reg_key_handle h, const lite_wstring& name, reg_value_kind kind, const reg_value& value, error_code_ref ec = nullptr);
      bool set_reg_value(reg_key_handle h, const lite_wstring& name, const reg_value& value, error_code_ref ec = nullptr);
      bool set_reg_value_uint32(reg_key_handle h, const lite_wstring& name, uint32 value, error_code_ref ec = nullptr);
      bool set_reg_value_uint64(reg_key_handle h, const lite_wstring& name, uint64 value, error_code_ref ec = nullptr);
      bool set_reg_value_string(reg_key_handle h, const lite_wstring& name, const lite_wstring& value, error_code_ref ec = nullptr);
      bool set_reg_value_exp_string(reg_key_handle h, const lite_wstring& name, const lite_wstring& value, error_code_ref ec = nullptr);
      
      reg_value get_reg_value(reg_key_handle h, const lite_wstring& name, error_code_ref ec = nullptr);

      reg_item_enumerator enum_reg_sub_keys(reg_key_handle key, error_code_ref ec = nullptr);
      reg_item_enumerator enum_reg_values(reg_key_handle key, error_code_ref ec = nullptr);
    }
    
   
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
      //!
      typedef reg_key_handle object_type;

      //! Deletes subkey
      bool delete_subkey(const reg_path& subkey, error_code_ref ec = nullptr) {
        return api::delete_reg_key(get_object(), subkey, ec);
      }

      //! Deletes value
      bool delete_value(const lite_wstring& name, error_code_ref ec = nullptr) {
        return api::delete_reg_value(get_object(), name, ec);
      }

      //! Get kind of the value
      reg_value_kind get_value_kind(const lite_wstring& name, error_code_ref ec = nullptr) const {
        return api::get_reg_value_kind(get_object(), name, ec);
      }

      //!
      bool set_value(const lite_wstring& name, const reg_value& value, error_code_ref ec = nullptr) {
        return api::set_reg_value(get_object(), name, value, ec);
      }

      //!
      bool set_value(const lite_wstring& name, const reg_value& value, reg_value_kind kind, error_code_ref ec = nullptr) {
        return api::set_reg_value(get_object(), name, kind, value, ec);
      }

      //!
      reg_value get_value(const lite_wstring& name, error_code_ref ec = nullptr) const {
        return api::get_reg_value(get_object(), name, ec);
      }

      //!
      template<reg_value_kind Kind>
      typename detail::reg_value_converter<
        Kind
      >::out_type get_value_as(const lite_wstring& name, error_code_ref ec = nullptr) const
      {
        reg_value val = get_value(name, ec);
        if (ec)
          return nullptr;

        return nullptr;
      }

      //!
      reg_item_enumerator enum_sub_keys(error_code_ref ec = nullptr) const {
        return detail::enum_reg_sub_keys(*this, ec);
      }

      //!
      reg_item_enumerator enum_values(error_code_ref ec = nullptr) const {
        return detail::enum_reg_values(*this, ec);
      }

    protected:
      ~reg_key_impl() { }

      template<class RegKey>
      static object_type construct(const RegKey& k, const reg_path& subkey, const reg_open_params& params, error_code_ref ec) {
        reg_key_handle h = get_object_handle(k);
        return api::create_reg_key(h, subkey, params, ec);
      }

      template<class RegKey>
      static object_type construct(const RegKey& k, const reg_path& subkey, error_code_ref ec) {
        return construct(k, subkey, nullptr, ec);
      }

      template<class RegKey>
      static object_type construct(const RegKey& k, error_code_ref ec) {
        return construct(k, reg_path(), nullptr, ec);
      }
    };

    typedef basic_object<reg_key_impl<scoped_object_holder>> scoped_reg_key;
    typedef basic_object<reg_key_impl<shared_object_holder>> shared_reg_key;

    namespace detail
    {
      reg_item_enumerator enum_reg_sub_keys(const scoped_reg_key::tracker& key, error_code_ref ec = nullptr);
      reg_item_enumerator enum_reg_sub_keys(const shared_reg_key::tracker& key, error_code_ref ec = nullptr);
      reg_item_enumerator enum_reg_values(const scoped_reg_key::tracker& key, error_code_ref ec = nullptr);
      reg_item_enumerator enum_reg_values(const shared_reg_key::tracker& key, error_code_ref ec = nullptr);
    }

  }
}

#endif
