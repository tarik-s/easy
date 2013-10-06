/*!
*  @file   easy/windows/registry.h
*  @author Sergey Tararay
*  @date   2013
*/
#ifndef EASY_WINDOWS_REGISTRY_H_INCLUDED
#define EASY_WINDOWS_REGISTRY_H_INCLUDED

#include <easy/windows/config.h>

#include <easy/types.h>
#include <easy/error_handling.h>
#include <easy/strings.h>
#include <easy/lite_buffer.h>
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

    //! Specifies the data types to use when storing values in the registry, or identifies the data type of a value in the registry.
    enum class reg_value_kind
    {
      unknown      = -2,
      none         = -1,
      dword        =  0,
      qword,
      string,
      multi_string,
      binary,
      expand_string
    };

    //! Specifies optional behavior when retrieving name/value pairs from a registry key.
    enum class reg_value_options
    {
      none,
      do_not_expand_environment_names
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
      uint32,                 // dword  = 0
      uint64,                 // qword  = 1
      std::wstring,           // string = 2
      wstring_list,           // multi_string = 3
      byte_vector             // binary = 4
    > reg_value;

    //! enumerates over keys and values names
    typedef std::unique_ptr<enumerator<std::wstring>> reg_item_enumerator;

    namespace detail
    {
      template<reg_value_kind Kind>
      struct kind_ 
        : std::integral_constant<reg_value_kind, Kind> {
      };

      struct reg_value_converter
      {

        boost::optional<uint32> operator() (const reg_value& v, kind_<reg_value_kind::dword>) {
          return get<uint32>(v);
        }

        boost::optional<uint64> operator() (const reg_value& v, kind_<reg_value_kind::qword>) {
          return get<uint64>(v);
        }

        boost::optional<std::wstring> operator() (const reg_value& v, kind_<reg_value_kind::string>) {
          return get<std::wstring>(v);
        }

        boost::optional<std::wstring> operator() (const reg_value& v, kind_<reg_value_kind::expand_string>) {
          return get<std::wstring>(v);
        }

        boost::optional<wstring_list> operator() (const reg_value& v, kind_<reg_value_kind::multi_string>) {
          return get<wstring_list>(v);
        }

        boost::optional<byte_vector> operator() (const reg_value& v, kind_<reg_value_kind::binary>) {
          return get<byte_vector>(v);
        }

      private:
        template<class Type>
        boost::optional<Type> get(const reg_value& v) const {
          const Type* p = boost::get<Type>(&v);
          if (!p)
            return nullptr;
          return *p;
        }

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
      std::wstring get_reg_key_name(reg_key_handle h, error_code_ref ec = nullptr);

      reg_value_kind get_reg_value_kind(reg_key_handle h, const lite_wstring& name, error_code_ref ec = nullptr);

      bool set_reg_value(reg_key_handle h, const lite_wstring& name, reg_value_kind kind, const byte* data_ptr, size_t size, error_code_ref ec = nullptr);
      bool set_reg_value(reg_key_handle h, const lite_wstring& name, reg_value_kind kind, const reg_value& value, error_code_ref ec = nullptr);
      bool set_reg_value(reg_key_handle h, const lite_wstring& name, const reg_value& value, error_code_ref ec = nullptr);
      bool set_reg_value_uint32(reg_key_handle h, const lite_wstring& name, uint32 value, error_code_ref ec = nullptr);
      bool set_reg_value_uint64(reg_key_handle h, const lite_wstring& name, uint64 value, error_code_ref ec = nullptr);
      bool set_reg_value_string(reg_key_handle h, const lite_wstring& name, const lite_wstring& value, error_code_ref ec = nullptr);
      bool set_reg_value_exp_string(reg_key_handle h, const lite_wstring& name, const lite_wstring& value, error_code_ref ec = nullptr);
      bool set_reg_value_multi_string(reg_key_handle h, const lite_wstring& name, const lite_buffer<wchar_t>& value, error_code_ref ec = nullptr);
      
      reg_value get_reg_value(reg_key_handle h, const lite_wstring& name, error_code_ref ec = nullptr);

      reg_item_enumerator enum_reg_sub_keys(reg_key_handle key, error_code_ref ec = nullptr);
      reg_item_enumerator enum_reg_value_names(reg_key_handle key, error_code_ref ec = nullptr);
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

      // Retrieves the name of the key
      std::wstring get_name(error_code_ref ec = nullptr) const {
        return api::get_reg_key_name(get_object(), ec);
      }

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

      //! Retrieves the value associated with the specified name. Returns an empty @b reg_value if the name/value pair does not exist in the registry.
      reg_value get_value(const lite_wstring& name, error_code_ref ec = nullptr) const {
        return api::get_reg_value(get_object(), name, ec);
      }

      //! Retrieves the value associated with the specified @b name. Returns @b null if the name/value pair does not exist in the registry.
      template<class T>
      boost::optional<T> get_value(const lite_wstring& name, error_code_ref ec = nullptr) const
      {
        reg_value val = get_value(name, ec);
        if (ec)
          return nullptr;

        T* p = boost::get<T>(&val);
        if (!p)
          return nullptr;
        return *p;
      }

      //! Retrieves the value associated with the specified @b name. If the name is not found, returns the default value that you provide.
      template<class T>
      T get_value(const lite_wstring& name, const T& def_value, error_code_ref ec = nullptr) const
      {
        auto val = get_value<T>(name, ec);
        if (ec)
          return def_value;
        return val.get_value_or(def_value);
      }

      //!
      reg_item_enumerator enum_sub_keys(error_code_ref ec = nullptr) const {
        return detail::enum_reg_sub_keys(*this, ec);
      }

      //!
      reg_item_enumerator enum_value_names(error_code_ref ec = nullptr) const {
        return detail::enum_reg_value_names(*this, ec);
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
      static object_type construct(const RegKey& k, const reg_open_params& params, error_code_ref ec) {
        return construct(k, reg_path(), params, ec);
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
      reg_item_enumerator enum_reg_value_names(const scoped_reg_key::tracker& key, error_code_ref ec = nullptr);
      reg_item_enumerator enum_reg_value_names(const shared_reg_key::tracker& key, error_code_ref ec = nullptr);
    }

  }
}

#endif
