#ifndef CB__ARRAY_H
#define CB__ARRAY_H
//
//  Title: Array Class
//
//  Author:  John M. Miano  miano@colosseumbuilders.com
//
//  Copyright 1999 Colosseum Builders, Inc.
//
//  Description:
//
//    The Array template class is a wrapper for the STL vector class 
//    that allows range checking to be enabled and disabled.
//
//    If the preprocessor symbol ENABLECHECKS is enabled range checking
//    is enabled.
//
//
//    THIS BASTARDIZED VERSION IS FOR MICROSOFT'S SORRY EXCUSE FOR A COMPILER.
//
#include <vector>
#include "checks.h"

namespace ColosseumPrivate
{
template<class TYPE>
class Array : public std::vector<TYPE>
{
public:
  typedef std::vector<TYPE> arraytype ;
  Array () {} 
  Array (size_t size) : arraytype (size) {} 
  Array (size_t size, const TYPE &value) : arraytype (size, value) {} 
  Array (const Array &source) : arraytype (source) {} 
  TYPE &operator[](size_t) ;
  const TYPE &operator[] (size_t) const ;
} ;

#if defined (ENABLECHECKS)
template<class TYPE>
inline TYPE &Array<TYPE>::operator[](size_t index)
{
  return vector<TYPE>::at (index) ;
}
template<class TYPE>
inline const TYPE &Array<TYPE>::operator[](size_t index) const
{
  return vector<TYPE>::at (index) ;
}
#else
template<class TYPE>
inline TYPE &Array<TYPE>::operator[](size_t index)
{
  return arraytype::operator[](index) ;
}
template<class TYPE>
inline const TYPE &Array<TYPE>::operator[](size_t index) const
{
  return arraytype::operator[](index) ;
}
#endif

}

#endif
