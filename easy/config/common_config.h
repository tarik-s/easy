/*!
 *  @file   easy/config/common_config.h
 *  @author Sergey Tararay
 *  @date   2013
 */
#ifndef EASY_CONFIG_COMMON_CONFIG_H_INCLUDED
#define EASY_CONFIG_COMMON_CONFIG_H_INCLUDED

/*!
 *  Operation system
 */

#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32)
#  define EASY_OS_WINDOWS
#  define EASY_WIN32
#elif defined(__WIN64__) || defined(_WIN64) || defined(WIN64)
#  define EASY_OS_WINDOWS
#  define EASY_WIN64
#else
#  error "Unsupported OS"
#endif

/*!
 *  Compiler type and its version
 */

#if defined(_MSC_VER)

#  define EASY_MSVC_VERSION _MSC_VER

#  define EASY_MSVC_2010 1600
#  define EASY_MSVC_2012 1700

#if EASY_MSVC_VERSION < EASY_MSVC_2010
#  error "We do not support versions of Visual Studio earlier then 2010"
#elif EASY_MSVC_VERSION > EASY_MSVC_2012
#  error "The version of Visual Studio is higher then we know"
#endif

#elif defined (__GNUC__)
#  define EASY_GCC
#else
#  error "Unknown compiler"
#endif

//////////////////////////////////////////////////////////////////////////

#if defined(EASY_MSVC_VERSION)
#  define EASY_HAS_UNDERLYING_TYPE
#  define EASY_HAS_FINAL_KEYWORD
#  define EASY_HAS_OVERRIDE_KEYWORD
//#  define EASY_HAS_NESTED_EXCEPTION
#elif defined (EASY_GCC)
#  define EASY_HAS_NOEXCEPT
#  define EASY_HAS_EXPLICIT_OPERATOR
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


#ifdef _DEBUG
#  define EASY_ASSERT(condition) /*assert(condition)*/
#else
#  define EASY_ASSERT(condition)
#endif

#define EASY_TEST_BOOL(Value)   \
  {                             \
    if (!Value)                 \
      throw std::exception(""); \
  }



#define EASY_STATIC_ASSERT(condition, message) \
  static_assert(condition, message)

#define EASY_HAS_WCAHR

//////////////////////////////////////////////////////////////////////////

// boost definitions

#define BOOST_SYSTEM_NO_DEPRECATED

//////////////////////////////////////////////////////////////////////////

#endif