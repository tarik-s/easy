/*!
 *  @file   easy/windows/api/registry_api.h
 *  @author Sergey Tararay
 *  @date   2013
 */
#ifndef EASY_WINDOWS_API_REGISTRY_H_INCLUDED
#define EASY_WINDOWS_API_REGISTRY_H_INCLUDED

#include <easy/windows/config.h>

#include <easy/error_code_ref.h>
#include <easy/strings.h>
#include <easy/types.h>

#include <boost/filesystem/path.hpp>
#include <boost/variant.hpp>

#include <Windows.h>


namespace easy {
namespace windows 
{
  //! Registry key handle
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

  //! Types which can be saved in the registry
  enum class reg_value_type
  {
    unknown = -1,
    none,
    dword,
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
  enum class reg_open_mode {
    open,
    create
  };

  //! Registry virtualization
  enum class reg_virtualization
  {
    enabled,
    disabled
  };

  //! Registry value holder type
  typedef boost::variant<
    uint32,
    uint64,
    std::wstring,
    wstring_list,
    byte_vector
  > reg_value_holder;

  namespace api 
  {  
    static const reg_key_handle invalid_reg_key_handle = nullptr;

    bool is_reg_key_handle_valid(reg_key_handle h) EASY_NOEXCEPT;
    bool check_reg_key_handle(reg_key_handle h, error_code_ref ec = nullptr);

    bool close_reg_key(reg_key_handle h, error_code_ref ec = nullptr);
    bool delete_reg_key(reg_key_handle h, const reg_path& subkey, error_code_ref ec = nullptr);
    bool delete_reg_value(reg_key_handle h, const c_wstring& name, error_code_ref ec = nullptr);

    reg_key_handle create_reg_key(reg_key_handle h, const reg_path& subkey, reg_open_mode mode, 
      reg_access access, reg_virtualization virt, error_code_ref ec = nullptr);

    reg_value_type get_reg_value_type(reg_key_handle h, const c_wstring& name, error_code_ref ec = nullptr);
    
    bool set_reg_value(reg_key_handle h, const c_wstring& name, reg_value_type type, const byte* data_ptr, size_t size, error_code_ref ec = nullptr);
    bool set_reg_value_uint32(reg_key_handle h, const c_wstring& name, uint32 value, error_code_ref ec = nullptr);
    bool set_reg_value_uint64(reg_key_handle h, const c_wstring& name, uint64 value, error_code_ref ec = nullptr);
    bool set_reg_value_string(reg_key_handle h, const c_wstring& name, const c_wstring& value, error_code_ref ec = nullptr);
    bool set_reg_value_exp_string(reg_key_handle h, const c_wstring& name, const c_wstring& value, error_code_ref ec = nullptr);


  }
}}

#endif