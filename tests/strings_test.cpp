/*!
  @file   tests/string_test.h
  @author Sergey Tararay
  @date   04.08.2013
 
  FILE DESCRIPTION
*/

#include "include.h"

#include <easy/strings.h>
#include <iostream>

namespace {

  enum class stringable_enum {
    value1,
    value2
  };

  std::string make_c_string(stringable_enum se) {
    switch(se) {
    case stringable_enum::value1 : return "__value1";
    case stringable_enum::value2 : return "__value2";
    }
    return nullptr;
  }
}

namespace easy {
  template<> struct underlying_char_type<stringable_enum> { typedef char type; };
}




BOOST_AUTO_TEST_CASE(Strings)
{
  using namespace easy;

  std::string s("hello");
  c_string _cs = s;

//  const char* pstr = "world";

  stringable_enum se = stringable_enum::value2;

  c_string _cse = se;

  c_string csss(std::string("heeee"));

}
