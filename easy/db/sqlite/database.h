#ifndef EASY_DB_SQLITE_DATABASE_H
#define EASY_DB_SQLITE_DATABASE_H

#include <easy/db/config.h>
#include <easy/db/sqlite/detail/sqlite_detail.h>

#include <easy/strings.h>
#include <easy/error_code_ref.h>
#include <easy/flags.h>

#include <easy/db/sqlite/statement.h>

namespace easy { 
namespace db {
  namespace sqlite
  {
    enum class open_flag
    {
      read_only       = SQLITE_OPEN_READONLY,        /* Ok for sqlite3_open_v2() */
      read_write      = SQLITE_OPEN_READWRITE,       /* Ok for sqlite3_open_v2() */
      create          = SQLITE_OPEN_CREATE,          /* Ok for sqlite3_open_v2() */
      delete_on_close = SQLITE_OPEN_DELETEONCLOSE,   /* VFS only */
      exclusive       = SQLITE_OPEN_EXCLUSIVE,       /* VFS only */
      auto_proxy      = SQLITE_OPEN_AUTOPROXY,       /* VFS only */
      uri             = SQLITE_OPEN_URI,             /* Ok for sqlite3_open_v2() */
      memory          = SQLITE_OPEN_MEMORY,          /* Ok for sqlite3_open_v2() */
      main_db         = SQLITE_OPEN_MAIN_DB,         /* VFS only */
      temp_db         = SQLITE_OPEN_TEMP_DB,         /* VFS only */
      transient_db    = SQLITE_OPEN_TRANSIENT_DB,    /* VFS only */
      main_journal    = SQLITE_OPEN_MAIN_JOURNAL,    /* VFS only */
      temp_journal    = SQLITE_OPEN_TEMP_JOURNAL,    /* VFS only */
      subjournal      = SQLITE_OPEN_SUBJOURNAL,      /* VFS only */
      master_journal  = SQLITE_OPEN_MASTER_JOURNAL,  /* VFS only */
      nomutex         = SQLITE_OPEN_NOMUTEX,         /* Ok for sqlite3_open_v2() */
      fullmutex       = SQLITE_OPEN_FULLMUTEX,       /* Ok for sqlite3_open_v2() */
      shared_cache    = SQLITE_OPEN_SHAREDCACHE,     /* Ok for sqlite3_open_v2() */
      private_cache   = SQLITE_OPEN_PRIVATECACHE,    /* Ok for sqlite3_open_v2() */
      wal             = SQLITE_OPEN_WAL,             /* VFS only */
    };
    EASY_DECLARE_AS_FLAGS(open_flag);

    enum class result_code : int
    {
      null_database    = -100,
      null_statement,
      // sqlite codes
      ok = SQLITE_OK,
    };

    class error_category 
      : public boost::system::error_category
    {
    public:
      const char * name() const;
      std::string message(int ev) const EASY_FINAL;
    };

    class database
      : boost::noncopyable
    {
    public:
      typedef std::unique_ptr<detail::database_impl> impl_ptr;
    public:

      database() EASY_NOEXCEPT;
      ~database() EASY_NOEXCEPT;

      database(database && r) EASY_NOEXCEPT;
      database& operator = (database && r) EASY_NOEXCEPT;

      explicit database(const c_utf8_string& db_file_path, error_code_ref ec = nullptr);
      explicit database(const c_utf16_string& db_file_path, error_code_ref ec = nullptr);
      database(const c_utf8_string& db_file_path, open_flag flags, error_code_ref ec = nullptr);
      database(const c_utf16_string& db_file_path, open_flag flags, error_code_ref ec = nullptr);      

      EASY_DECLARE_EXPLICIT_OPERATOR_BOOL(m_impl_ptr)

      bool execute(const c_utf8_string& sql, error_code_ref ec = nullptr);
      bool execute(const c_utf16_string& sql, error_code_ref ec = nullptr);
      
      statement create_statement(const c_utf8_string& query, error_code_ref ec = nullptr);
      statement create_statement(const c_utf16_string& query, error_code_ref ec = nullptr);

    private:
      impl_ptr m_impl_ptr;
    };
  }
}}

namespace boost {
namespace system {
  template<>
  struct is_error_code_enum<easy::db::sqlite::result_code> 
    : boost::true_type {
  };
}
}


#endif