/*!
 * \file   easy/config.h
 * \author Sergey Tararay
 * \date   04.08.2013
 *
 * FILE DESCRIPTION
 */
#ifndef EASY_CONFIG_H_INCLUDED
#define EASY_CONFIG_H_INCLUDED


//////////////////////////////////////////////////////////////////////////

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#  define EASY_OS_WINDOWS
#else
#  error "Unsupported OS"
#endif

//////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#  define EASY_MSVC
#elif defined (__GNUC__)
#  define EASY_GCC
#else
#  error "Unknown compiler"
#endif

//////////////////////////////////////////////////////////////////////////

#if defined(EASY_MSVC)
#  define EASY_HAS_UNDERLYING_TYPE
#  define EASY_HAS_FINAL_KEYWORD
#  define EASY_HAS_OVERRIDE_KEYWORD
#elif defined (EASY_GCC)
#  define EASY_HAS_NOEXCEPT
#endif

//////////////////////////////////////////////////////////////////////////

#ifdef EASY_HAS_NOEXCEPT
#  define EASY_NOEXCEPT noexcept
#else
#  define EASY_NOEXCEPT
#endif

//////////////////////////////////////////////////////////////////////////

#ifdef EASY_HAS_FINAL_KEYWORD
#  define EASY_FINAL final
#else
#  define EASY_FINAL
#endif

#define EASY_FINAL_NOEXCEPT EASY_FINAL EASY_NOEXCEPT

//////////////////////////////////////////////////////////////////////////

#ifdef EASY_HAS_OVERRIDE_KEYWORD
#  define EASY_OVERRIDE override
#else
#  define EASY_OVERRIDE
#endif

#define EASY_OVERRIDE_NOEXCEPT EASY_OVERRIDE EASY_NOEXCEPT

//////////////////////////////////////////////////////////////////////////

#define EASY_PURE_VIRTUAL = 0



#define EASY_ASSERT(condition)

#define EASY_STATIC_ASSERT(condition, message) \
  static_assert(condition, message)

#define EASY_HAS_WCAHR


//////////////////////////////////////////////////////////////////////////

#define BOOST_SYSTEM_NO_DEPRECATED

//////////////////////////////////////////////////////////////////////////


#endif
