#include <easy/db/sqlite/sqlite.h>

#include <easy/strings/string_conv.h>
#include <easy/stlex/make_unique.h>

namespace easy { namespace db { namespace sqlite 
{

  static error_category g_db_error_cat;

  const char* error_category::name() const
  {
    return "SQLite error";
  }

  std::string error_category::message(int ev) const
  {
    if (ev >= SQLITE_OK)
      return ::sqlite3_errstr(ev);

    switch (ev)
    {
    case result_code::null_database:
      return "null database";
    case result_code::null_statement:
      return "null statement";
    default:
      break;
    }
    return std::string();
  }

  namespace detail
  {
    error_code make_db_error_code(int code)
    {
      return error_code(code, g_db_error_cat);
    }

    error_code make_db_error_code(result_code code)
    {
      return error_code(static_cast<int>(code), g_db_error_cat);
    }

    //////////////////////////////////////////////////////////////////////////

    class database_impl 
      : boost::noncopyable
    {
    public:
      database_impl(const lite_string& file_path) EASY_NOEXCEPT
        : m_is_v2(false)
      {
        m_code = ::sqlite3_open(file_path.c_str(), &m_db);
      }

      database_impl(const lite_string& file_path, int flags)  EASY_NOEXCEPT
        : m_is_v2(true)
      {
        m_code = ::sqlite3_open_v2(file_path.c_str(), &m_db, flags, nullptr);
      }

      ~database_impl() EASY_NOEXCEPT {
        if (m_db) {
          int res = m_is_v2 ? ::sqlite3_close_v2(m_db) : ::sqlite3_close(m_db);
          EASY_ASSERT(res == SQLITE_OK);
        }
      }

      int execute(const lite_string& sql) EASY_NOEXCEPT {
        return (m_code = ::sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, nullptr));
      }

      ::sqlite3* get_handle() const EASY_NOEXCEPT {
        return m_db;
      }

      error_code get_last_ec() const EASY_NOEXCEPT {
        return make_db_error_code(m_code);
      }

    private:
      const bool m_is_v2;
      int m_code;
      ::sqlite3* m_db;
    };

    //////////////////////////////////////////////////////////////////////////

    class statement_impl
      : boost::noncopyable
    {
    public:
      statement_impl(database_impl& _db, const lite_string& query) EASY_NOEXCEPT
        : m_stmt(nullptr)
      {
        m_code = ::sqlite3_prepare_v2(_db.get_handle(), query.c_str(), query.length(), &m_stmt, nullptr);
      }

      ~statement_impl() EASY_NOEXCEPT {
        if (m_stmt)
          ::sqlite3_finalize(m_stmt);
      }

      void step() EASY_NOEXCEPT {
        m_code = ::sqlite3_step(m_stmt);
      }

      error_code get_last_ec() const EASY_NOEXCEPT {
        return make_db_error_code(m_code);
      }
    private:
      ::sqlite3_stmt* m_stmt;
      int m_code;
    };

  }

  using namespace detail;

  //////////////////////////////////////////////////////////////////////////

  database::database()
  {
  }

  database::database(database && r)
    : m_impl_ptr(std::move(r.m_impl_ptr))
  {

  }

  database::database(const lite_string& db_file_path, error_code_ref ec)
    : m_impl_ptr(std::make_unique<database_impl>(db_file_path.c_str()))
  {
    ec = m_impl_ptr->get_last_ec();
    if (ec)
      m_impl_ptr.reset();
  }

  database::database(const lite_string& db_file_path, open_flag flags, error_code_ref ec)
    : m_impl_ptr(std::make_unique<database_impl>(db_file_path.c_str(), static_cast<int>(flags)))
  {
    ec = m_impl_ptr->get_last_ec();
    if (ec)
      m_impl_ptr.reset();
  }

  database::~database()
  {

  }

  database& database::operator=(database && r)
  {
    m_impl_ptr.swap(std::move(r.m_impl_ptr));
    return *this;
  }

  bool database::execute(const lite_string& sql, error_code_ref ec)
  {
    if (m_impl_ptr) {
      m_impl_ptr->execute(sql);
      ec = m_impl_ptr->get_last_ec();
    }
    else {
      ec = make_db_error_code(result_code::null_database);
    }
    return !ec;
  }

  statement database::create_statement(const lite_string& query, error_code_ref ec)
  {
    if (m_impl_ptr) {
      statement::impl_ptr ps(std::make_unique<statement_impl>(*m_impl_ptr, query));
      ec = ps->get_last_ec();
      if (!ec)
        return statement(std::move(ps));
    }
    else {
      ec = make_db_error_code(result_code::null_database);
    }
    return statement();
  }

  //////////////////////////////////////////////////////////////////////////

  statement::statement()
  {

  }

  statement::~statement()
  {

  }

  statement::statement(statement && r)
    : m_impl_ptr(std::move(r.m_impl_ptr))
  {

  }

  statement::statement(impl_ptr && r)
    : m_impl_ptr(std::move(r))
  {

  }
  
  statement& statement::operator=(statement && r)
  {
    m_impl_ptr.swap(std::move(r.m_impl_ptr));
    return *this;
  }

  statement::statement(database& _db, const lite_string& query, error_code_ref ec)
  {
    *this = _db.create_statement(query, ec);
  }

  bool statement::next(error_code_ref ec)
  {
    int res = SQLITE_ERROR;
    EASY_ASSERT(!!m_impl_ptr);
    if (m_impl_ptr) {
      m_impl_ptr->step();

      if (res == SQLITE_ROW)
        return true;
      else if (res == SQLITE_DONE)
        return false;
    }
    ec = make_db_error_code(res);
    return false;

    
    
    
  }







}}}