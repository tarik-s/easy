#ifndef EASY_UNSPECIFIED_BOOL_DETAIL_H_INCLUDED
#define EASY_UNSPECIFIED_BOOL_DETAIL_H_INCLUDED

namespace easy
{
  namespace detail
  {
    struct unspecified {
      void true_value() {} 
    };

    typedef void (unspecified::*unspecified_bool_type)();
  }
}

#endif