#ifndef CB__BUFFER_H
#define CB__BUFFER_H

#include <algorithm>

namespace Colosseum
{

template<class TYPE=char>
class Buffer
{
public:
  Buffer (unsigned int size = 0) ;
  Buffer (const Buffer &source) ;
  ~Buffer () ;
  Buffer &operator=(const Buffer &source) ;
  TYPE &operator [](unsigned int) ;
  const TYPE &operator[] (unsigned int) const ;
  unsigned int getSize () const ;
private:
  TYPE *data_buffer ;
  unsigned int buffer_size ;
} ;

template<class TYPE>
inline Buffer<TYPE>::Buffer (unsigned int size)
: buffer_size (size),
  data_buffer (size != 0 ? new TYPE [size] : 0)
{
}

template<class TYPE>
inline Buffer<TYPE>::Buffer (const Buffer &source)
: buffer_size (source.buffer_size),
  data_buffer (source.buffer_size != 0 ? new TYPE [source.buffer_size] : 0)
{
  if (buffer_size != 0)
    copy (source.data_buffer, source.data_buffer + buffer_size, data_buffer) ;
  return ;
}

template<class TYPE>
inline Buffer<TYPE>::~Buffer ()
{
  delete [] data_buffer ; data_buffer = 0 ;
}


template<class TYPE>
inline Buffer<TYPE> &Buffer<TYPE>::operator=(const Buffer<TYPE> &source)
{
  delete [] data_buffer ; data_buffer = 0 ;
  buffer_size = source.buffer_size ;
  if (buffer_size != 0)
  {
    data_buffer = new TYPE [buffer_size] ;
    copy (source.data_buffer, source.data_buffer + buffer_size, data_buffer) ;
  }
  return ;
}

template<class TYPE>
TYPE &Buffer<TYPE>::operator [](unsigned int index)
{
  return data_buffer [index] ;
}

template<class TYPE>
const TYPE &Buffer<TYPE>::operator[] (unsigned int) const
{
  return data_buffer [index] ;
}

template<class TYPE>
unsigned int Buffer<TYPE>::getSize () const
{
  return buffer_size ;
}

} 


#endif
                                    
