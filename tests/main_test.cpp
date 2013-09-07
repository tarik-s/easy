#define BOOST_TEST_MAIN

#include <easy/easy.h>

#include "include.h"


BOOST_AUTO_TEST_CASE(MainEasyTest)
{

  using namespace easy::windows;

  reg_key key(reg_hive::hkcu, L"Software\\ABBYY");

  auto subkeys = key.keys();

  for (const std::wstring& k : subkeys) {
    reg_key _rk(key, k);
    std::wcout << k << '\n';
  }

  for (const std::wstring& k : subkeys) {
    reg_key _rk(key, k);
    std::wcout << k << '\n';
  }

  easy::windows::environment_variable var(L"TempVariable");


//   for (auto v : easy::windows::environment::get_variables()) {
//     std::wcout << v.get_name() << L"  " << v.get_value() << '\n';
//   }


  //BOOST_CHECK(1 == 2);
}
