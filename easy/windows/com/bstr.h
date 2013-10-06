#ifndef EASY_WINDOWS_COM_BSTR_H_INCLUDED
#define EASY_WINDOWS_COM_BSTR_H_INCLUDED

#include <easy/windows/com/config.h>

#include <easy/safe_bool.h>
#include <easy/strings.h>

#include <type_traits>

#include <Windows.h>

namespace easy
{
  namespace windows { namespace com { class com_bstr; } }

  template<>
  struct underlying_char_type<windows::com::com_bstr> { typedef wchar_t type; };
  
  namespace windows
  {
    namespace com
    {
      //! com_bstr
      class com_bstr
        : public safe_bool<com_bstr>
      {
      public:
        typedef size_t    size_type;
        typedef OLECHAR   char_type;
        typedef char_type value_type;
        typedef ptrdiff_t difference_type;

        typedef value_type&       reference;
        typedef const value_type& const_reference;
        typedef value_type*       pointer;
        typedef const value_type* const_pointer;

        typedef pointer           iterator;
        typedef const_pointer     const_iterator;

        typedef std::reverse_iterator<iterator> reverse_iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

        static const size_type npos = (size_type)-1;

      public:
        //!
        com_bstr() EASY_NOEXCEPT;

        //!
        com_bstr(nullptr_t) EASY_NOEXCEPT;

        //!
        com_bstr(const com_bstr& r);

        //!
        com_bstr(com_bstr && r) EASY_NOEXCEPT;

        //!
        com_bstr(const lite_wstring& s);

        //!
        com_bstr(const lite_string& s);

        //!
        ~com_bstr() EASY_NOEXCEPT;

        //!
        com_bstr& operator = (nullptr_t);

        //! 
        com_bstr& operator = (const com_bstr& r);

        //! 
        com_bstr& operator = (com_bstr && r) EASY_NOEXCEPT;

        //!
        com_bstr& operator = (const lite_wstring& s);

        //!
        com_bstr& operator = (const lite_string& s);

        //!
        bool operator ! () const;

        //!
        iterator begin();
        const_iterator begin() const;
        const_iterator cbegin() const;

        //!
        iterator end();
        const_iterator end() const;
        const_iterator cend() const;

        //!
        const_reference at(size_type pos) const;
        reference at(size_type pos);
        const_reference operator [] (size_type pos) const;
        reference operator [] (size_type pos);

        //!
        const_pointer get() const EASY_NOEXCEPT;
        const_pointer data() const EASY_NOEXCEPT;
        const_pointer c_str() const EASY_NOEXCEPT;

        //!
        bool empty() const;

        //!
        size_type size() const;
        size_type length() const;

        //!
        void swap(com_bstr& r) EASY_NOEXCEPT;

        //!
        void clear();

        //!
        void assign(const lite_wstring& s);

        //!
        void assign(const lite_string& s);
        
      private:
        BSTR m_str;
      };
    
      inline lite_wstring make_c_string(const com_bstr& s) {
        return lite_wstring(s.get(), s.size());
      }

      //!
      template<class Traits>
      std::basic_ostream<com_bstr::value_type, Traits>& operator << (std::basic_ostream<com_bstr::value_type, Traits>& os, const com_bstr& s)
      {
        if (s)
          os << s.c_str();
        return os;
      }

      //!
      inline bool operator == (const com_bstr& s1, const com_bstr& s2)
      {
        return lite_wstring(s1) == lite_wstring(s2);
      }

      inline bool operator < (const com_bstr& s1, const com_bstr& s2)
      {
        return lite_wstring(s1) < lite_wstring(s2);
      }

      inline bool operator <= (const com_bstr& s1, const com_bstr& s2)
      {
        return lite_wstring(s1) <= lite_wstring(s2);
      }

      inline bool operator != (const com_bstr& s1, const com_bstr& s2)
      {
        return lite_wstring(s1) != lite_wstring(s2);
      }

      inline bool operator > (const com_bstr& s1, const com_bstr& s2)
      {
        return lite_wstring(s1) > lite_wstring(s2);
      }

      inline bool operator >= (const com_bstr& s1, const com_bstr& s2)
      {
        return lite_wstring(s1) >= lite_wstring(s2);
      }
    
    }
  }  
}


#endif