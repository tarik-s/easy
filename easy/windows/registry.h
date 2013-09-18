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

#include <vector>

#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/variant.hpp>

#include <Windows.h>

namespace easy {
namespace windows 
{

  enum class reg_hive 
  {
    hkey_classes_root                = (int)HKEY_CLASSES_ROOT,
    hkey_current_user                = (int)HKEY_CURRENT_USER,
    hkey_local_machine               = (int)HKEY_LOCAL_MACHINE,
    hkey_users                       = (int)HKEY_USERS,
    hkey_performance_data            = (int)HKEY_PERFORMANCE_DATA,
    hkey_current_config              = (int)HKEY_CURRENT_CONFIG,
    hkey_dyn_data                    = (int)HKEY_DYN_DATA,
/*
    hkey_performance_text            = (int)HKEY_PERFORMANCE_TEXT,
    hkey_performance_nlstext         = (int)HKEY_PERFORMANCE_NLSTEXT,
    hkey_current_user_local_settings = (int)HKEY_CURRENT_USER_LOCAL_SETTINGS,
*/
    hkcu = hkey_current_user,
    hklm = hkey_local_machine,
  };

  enum class reg_value_type 
  {
    none = -1,
    int_,
    int64_,
    string,
    multi_string,
    binary
    /*
    none                = REG_NONE,
    sz                  = REG_SZ,
    expand_sz           = REG_EXPAND_SZ,
    binary              = REG_BINARY,
    dword               = REG_DWORD,
    dword_big_endian    = REG_DWORD_BIG_ENDIAN,    
    link                = REG_LINK,
    multi_sz            = REG_MULTI_SZ,
    qword               = REG_QWORD,  
    */
  };

  typedef boost::variant<
    int,
    int64,
    std::wstring,
    wstring_list,
    byte_vector
  > reg_value;

  enum class reg_option {
    non_volatile   = REG_OPTION_NON_VOLATILE,
    volatile_      = REG_OPTION_VOLATILE,    
    create_link    = REG_OPTION_CREATE_LINK,
    backup_restore = REG_OPTION_BACKUP_RESTORE,
    open_link      = REG_OPTION_OPEN_LINK,
  };

  enum class reg_access {
    read  = KEY_READ,
    write = KEY_WRITE,
    all   = KEY_ALL_ACCESS
  };

  enum class reg_open_mode {
    open,
    create
  };

  struct reg_open_options
  {
    reg_open_options() 
      : access(reg_access::all)
      , option(reg_option::non_volatile)
      , open_mode(reg_open_mode::open)
    {
    }

    reg_open_options& operator << (reg_access a) {
      access = a;
      return *this;
    }

    reg_open_options& operator << (reg_option o) {
      option = o;
      return *this;
    }

    reg_open_options& operator << (reg_open_mode m) {
      open_mode = m;
      return *this;
    }

    reg_access access;
    reg_option option;
    reg_open_mode open_mode;
  };
    
  class reg_key;

  class reg_key_iterator
    : public boost::iterator_facade<reg_key_iterator, const std::wstring, boost::forward_traversal_tag>
  {
  public:
    reg_key_iterator() EASY_NOEXCEPT;
    reg_key_iterator(reg_key_iterator && r) EASY_NOEXCEPT;
    explicit reg_key_iterator(const reg_key& key, error_code_ref ec = nullptr);
    ~reg_key_iterator() EASY_NOEXCEPT;

  private:
    friend class boost::iterator_core_access;
    friend class reg_key_enum_impl;
    void increment();
    bool equal(const reg_key_iterator& other) const EASY_NOEXCEPT;
    const std::wstring& dereference() const;

  private:
    typedef std::shared_ptr<reg_key_enum_impl> impl_ptr;
    impl_ptr m_pimpl;
  };

  class reg_value_iterator
  {
  public:

  private:

  };

  typedef boost::iterator_range<reg_key_iterator> reg_key_iterator_range;

  class reg_key
    : boost::noncopyable
  {
  public:
    typedef HKEY handle_type;

    // default constructor
    reg_key() EASY_NOEXCEPT;

    // move constructor and move assignment operator
    reg_key(reg_key && r) EASY_NOEXCEPT;
    reg_key& operator = (reg_key && r) EASY_NOEXCEPT;

    // constructors
    reg_key(reg_hive h, const c_wstring& subkey, const reg_open_options& options = reg_open_options(), error_code_ref ec = nullptr);
    reg_key(const reg_key& key, const c_wstring& subkey, const reg_open_options& options = reg_open_options(), error_code_ref ec = nullptr);

    // destructors
    ~reg_key() EASY_NOEXCEPT;

    handle_type handle() const EASY_NOEXCEPT {
      return m_key;
    }

    // key deleters
    bool delete_key(const c_wstring& subkey, error_code_ref ec = nullptr);
    static bool delete_key(reg_hive h, const c_wstring& subkey, error_code_ref ec = nullptr);

    // enumerators
    reg_key_iterator enum_keys(error_code_ref ec = nullptr) const;
    reg_key_iterator_range keys(error_code_ref ec = nullptr) const;
  private:
    reg_key(HKEY key, const c_wstring& subkey, const reg_open_options& options, error_code_ref ec, void* dummy);

  private:
    HKEY m_key;
  };
    

}}

#endif
