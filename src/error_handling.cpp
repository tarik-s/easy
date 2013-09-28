#include <easy/error_handling.h>

namespace easy
{
  namespace 
  {
    generic_error_category g_cat;

  }
  error_code make_error_code(generic_error e)
  {
    return error_code(static_cast<int>(e), g_cat);
  }


  const char* generic_error_category::name() const
  {
    return "easy generic error";
  }

  std::string generic_error_category::message(int ev) const
  {
    switch (ev)
    {
      case generic_error::null_ptr
        : return "null pointer";
        case generic_error::invalid_value
          : return "invalid value";
    }
    EASY_ASSERT(!"Unknown error code");
    return std::string();
  }

  //////////////////////////////////////////////////////////////////////////

  error_code_ref::error_code_ref()
    : m_pcode(nullptr)
  {

  }

  error_code_ref::error_code_ref(nullptr_t)
    : m_pcode(nullptr)
  {

  }

  error_code_ref::error_code_ref(error_code& ec)
    : m_pcode(&ec) 
  {
    clear();
  }

  error_code_ref::error_code_ref(error_code* ec)
    : m_pcode(ec) 
  {
    clear();
  }

  error_code_ref::error_code_ref(const error_code_ref& ec)
    : m_pcode(ec.m_pcode) 
  {
    clear();
  }

  error_code_ref& error_code_ref::operator = (const error_code& ec)
  {
    if (!m_pcode) {
      if (ec)
        throw system_error(ec);
    } else {
      *m_pcode = ec;
    }      
    return *this;
  }

  void error_code_ref::assign(int code, const error_category& cat)
  {
    *this = error_code(code, cat);
  }

  void error_code_ref::set(int code, const error_category& cat)
  {
    assign(code, cat);
  }

  void error_code_ref::set_system_error(int code)
  {
    set(code, boost::system::system_category());
  }

  void error_code_ref::clear()
  {
    if (m_pcode)
      m_pcode->clear();
  }

  detail::error_code_holder error_code_ref::get()
  {
    return detail::error_code_holder(m_pcode);
  }

  bool error_code_ref::is_throwable() const
  {
    return !m_pcode;
  }

  bool error_code_ref::is_error() const
  {
    return m_pcode && *m_pcode;
  }

  bool error_code_ref::operator ! () const
  {
    return !is_error();
  }

  //////////////////////////////////////////////////////////////////////////

  namespace detail
  {
    error_code_holder::error_code_holder(error_code* ec)
      : m_code(!ec ? m_own_code : *ec)
    {

    }

    error_code_holder::~error_code_holder()
    {
      if (m_own_code) {
        if (!std::uncaught_exception())
          throw system_error(m_own_code);
        EASY_ASSERT(!"The stack is unwinding now. An exception cannot be thrown.");
      }
    }

    error_code_holder::operator error_code& ()
    {
      return m_code;
    }

    error_code_holder::operator error_code* ()
    {
      return &m_code;
    }
  }

}