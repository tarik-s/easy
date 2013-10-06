#ifndef EASY_WINDOWS_COM_PTR_H_INCLUDED
#define EASY_WINDOWS_COM_PTR_H_INCLUDED

#include <easy/windows/com/config.h>

#include <easy/safe_bool.h>

#include <type_traits>

#include <Windows.h>


namespace easy
{
  namespace windows
  {
    namespace com
    {
      namespace detail
      {
        template<class T>
        class no_ref_ptr
          : public T
        {
          STDMETHOD_(ULONG, AddRef)()=0;
          STDMETHOD_(ULONG, Release)()=0;
        };
      }

      //! com_ptr_base
      template<class T>
      class com_ptr_base
        : public safe_bool<com_ptr_base<T>>
      {
      public:
        //! this_type
        typedef  com_ptr_base this_type;

        //! Underlying interface type
        typedef T interface_type;

      protected:
        //! Base constructor
        com_ptr_base(interface_type* p, bool add_ref = true)
          : m_ptr(p)
        {
          if (m_ptr && add_ref)
            m_ptr->AddRef();
        }

      public:
        //! Destructor
        ~com_ptr_base()
        {
          LONG cnt = 0;
          if (m_ptr)
            cnt = m_ptr->Release();
          cnt;
        }

        //! swap
        void swap(this_type& r)
        {
          std::swap(m_ptr, r.m_ptr);
        }

        //! reset
        void reset(interface_type* p)
        {
          this_type(p).swap(*this);
        }
        
        //! get
        interface_type* get() const
        {
          return m_ptr;
        }

        //! release
        interface_type* release()
        {
          interface_type* tmp = m_ptr;
          m_ptr = nullptr;
          return tmp;
        }



        interface_type& operator*() const
        {
          EASY_ASSERT(m_ptr);
          return *m_ptr;
        }

        interface_type** operator&()
        {
          EASY_ASSERT(!m_ptr);
          return &m_ptr;
        }
        detail::no_ref_ptr<interface_type>* operator->() const
        {
          EASY_ASSERT(m_ptr);
          return (detail::no_ref_ptr<interface_type>*)m_ptr;
        }
        bool operator!() const
        {	
          return !m_ptr;
        }
        
      private:
        interface_type* m_ptr;
      };

      //! com_ptr
      template<class T>
      class com_ptr
        : public com_ptr_base<T>
      {
        typedef com_ptr_base<T> base_type;
      public:
        //! Default constructor
        com_ptr()
          : base_type(nullptr) {
        }

        //! Constructor from null
        com_ptr(nullptr_t)
          : base_type(nullptr) {
        }

        //! 
        com_ptr(const com_ptr& r)
          : base_type(r.get()) {
        }

        //! 
        com_ptr(com_ptr && r)
          : base_type(nullptr)
        {
          r.swap(*this);
        }

        //! 
        explicit com_ptr(interface_type* p, bool add_ref = true)
          : base_type(p, add_ref) {
        }

        //!
        com_ptr& operator = (nullptr_t) 
        {
          com_ptr(nullptr).swap(*this);
          return *this;
        }

        //!
        com_ptr& operator = (const com_ptr& r)
        {
          if (*this != r)
            com_ptr(r).swap(*this);
          return *this;
        }

        //!
        com_ptr& operator = (com_ptr && r)
        {
          if (*this != r)
            com_ptr(static_cast<com_ptr&&>(r)).swap(*this);
          return *this;
        }

        //!
        com_ptr& operator = (interface_type* r)
        {
          if (*this != r)
            com_ptr(r).swap(*this);
          return *this;
        }
      };


      //!
      template<class To, class From>
      com_ptr<To> com_ptr_cast(const com_ptr<From>& p)
      {
        if (!p)
          return nullptr;

        com_ptr<To> ptr;
        HRESULT hr = p.get()->QueryInterface(__uuidof(To), (void**)&ptr);
        if (FAILED(hr))
          return nullptr;

        return ptr;
      }

      //! 
      enum class class_context
      {
        inproc,
        server,
        all
      };

      //!
      template<class T>
      com_ptr<T> make_com_ptr(const CLSID& clsid, IUnknown* p_unk = nullptr, class_context ctx = class_context::all)
      {
        DWORD _ctx = CLSCTX_ALL;
        if (ctx == class_context::inproc)
          _ctx = CLSCTX_INPROC;
        else if (ctx == class_context::server)
          _ctx = CLSCTX_SERVER;

        com_ptr<T> ptr;
        HRESULT hr = ::CoCreateInstance(clsid, p_unk, _ctx, __uuidof(T), (void**)&ptr);
        if (FAILED(hr))
          return nullptr;

        return ptr;
      }


    }
  }
}

#endif