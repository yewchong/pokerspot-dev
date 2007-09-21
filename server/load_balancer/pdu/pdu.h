#ifndef __cwg_pdu_h__
#define __cwg_pdu_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CPDU
  Created  : 09/21/2001

  OVERVIEW : PDU (Protocol Data Unit) base class.

 ****************************************************************************/


#include "pdu/pduallocator.h"
#include "common/pdutypes.h"


#define MAXBUFFERSIZE		1024 // Maximum PDU size!
#define PDU_STRINGSIZE		16

// Every PDU starts with a header
struct pduh_t
{
	u_int16_t type;
	u_int16_t length;
};

#define PDU_HEADER_SIZE (sizeof(pduh_t))
#define PDU_HEADERSIZE (sizeof(pduh_t))

class CLounge;


class CPDU
{
public:
  CPDU();
  virtual ~CPDU();
  
  u_int16_t   getType() const;
  u_int16_t   getLength() const;
  const char* getValue() const;
  bool        isPDU(u_int16_t pdu_type) const;
  
  static int   Peek(SOCKET);
  static CPDU* Read(SOCKET, u_int16_t pdu_type);
  static void  RegisterAllocator(u_int16_t pdu_type, PDUAllocator*);

  
  virtual void grok(SOCKET, CLounge*);
  virtual int  readData(SOCKET);
  virtual int  writeData(SOCKET);
  
  void stuffHeader();
  int sendTo(SOCKET);
  int broadcast(SOCKET except_this_sd = INVALID_SOCKET);
  
protected:
  u_int16_t type_;
  u_int16_t length_;
  char value_[MAXBUFFERSIZE];
  
private:
  static CPDU* AllocPDU(u_int16_t pdu_type);
};


inline u_int16_t CPDU::getType() const
{
  return type_;
}

inline u_int16_t CPDU::getLength() const
{
  return length_;
}

inline const char* CPDU::getValue() const
{
  return value_;
}

inline bool CPDU::isPDU(u_int16_t pdu_type) const
{
  return type_ == pdu_type;
}

#endif
