/*!
 *  @file   easy/object.h
 *  @author Sergey Tararay
 *  @date   2013
 */
#ifndef EASY_OBJECT_H_INCLUDED
#define EASY_OBJECT_H_INCLUDED

#include <easy/config.h>

#include <easy/stlex/nullptr_t.h>
#include <easy/types.h>
#include <easy/safe_bool.h>

#include <memory>

#include <boost/noncopyable.hpp>

namespace easy
{
  //! scoped_tag
  struct scoped_tag { };

  //! shared_tag
  struct shared_tag { };
  
  //! scoped_object_holder
  template<class Traits>
  class scoped_object_holder
    : boost::noncopyable
    , public scoped_tag
  {
  public:
    typedef Traits                            traits_type;
    typedef typename traits_type::object_type object_type;

    class tracker
    {
    public:
      tracker(const scoped_object_holder& obj)
        : m_obj(obj) {
      }
      object_type get() const {
        return m_obj.get_object();
      }
    private:
      const scoped_object_holder& m_obj;
    };

    explicit scoped_object_holder(object_type obj = traits_type::get_invalid_object()) EASY_NOEXCEPT
      : m_obj(obj) {
    }

    ~scoped_object_holder() EASY_NOEXCEPT {
      reset_object();
    }

    object_type get_object() const EASY_NOEXCEPT {
      return m_obj;
    }

    object_type release_object() EASY_NOEXCEPT {
      object_type obj = m_obj;
      m_obj = traits_type::get_invalid_object();
      return obj;
    }

    void reset_object(object_type obj = traits_type::get_invalid_object()) EASY_NOEXCEPT {
      if (obj != m_obj) {
        if (traits_type::is_valid(m_obj)) {
          error_code ec;
          traits_type::close_object(m_obj, ec);
          EASY_ASSERT(!ec);
        }
        m_obj = obj;
      }
    }

    bool operator ! () const EASY_NOEXCEPT {
      return !traits_type::is_valid(m_obj);
    }

  protected:
    void swap_impl(scoped_object_holder& r) EASY_NOEXCEPT {
      std::swap(m_obj, r.m_obj);
    }
  private:
    object_type m_obj;
  };

  //! shared_object_holder
  template<class Traits>
  class shared_object_holder
    : public shared_tag
  {
  public:
    typedef Traits                            traits_type;
    typedef typename traits_type::object_type object_type;  

    class tracker
    {
    public:
      tracker(const shared_object_holder& obj)
        : m_obj(obj) {
      }
      object_type get() const {
        return m_obj.get_object();
      }
    private:
      const shared_object_holder& m_obj;
    };

    explicit shared_object_holder(object_type obj = traits_type::get_invalid_object())
    {
      if (traits_type::is_valid(obj))
        m_impl = std::make_shared<holder_type>(obj);
    }

    object_type get_object() const EASY_NOEXCEPT {
      return m_impl ? m_impl->get_object() : traits_type::get_invalid_object();
    }

    void reset_object(object_type obj = traits_type::get_invalid_object()) {
      if (obj != get_object()) {
        if (m_impl)
          m_impl->reset_object(obj);
        else
          *this = shared_object_holder(obj);
      }
    }

    bool operator ! () const EASY_NOEXCEPT {
      return !m_impl || !*m_impl;
    }
  protected:
    void swap_impl(shared_object_holder& r) EASY_NOEXCEPT {
      m_impl.swap(r);
    }
  private:
    typedef scoped_object_holder<traits_type> holder_type;
    typedef std::shared_ptr<holder_type> holder_ptr;
  private:
    holder_ptr m_impl;
  };

  //! basic_object
  template<class Impl>
  class basic_object
    : public Impl
    , public safe_bool<basic_object<Impl>>
  {
  public:
    typedef Impl                              base;
    typedef basic_object<base>                this_type;   
    typedef typename base::traits_type        traits_type;
    typedef typename traits_type::object_type object_type; 

  public:
    basic_object() EASY_NOEXCEPT {
    }

    basic_object(nullptr_t) EASY_NOEXCEPT {
    }

    explicit basic_object(object_type obj)
    {
      reset_object(obj);
    }

    template<class A1>
    explicit basic_object(A1 && a1, error_code_ref ec = nullptr)
    {
      reset_object(base::construct(
        std::forward<A1>(a1), 
        ec));
    }

    template<class A1, class A2>
    basic_object(A1 && a1, A2 && a2, error_code_ref ec = nullptr)
    {
      reset_object(base::construct(
        std::forward<A1>(a1),
        std::forward<A2>(a2),
        ec));
    }

    template<class A1, class A2, class A3>
    basic_object(A1 && a1, A2 && a2, A3 && a3, error_code_ref ec = nullptr)
    {
      reset_object(base::construct(
        std::forward<A1>(a1),
        std::forward<A2>(a2),
        std::forward<A3>(a3),
        ec));
    }

    template<class A1, class A2, class A3, class A4>
    basic_object(A1 && a1, A2 && a2, A3 && a3, A4 && a4, error_code_ref ec = nullptr)
    {
      reset_object(base::construct(
        std::forward<A1>(a1),
        std::forward<A2>(a2),
        std::forward<A3>(a3),
        std::forward<A4>(a4),
        ec));
    }


    basic_object(this_type && r) EASY_NOEXCEPT
    {
      *this = std::forward<this_type>(r);
    }

    basic_object& operator = (this_type && r) EASY_NOEXCEPT {
      if (this != &r)
        swap(r);
      return *this;
    }

    basic_object& operator = (nullptr_t) EASY_NOEXCEPT {
      reset_object();
      return *this;
    }

    void swap(this_type& r) EASY_NOEXCEPT {
      swap_impl(r);
    }
  };

  template<class T>
  struct is_object_handle
    : boost::false_type {
  };

  template<class ObjectImpl>
  typename ObjectImpl::object_type get_object_handle(const basic_object<ObjectImpl>& obj) 
  {
    return obj.get_object();
  }

}

template<class ObjectHandle>
ObjectHandle get_object_handle(const ObjectHandle& obj, typename boost::enable_if<easy::is_object_handle<ObjectHandle>>::type* = nullptr) 
{
  return obj;
}

#endif