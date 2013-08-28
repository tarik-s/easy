#include "include.h"
#include <easy/flags.h>


namespace {

  enum class open_param
  {
    read  = 0x0001,
    write = 0x0002,
  };

  EASY_DECLARE_AS_FLAGS(open_param);


}

// namespace easy {
//   template<>
//   struct is_flag<open_param> 
//     : boost::true_type {
//   };
// }

BOOST_AUTO_TEST_CASE(Flags)
{
  open_param p = open_param::read;
  p = ~p;
  BOOST_CHECK((p & open_param::write) == open_param::write);
  BOOST_CHECK((p & open_param::write) != open_param::read);
  p |= open_param::read;

  bool res = (p & open_param::read) == open_param::read;
  BOOST_CHECK(res);
 

//  open_param op = static_cast<open_params::flag_type>((open_params::underlying_type)params);
}
