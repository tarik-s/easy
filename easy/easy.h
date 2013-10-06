/*!
 * @file   easy/easy.h
 * @author Sergey Tararay
 * @date   2013
 *
 * @brief Includes all headers the library exposes
 */
#ifndef EASY_EASY_H_INCLUDED
#define EASY_EASY_H_INCLUDED

#include <easy/config.h>

#include <easy/stlex/stlex.h>

#include <easy/types.h>
#include <easy/safe_bool.h>
#include <easy/safe_call.h>
#include <easy/error_handling.h>
#include <easy/flags.h>
#include <easy/strings.h>
#include <easy/scope.h>
#include <easy/range.h>
#include <easy/object.h>
#include <easy/lite_buffer.h>
#include <easy/os.h>

#include <easy/db/db.h>

#ifdef EASY_OS_WINDOWS
#include <easy/windows/windows.h>
#endif


#endif

/*!
 * @namespace easy
 * @brief Main namespace of the Easy Library
 */

/*!
 * @mainpage Easy Library
 *
 * Easy is just a set of ideas written with C++
 *
 * - @subpage page_windows
 */