/*!
 *  @file   easy/types.h
 *  @author Sergey Tararay
 *  @date   2013
 */
#ifndef EASY_TYPES_H_INCLUDED
#define EASY_TYPES_H_INCLUDED

#include <easy/config.h>

#include <vector>
#include <string>

namespace easy
{
  typedef char int8;
  typedef unsigned char uint8;
  typedef uint8 byte;

  typedef short int16;
  typedef unsigned short uint16;

  typedef unsigned int uint;
  typedef int int32;  
  typedef uint uint32;

  typedef __int64 int64;
  typedef unsigned __int64 uint64;

  typedef std::vector<byte> byte_vector;

  typedef std::vector<std::string> string_list;
  typedef std::vector<std::wstring> wstring_list;


  
}

#endif