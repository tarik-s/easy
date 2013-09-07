#ifndef EASY_WINDOWS_ENVIRONMENT_H_INCLUDED
#define EASY_WINDOWS_ENVIRONMENT_H_INCLUDED

#include <easy/windows/config.h>

#include <easy/strings.h>
#include <easy/error_code_ref.h>

#include <boost/noncopyable.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/iterator_range.hpp>

#include <map>

namespace easy {
namespace windows 
{
  enum class environment_variable_target
  {
    unknown,
    process,
    machine,
    user
  };
  
  class environment_variable
  {
  public:
    typedef environment_variable_target target;

    environment_variable() EASY_NOEXCEPT;
    environment_variable(environment_variable && v) EASY_NOEXCEPT;
    environment_variable(const c_wstring& _name, error_code_ref ec = nullptr);

    const std::wstring& get_name() const EASY_NOEXCEPT;
    const std::wstring& get_value() const EASY_NOEXCEPT;

    bool set_value(const c_wstring& _value, target _target, error_code_ref ec = nullptr);

    EASY_DECLARE_EXPLICIT_OPERATOR_BOOL(!m_name.empty())

  private:
    friend class environment_variable_iterator_impl;

    environment_variable(std::wstring && _name, std::wstring && _value) EASY_NOEXCEPT;
      
  private:
    std::wstring m_name;
    std::wstring m_value;
  };

  //////////////////////////////////////////////////////////////////////////

  class environment_variable_iterator
    : public boost::iterator_facade<environment_variable_iterator
      , const environment_variable, boost::forward_traversal_tag>
  {
  public:
    typedef environment_variable_target target;

    environment_variable_iterator() EASY_NOEXCEPT;
    environment_variable_iterator(environment_variable_iterator && r) EASY_NOEXCEPT;
    explicit environment_variable_iterator(target _target, error_code_ref ec = nullptr);
    ~environment_variable_iterator() EASY_NOEXCEPT;

    target get_target() const EASY_NOEXCEPT;

    EASY_DECLARE_EXPLICIT_OPERATOR_BOOL(!equal(environment_variable_iterator()))

  private:
    friend class boost::iterator_core_access;

    void increment();
    bool equal(const environment_variable_iterator& other) const EASY_NOEXCEPT;
    const environment_variable& dereference() const;

  private:
    typedef std::shared_ptr<environment_variable_iterator_impl> impl_ptr;
    impl_ptr m_pimpl;
  };

  typedef boost::iterator_range<environment_variable_iterator> 
    environment_variable_range;

  //////////////////////////////////////////////////////////////////////////

  namespace environment
  {
    typedef environment_variable_target target;

    static environment_variable_iterator enum_variables(target _target, error_code_ref ec = nullptr);
    static environment_variable_range get_variables(target _target, error_code_ref ec = nullptr);
  };
}}

#endif