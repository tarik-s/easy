
#include <easy/windows/com/bstr.h>
#include <exception>

namespace easy
{
  namespace windows
  {
    namespace com
    {
      com_bstr::com_bstr()
        : m_str(nullptr)
      {

      }

      com_bstr::com_bstr(nullptr_t)
        : m_str(nullptr)
      {

      }

      com_bstr::com_bstr(const com_bstr& r)
        : m_str(nullptr)
      {
        assign(lite_wstring(r.get(), r.size()));
      }

      com_bstr::com_bstr(com_bstr && r)
        : m_str(r.m_str)
      {
        r.m_str = nullptr;
      }

      com_bstr::com_bstr(const lite_wstring& s)
        : m_str(nullptr)
      {
        assign(s);
      }

      com_bstr::com_bstr(const lite_string& s)
        : m_str(nullptr)
      {
        assign(s);
      }

      com_bstr::~com_bstr()
      {
        if (m_str)
          ::SysFreeString(m_str);
      }

      com_bstr& com_bstr::operator = (nullptr_t)
      {
        com_bstr(nullptr).swap(*this);
        return *this;
      }

      com_bstr& com_bstr::operator = (const com_bstr& r)
      {
        if (r.m_str != m_str)
          assign(lite_wstring(r.get(), r.size()));
        return *this;
      }

      com_bstr& com_bstr::operator = (com_bstr && r)
      {
        if (r.m_str != m_str)
          r.swap(*this);
        return *this;
      }

      com_bstr& com_bstr::operator = (const lite_wstring& s)
      {
        assign(s);
        return *this;
      }

      com_bstr& com_bstr::operator = (const lite_string& s)
      {
        assign(s);
        return *this;
      }

      bool com_bstr::operator ! () const
      {
        return empty();
      }

      bool com_bstr::empty() const
      {
        return size() == 0;
      }

      com_bstr::size_type com_bstr::size() const
      {
        if (!m_str)
          return 0;
        return ::SysStringLen(m_str);
      }

      com_bstr::size_type com_bstr::length() const
      {
        return size();
      }

      void com_bstr::swap(com_bstr& r)
      {
        std::swap(r.m_str, m_str);
      }

      void com_bstr::clear()
      {
        com_bstr(nullptr).swap(*this);
      }

      com_bstr::const_pointer com_bstr::get() const
      {
        return m_str;
      }

      com_bstr::const_pointer com_bstr::data() const
      {
        return m_str;
      }

      com_bstr::const_pointer com_bstr::c_str() const
      {
        return m_str;
      }

      void com_bstr::assign(const lite_wstring& s)
      {
        BSTR str = nullptr; 
        if (s) {
          str = ::SysAllocStringLen(s.c_str(), s.size());
          if (!str)
            throw std::bad_alloc();
        }
        clear();
        m_str = str;
      }

      void com_bstr::assign(const lite_string& s)
      {
        assign(utf8_to_utf16(s));
      }

      com_bstr::const_reference com_bstr::at(size_type pos) const
      {
        if (pos >= size())
          throw std::out_of_range("Invalid position");
        return m_str[pos];
      }

      com_bstr::reference com_bstr::at(size_type pos)
      {
        if (pos >= size())
          throw std::out_of_range("Invalid position");
        return m_str[pos];
      }

      com_bstr::const_reference com_bstr::operator [] (size_type pos) const
      {
        EASY_ASSERT(pos < size());
        return m_str[pos];
      }

      com_bstr::reference com_bstr::operator [] (size_type pos)
      {
        EASY_ASSERT(pos < size());
        return m_str[pos];
      }

      com_bstr::iterator com_bstr::begin()
      {
        return m_str;
      }

      com_bstr::const_iterator com_bstr::begin() const
      {
        return m_str;
      }

      com_bstr::const_iterator com_bstr::cbegin() const
      {
        return begin();
      }

      com_bstr::iterator com_bstr::end()
      {
        if (!m_str)
          return nullptr;
        return m_str + length();

      }

      com_bstr::const_iterator com_bstr::end() const
      {
        if (!m_str)
          return nullptr;
        return m_str + length();
      }

      com_bstr::const_iterator com_bstr::cend() const
      {
        return end();
      }

    }
  }
}