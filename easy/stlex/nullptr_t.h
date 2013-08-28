/*!
 * \file   easy/stlex/nullptr_t.h
 * \author Sergey Tararay
 * \date   20.08.2013
 *
 * FILE DESCRIPTION
 */
#ifndef EASY_STLEX_NULLPTR_T_INCLUDED_H
#define EASY_STLEX_NULLPTR_T_INCLUDED_H

#include <easy/config.h>

namespace easy
{
  typedef decltype(nullptr) nullptr_t;
}

#endif