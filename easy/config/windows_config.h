/*!
 *  @file   easy/config/windows_config.h
 *  @author Sergey Tararay
 *  @date   2013
 */
#ifndef EASY_CONFIG_WINDOWS_CONFIG_H_INCLUDED
#define EASY_CONFIG_WINDOWS_CONFIG_H_INCLUDED


#ifndef EASY_CONFIG_H_INCLUDED
#  error "This is not intended to be included directly."
#endif


#define EASY_WINDOWS_XP    0x0502
#define EASY_WINDOWS_VISTA 0x0600
#define EASY_WINDOWS_7     0x0601
#define EASY_WINDOWS_8     0x0602


#define EASY_WIN_VERSION EASY_WINDOWS_XP

#ifdef WINVER
#  undef WINVER
#endif

#ifdef _WIN32_WINNT
#  undef _WIN32_WINNT
#endif

#define WINVER        EASY_WIN_VERSION
#define _WIN32_WINNT  EASY_WIN_VERSION

#pragma warning(error : 4715) // not all control paths return a value
#pragma warning(disable : 4996) // 'function': was declared deprecated

#endif