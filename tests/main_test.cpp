#define BOOST_TEST_MAIN

#include <easy/easy.h>

#include "include.h"


BOOST_AUTO_TEST_CASE(MainEasyTest)
{

  using namespace easy::windows;

  scoped_handle h;//(10, 20);
  h = nullptr;

  scoped_event evt(event_type::auto_, event_state::set);
  evt.set();

  evt = scoped_event(event_type::manual, event_state::reset);

  //scoped_event evt1 = evt;

  shared_event sevt;

  shared_event sev2(event_type::manual, event_state::reset);
  sevt = sev2;

  h.reset_object(nullptr);

  h = nullptr;

  scoped_reg_key key(reg_hive::hkcu, L"Software/ABBYY");
  //reg_value_type rvt = key.get_value_type(L"InstallationCount");

  for (const auto& k : make_range(key.enum_keys())) {
    //reg_key _rk(key, *k);
    std::wcout << k << '\n';
  }
// 
//   while (auto k = subkeys->get_next()) {
//     reg_key _rk(key, *k);
//     std::wcout << *k << '\n';
//   }

  //easy::windows::environment_variable var(L"TempVariable");

  for (const auto& v : make_range(environment_variable::enum_variables(environment_variable_target::process))) {
    std::wcout << v.get_name() << L"  " << v.get_value() << '\n';
  }

  environment_variable ev = environment_variable::create(L"Test", L"VAL", environment_variable_target::process);


  //BOOST_CHECK(1 == 2);
}
