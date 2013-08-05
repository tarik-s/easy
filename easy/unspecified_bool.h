/*!
 * \file   easy/unspecified_bool.h
 * \author Sergey Tararay
 * \date   04.08.2013
 *
 * FILE DESCRIPTION
 */
#ifndef EASY_UNSPECIFIED_BOOL_H_INCLUDED
#define EASY_UNSPECIFIED_BOOL_H_INCLUDED

// Dependencies
#include <easy/detail/unspecified_bool_detail.h>


namespace easy
{
  /*!
 * \brief A type convertible to bool
 */
  typedef detail::unspecified_bool_type unspecified_bool_type;

  /*!
 * \brief value which is to be recognized as true value
 */
	inline unspecified_bool_type unspecified_true() {
		return &detail::unspecified::true_value;
	}
}


#endif