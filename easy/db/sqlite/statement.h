/*!
 *  @file   easy/db/sqlite/statement.h
 *  @author Sergey Tararay
 *  @date   2013
 */
#ifndef EASY_DB_SQLITE_STATEMENT_H_INCLUDED
#define EASY_DB_SQLITE_STATEMENT_H_INCLUDED

#include <easy/db/sqlite/config.h>
#include <easy/db/sqlite/detail/sqlite_detail.h>

namespace easy { 
namespace db
{
  namespace sqlite
  {
    class database;

    class statement
      : boost::noncopyable
    {
    public:
      typedef std::unique_ptr<detail::statement_impl> impl_ptr;
    public:
      statement() EASY_NOEXCEPT;
      ~statement() EASY_NOEXCEPT;
      statement(statement && r) EASY_NOEXCEPT;
      statement& operator = (statement && r) EASY_NOEXCEPT;
      statement(database& _db, const lite_string& query, error_code_ref ec = nullptr);

      bool next(error_code_ref ec = nullptr);
    protected:
      statement(impl_ptr && r);
    private:
      friend database;
      impl_ptr m_impl_ptr;
    };




  }
}}

#endif