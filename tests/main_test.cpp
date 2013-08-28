#define BOOST_TEST_MAIN

#include <easy/easy.h>

#include "include.h"

BOOST_AUTO_TEST_CASE(MainEasyTest)
{
  easy::scope_exit s = [&]() { std::cout << "Hello"; };


  //BOOST_CHECK(1 == 2);
}
