/*!
 *  @file   easy/safe_call.h
 *  @author Sergey Tararay
 *  @date   2013
 */
#ifndef EASY_SAFE_CALL_H_INCLUDED
#define EASY_SAFE_CALL_H_INCLUDED

#include <easy/config.h>

#include <boost/optional.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_void.hpp>

#include <exception>
#include <functional>

namespace easy
{
  namespace safe_call_detail
  {
    typedef std::function<void(const std::exception&)> error_handler_type;

    template<class Func>
    bool safe_call_impl(Func func, const error_handler_type& handler, boost::mpl::true_)
    {
      try {
        func();
        return true;
      }
      catch(const std::exception& e) {
        if (handler)
          handler(e);
      }
      catch(...) {
        if (handler)
          handler(std::exception("Unknown exception"));
      }
      return false;
    }

    template<class Func>
    auto safe_call_impl(Func func, const error_handler_type& handler, boost::mpl::false_)
      -> boost::optional<decltype(func())>
    {
      try {
        return boost::make_optional(func());
      }
      catch(const std::exception& e) {
        if (handler)
          handler(e);
      }
      catch(...) {
        if (handler)
          handler(std::exception("Unknown exception"));
      }
      return nullptr;
    }

    template<class Func>
    auto safe_call(Func func, const error_handler_type& handler = nullptr) -> 
      typename boost::mpl::if_<
      boost::is_void<decltype(func())>,
      bool,
      boost::optional<decltype(func())>
      >::type
    {
      return safe_call_detail::safe_call_impl(func, handler, boost::is_void<decltype(func())>());
    }
  }
  
  typedef safe_call_detail::error_handler_type safe_call_error_handler_type;

  template<class Func>
  auto safe_call(Func func, const safe_call_error_handler_type& handler) ->
    decltype(safe_call_detail::safe_call(func, handler))
  {
    return safe_call_detail::safe_call(func, handler);
  }

  template<class Func>
  auto safe_call(Func func) ->
    decltype(safe_call_detail::safe_call(func))
  {
    return safe_call_detail::safe_call(func);
  }

  template<class Func>
  auto safe_call(Func func, std::exception_ptr& eptr) ->
    decltype(safe_call_detail::safe_call(func))
  {
    return safe_call_detail::safe_call(func, [&](const std::exception&) 
    {
      eptr = std::current_exception();
    });
  }

  template<class Printer>
  void print_exception(const std::exception& e, const Printer& printer, int level = 0) 
  {
    printer(e, level);
#ifdef EASY_HAS_NESTED_EXCEPTION
    try {
      std::rethrow_if_nested(e);
    }
    catch(const std::exception& e) {
      print_exception(e, printer, level + 1);
    }
    catch(...) {
      print_exception(std::exception("Unknown exception"), printer, level + 1);
    }
#endif
  }

  template<class Printer>
  void print_exception(const std::exception_ptr& eptr, const Printer& printer) {
    if (!eptr)
      return;
    try {
      std::rethrow_exception(eptr);
    }
    catch(const std::exception& e) {
      print_exception(e, printer);
    }
  }

}

#endif