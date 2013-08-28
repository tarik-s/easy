#include "include.h"
#include <easy/db/sqlite.h>


BOOST_AUTO_TEST_CASE(SQLite)
{
  using namespace easy::db;
  
  easy::error_code ec;
  sqlite::database db("d:\\sss.db", ec);
  BOOST_CHECK(!ec);

  sqlite::statement st(db, "create table MyEmployees ( _id integer primary key, name text not null)", ec);
  BOOST_CHECK(!ec);

  st.next();
  //BOOST_CHECK(!ec);

  st.next();
  //BOOST_CHECK(!ec);

}
