#include <easy/error_code_ref.h>

namespace easy
{
  namespace 
  {
    generic_error_category g_cat;
  }
  error_code make_error_code(generic_error e)
  {
    return error_code(static_cast<int>(e), g_cat);
  }


  const char* generic_error_category::name() const
  {
    return "easy generic error";
  }

  std::string generic_error_category::message(int ev) const
  {
    switch (ev)
    {
    case generic_error::null_ptr
      : return "null pointer";
    case generic_error::invalid_value
      : return "invalid value";
    }
    EASY_ASSERT(!"Unknown error code");
    return std::string();
  }

}