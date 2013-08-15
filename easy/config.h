/*!
 * \file   easy/config.h
 * \author Sergey Tararay
 * \date   04.08.2013
 *
 * FILE DESCRIPTION
 */
#ifndef EASY_CONFIG_H_INCLUDED
#define EASY_CONFIG_H_INCLUDED


#define BOOST_SYSTEM_DYN_LINK




#define EASY_OS_WINDOWS

#define EASY_PURE_VIRTUAL = 0

#define EASY_NOEXCEPT throw()

#define EASY_ASSERT(condition)

#define EASY_STATIC_ASSERT(condition, message) static_assert(condition, message)

#define EASY_HAVE_WCAHR


#endif