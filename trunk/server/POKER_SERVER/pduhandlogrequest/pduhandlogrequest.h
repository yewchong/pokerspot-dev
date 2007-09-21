#ifndef __table_pduhandlogrequest_h__
#define __table_pduhandlogrequest_h__

/*******************************************************************************

    NAME:       pduShowCards
    DATE:       5/13/00
    PURPOSE:    Receive-only Handlog Request PDU

*******************************************************************************/

#include "../pduhandler/pduhandler.h"
#include <string>

struct pdu_handlogrequest
{
  u_int16_t numhands;
  u_int16_t addrlen;
};

class CpduHandlogRequest : public Cpdu
{
public:
    CpduHandlogRequest();

    int readData(SOCKET);
    void grok(SOCKET, CTable*);

    u_int16_t getNumHands() const;
    u_int16_t getAddrLen() const;
    const char* getAddress() const;

private:
    string address_;
};

inline u_int16_t CpduHandlogRequest::getNumHands() const
{
    pdu_handlogrequest* pdu = (pdu_handlogrequest*)(value_ + PDU_HEADER_SIZE);
    return ntohs(pdu->numhands);
}

inline u_int16_t CpduHandlogRequest::getAddrLen() const
{
    pdu_handlogrequest* pdu = (pdu_handlogrequest*)(value_ + PDU_HEADER_SIZE);
    return ntohs(pdu->addrlen);
}

inline const char* CpduHandlogRequest::getAddress() const
{
    return address_.c_str();
}



#endif