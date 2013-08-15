/*!
  @file   tests/string_test.h
  @author Sergey Tararay
  @date   04.08.2013
 
  FILE DESCRIPTION
*/

#include <boost/test/unit_test.hpp>
#include <easy/strings.h>

namespace {

  enum class stringable_enum {
    value1,
    value2
  };

  template<class TChar>
  std::basic_string<TChar> make_c_string(stringable_enum se) {
    switch(se) {
    case stringable_enum::value1 : return "__value1";
    case stringable_enum::value2 : return "__value2";
    }
    return nullptr;
  }
}

namespace easy {
  template<> struct is_stringable<stringable_enum> : boost::true_type { };
}




BOOST_AUTO_TEST_CASE(Strings)
{
  using namespace easy;

  std::string s("hello");
  c_string _cs = s;

  const char* pstr = "world";

  stringable_enum se = stringable_enum::value2;

  c_string _cse = se;

  c_string csss(std::string("heeee"));
  
  

}