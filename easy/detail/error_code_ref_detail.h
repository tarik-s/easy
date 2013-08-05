#ifndef EASY_ERROR_CODE_REF_DETAIL_H_INCLUDED
#define EASY_ERROR_CODE_REF_DETAIL_H_INCLUDED

#include <easy/config.h>

#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>

namespace easy
{
  namespace detail
  {
    using boost::system::error_code;
    using boost::system::system_error;

    class error_code_holder
    {
    public:
      error_code_holder(error_code* ec) EASY_NOEXCEPT
        : m_code(!ec ? m_own_code : *ec) {
      }
      ~error_code_holder() {
        if (m_own_code) {
          if (!std::uncaught_exception())
            throw system_error(m_own_code);
          EASY_ASSERT(!"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
        }
      }
      operator error_code& () EASY_NOEXCEPT {
        return m_code;
      }
      operator error_code* () EASY_NOEXCEPT {
        return &m_code;
      }
    private:
      error_code m_own_code;
      error_code& m_code;
    };
  }
}

#endif