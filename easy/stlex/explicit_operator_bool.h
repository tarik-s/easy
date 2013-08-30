/*!
 * \file   easy/unspecified_bool.h
 * \author Sergey Tararay
 * \date   04.08.2013
 *
 * FILE DESCRIPTION
 */
#ifndef EASY_UNSPECIFIED_BOOL_H_INCLUDED
#define EASY_UNSPECIFIED_BOOL_H_INCLUDED

#include <easy/config.h>

namespace easy
{

#ifdef EASY_HAS_EXPLICIT_OPERATOR
#  define EASY_EXPLICIT_OPERATOR_BOOL explicit operator bool
#  define EASY_EXPLICIT_TRUE  true
#  define EASY_EXPLICIT_FALSE false
#else

  namespace explicit_bool_detail
  {
    template<class T>
    struct bool_ {
      typedef bool_ this_type;
      void true_value() {}

      typedef void (this_type::*bool_type)();

      static inline bool_type true_() {
        return &this_type::true_value;
      }
      static inline bool_type false_() {
        return nullptr;
      }
    };
  }

#  define EASY_EXPLICIT_OPERATOR_BOOL              \
     class dummy_class_ {};                        \
     typedef ::easy::explicit_bool_detail::bool_<  \
       dummy_class_> explicit_bool_type;           \
     operator decltype(explicit_bool_type::true_())

#  define EASY_EXPLICIT_TRUE  explicit_bool_type::true_()
#  define EASY_EXPLICIT_FALSE explicit_bool_type::false_()
  
#endif

#  define EASY_DECLARE_EXPLICIT_OPERATOR_BOOL(Contition)                  \
    EASY_EXPLICIT_OPERATOR_BOOL() const EASY_NOEXCEPT {              \
      return (Contition) ? EASY_EXPLICIT_TRUE : EASY_EXPLICIT_FALSE; \
    }
}

#endif