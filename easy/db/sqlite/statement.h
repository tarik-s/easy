#ifndef EASY_DB_SQLITE_STATEMENT_H
#define EASY_DB_SQLITE_STATEMENT_H

#include <easy/db/config.h>
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
      statement(database& _db, const c_utf8_string& query, error_code_ref ec = nullptr);

      //EASY_DECLARE_EXPLICIT_OPERATOR_BOOL(m_impl_ptr)

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