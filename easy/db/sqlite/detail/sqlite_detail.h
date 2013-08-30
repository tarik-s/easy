#ifndef EASY_DB_SQLITE_DETAIL_H
#define EASY_DB_SQLITE_DETAIL_H

#include <easy/db/config.h>
#include <easy/stlex/explicit_operator_bool.h>
#include <easy/error_code_ref.h>
#include <easy/strings.h>
#include <boost/noncopyable.hpp> // most classes are noncopyable

#include <memory>

#include <sqlite3.h>

namespace easy { 
namespace db { 
namespace sqlite {

  namespace detail 
  {
    error_code make_db_error_code(int code);

    class database_impl;
    class statement_impl;
  }
}}}

#endif