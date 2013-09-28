/*!
 *  @file   easy/db/sqlite/detail/sqlite_detail.h
 *  @author Sergey Tararay
 *  @date   2013
 */
#ifndef EASY_DB_SQLITE_DETAIL_SQLITE_DETAIL_H_INCLUDED
#define EASY_DB_SQLITE_DETAIL_SQLITE_DETAIL_H_INCLUDED

#include <easy/db/sqlite/config.h>
#include <easy/safe_bool.h>
#include <easy/error_handling.h>
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