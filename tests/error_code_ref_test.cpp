///
/// \file   tests/error_code_ref_test.h
/// \author Sergey Tararay
/// \date   04.08.2013
///
/// FILE DESCRIPTION
///

#include "include.h"

#include <easy/error_code_ref.h>

namespace {
  enum class user_error;
}

namespace boost {
  namespace system {
    template<>
    struct is_error_code_enum<user_error> 
      : boost::true_type {
    };
  }
}

namespace {

  enum class user_error
  {
    invalid = 1,
    empty
  };

  class user_error_category 
    : public boost::system::error_category
  {
  public:

    const char * name() const EASY_NOEXCEPT
    {
      return "user_error";
    }
    std::string message(int ev) const EASY_FINAL
    {
      return "test";
    }
  };

  user_error_category g_cat;

  inline easy::error_code make_error_code(user_error e) EASY_NOEXCEPT
  { return easy::error_code((int)e, g_cat); }




  void test_func1(easy::error_code_ref ec = nullptr) {
    ec = user_error::invalid; // will throw if ec is null
  }

  void test_func2(easy::error_code_ref ec = nullptr) {
    ec.set_system_error(0); // will not throw
  }
}




BOOST_AUTO_TEST_CASE(ErrorCode)
{

  easy::error_code _ec = user_error::invalid;
  test_func1(_ec);
  bool res = _ec == user_error::invalid;
  BOOST_CHECK(res);
  test_func2();
  try
  {
    test_func1();
  }
  catch(const easy::system_error&) {
    return;
  }
  throw "The call within try-catch block must shouldn't throw";
}
