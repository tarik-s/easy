#ifndef EASY_WINDOWS_SYNCHRONIZATION_H_INCLUDED
#define EASY_WINDOWS_SYNCHRONIZATION_H_INCLUDED

#include <easy/windows/config.h>

#include <boost/noncopyable.hpp>

#include <windows.h>

namespace easy {
namespace windows {

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


  class critical_section
    : boost::noncopyable 
  {
  public:
    critical_section();
    ~critical_section();

    void lock();
    void unlock();
  private:
    RTL_CRITICAL_SECTION m_cs;
  };
  typedef lock<critical_section> critical_section_lock;


}}

#endif