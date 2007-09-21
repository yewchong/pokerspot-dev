#ifndef __table_pduhandler_h__
#define __table_pduhandler_h__

#include "includes.h"
#include "pduallocator.h"
#include "def.h"

class CTable;
class Cring;
class CPlayer;


#define PDU_HEADERSIZE		4
#define PDUSTRINGSIZE		16
#define PDU_STRINGSIZE		16

typedef struct pduh
{
	u_int16_t type;
	u_int16_t length;
} pduh_t;

#define PDU_HEADER_SIZE (sizeof(pduh_t))
#define PDU_NOT_SEQUENCED 0xffff

class Cpdu
{
public:
	Cpdu();
  virtual ~Cpdu();

  u_int16_t getType() const;
	u_int16_t getLength() const;
  const char* getValue() const;

  static int Peek(SOCKET);
  static Cpdu* Read(SOCKET, u_int16_t pdu_type);
  static void RegisterAllocator(u_int16_t pdu_type, PduAllocator*);
  bool isPdu(u_int16_t pdu_type) const;

  virtual void grok(SOCKET, CTable*);
  virtual int  readData(SOCKET);
  virtual int  readData(const char* buf);
  virtual int  writeData(SOCKET);
  virtual u_int16_t getSequenceNumber() const;

	int getHeader(SOCKET);
	int getData(SOCKET);
	bool peekHeader(SOCKET);
	bool peekData(SOCKET, size_t);
	void stuffHeader();

	int sendTo(SOCKET);
	int broadcastTo(Cring*, CPlayer* except = NULL);

// protected:
	u_int16_t type_;
	u_int16_t length_;
	char value_[MAXBUFFERSIZE];

public:
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
