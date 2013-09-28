/*!
 *  @file   easy/windows/synchronization.h
 *  @author Sergey Tararay
 *  @date   2013
 */
#ifndef EASY_WINDOWS_SYNCHRONIZATION_H_INCLUDED
#define EASY_WINDOWS_SYNCHRONIZATION_H_INCLUDED

#include <easy/windows/config.h>

#include <boost/noncopyable.hpp>

#include <windows.h>

namespace easy {
namespace windows 
{

  template<class Lockable>
  class lock
  {
  public:
    typedef Lockable lockable;

    lock(lockable& l)
      : m_lockable(l) 
    {
      m_lockable.lock();
    }

    ~lock() {
      m_lockable.unlock();
    }
  private:
    lockable& m_lockable;
  };

  //! Wrapper over system CRITICAL_SECTION
  class critical_section
    : boost::noncopyable 
  {
  public:
    //! Constructor. Creates a critical section.
    critical_section();

    //! Destructor. Destroys the critical section.
    ~critical_section();

    //! Acquires the critical section
    void lock();

    //! Releases the critical section
    void unlock();
  private:
    RTL_CRITICAL_SECTION m_cs;
  };
  typedef lock<critical_section> critical_section_lock;

}}

#endif