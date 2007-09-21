#ifndef __lounge_pdu_h__
#define __lounge_pdu_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : Class Cpdu
  Created  : 04/16/2000

  OVERVIEW : PDU base class.

 ****************************************************************************/


#include "includes.h"
#include "pduallocator.h"

#define MAXBUFFERSIZE		1024 // Maximum PDU size!
#define PDU_STRINGSIZE		16

class CPlayer;
class CTable;
class CLoadBalancer;

struct pduh_t
{
	u_int16_t type;
	u_int16_t length;
};

#define PDU_HEADER_SIZE (sizeof(pduh_t))
#define PDU_HEADERSIZE (sizeof(pduh_t))

class Cpdu
{
public:
  Cpdu();
  Cpdu(int type, int length, const char* data);
  virtual ~Cpdu();
  
  u_int16_t getType() const;
  u_int16_t getLength() const;
  const char* getValue() const;
  
  static int Peek(SOCKET);
  static Cpdu* Read(SOCKET, u_int16_t pdu_type);
  static void RegisterAllocator(u_int16_t pdu_type, PduAllocator*);
  bool isPdu(u_int16_t pdu_type) const;
  
  virtual void grok(SOCKET, CPlayer*);
  virtual void grok(SOCKET, CTable*);
  virtual void grok(SOCKET, CLoadBalancer*);
  virtual int  readData(SOCKET);
  virtual int  writeData(SOCKET);
  virtual int  setData(u_int16_t type,
                       u_int16_t length,
                       const char* data);
  
  void stuffHeader();
  int sendTo(SOCKET);
  int sendTo(CPlayer*);
  int sendTo(CTable*);
  int broadcastToPlayers(CPlayer* except = NULL);

  int sendToLoadBalancer();
  int sendToLounges();
  
protected:
  u_int16_t type_;
  u_int16_t length_;
  char value_[MAXBUFFERSIZE];
  
protected:
  static Cpdu* AllocPdu(u_int16_t pdu_type);
};


inline u_int16_t Cpdu::getType() const
{
  return type_;
}

inline u_int16_t Cpdu::getLength() const
{
  return length_;
}

inline const char* Cpdu::getValue() const
{
  return value_;
}

inline bool Cpdu::isPdu(u_int16_t pdu_type) const
{
  return type_ == pdu_type;
}

#endif
