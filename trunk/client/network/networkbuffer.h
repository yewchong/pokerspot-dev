#ifndef __psi_networkbuffer_h__
#define __psi_networkbuffer_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class Networkbuffer
  Created  : 24.12.1999

  OVERVIEW : Class for serializing data to/from a network-compatible buffer.

 ****************************************************************************/

#include "network/compat.h"
#include "network/ntwktype.h"

BEGIN_NAMESPACE(Network)


class ReadBuffer
{
public:
  ReadBuffer(const char* buf);

  const char* getbuf() { return (const char*)buf_; }
  const char* getpos() { return (const char*)pos_; }
  size_t getsize() const { return (pos_ - buf_); }

  ReadBuffer& operator >> (u_byte_t&);
  ReadBuffer& operator >> (u_int16_t&);
  ReadBuffer& operator >> (u_int32_t&);
  ReadBuffer& operator >> (string_t& s);

private:
  const u_byte_t* buf_;
  const u_byte_t* pos_;
};

inline
ReadBuffer& ReadBuffer::operator >> (u_byte_t& b)
{
  b = *pos_++;
  return *this;
}

inline
ReadBuffer& ReadBuffer::operator >> (u_int16_t& i)
{
  u_int16_t* pint = (u_int16_t*)pos_;
  i = ntohs(*pint++);
  pos_ = (u_byte_t*)pint;
  return *this;
}

inline
ReadBuffer& ReadBuffer::operator >> (u_int32_t& i)
{
  u_int32_t* pint = (u_int32_t*)pos_;
  i = ntohl(*pint++);
  pos_ = (u_byte_t*)pint;
  return *this;
}

inline
ReadBuffer& ReadBuffer::operator >> (string_t& s)
{
  memcpy(s, pos_, PDU_STRINGSIZE);
  pos_ += PDU_STRINGSIZE;
  return *this;
}


class WriteBuffer
{
public:
  WriteBuffer(char* buf);

  char*  getbuf() { return (char*)buf_; }
  char*  getpos() { return (char*)pos_; }
  size_t getsize() const { return (pos_ - buf_); }

  WriteBuffer& operator << (u_byte_t);
  WriteBuffer& operator << (u_int16_t);
  WriteBuffer& operator << (u_int32_t);
  WriteBuffer& operator << (const string_t& s);

private:
  u_byte_t* buf_;
  u_byte_t* pos_;
};


inline
WriteBuffer& WriteBuffer::operator << (u_byte_t b)
{
  *pos_++ = b;
  return *this;
}

inline
WriteBuffer& WriteBuffer::operator << (u_int16_t i)
{
  u_int16_t* pint = (u_int16_t*)pos_;
  *pint++ = htons(i);
  pos_ = (u_byte_t*)pint;
  return *this;
}

inline
WriteBuffer& WriteBuffer::operator << (u_int32_t i)
{
  u_int32_t* pint = (u_int32_t*)pos_;
  *pint++ = htonl(i);
  pos_ = (u_byte_t*)pint;
  return *this;
}

inline
WriteBuffer& WriteBuffer::operator << (const string_t& s)
{
  strncpy((char*)pos_, s, PDU_STRINGSIZE);
  pos_ += PDU_STRINGSIZE;
  return *this;
}

END_NAMESPACE(Network)

#endif