#ifndef __lounge_pdutablelogin_h__
#define __lounge_pdutablelogin_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduTableLogin
  Created  : 04/21/2000

  OVERVIEW : Table Login PDU - a table server connects
             to Lounge server using this PDU.

 ****************************************************************************/

#include "pdu.h"
#include "common/chips.h"

struct pdu_tablelogin
{
    u_int16_t             gametype;
    u_int16_t             tablenumber;
    CChips::chip_amount_t tablehigh;
    CChips::chip_amount_t tablelow;
    u_int16_t             tableport;
};


class CpduTableLogin : public Cpdu
{
public:
    CpduTableLogin();

    u_int16_t getGameType() const;
    u_int16_t getTableNumber() const;
    CChips    getTableHigh() const;
    CChips    getTableLow() const;
    u_int16_t getPort() const;

    void grok(SOCKET, CTable*);
};

inline u_int16_t CpduTableLogin::getGameType() const
{
    pdu_tablelogin* pdu = (pdu_tablelogin*)(value_ + PDU_HEADER_SIZE);
    return ntohs(pdu->gametype);
}

inline u_int16_t CpduTableLogin::getTableNumber() const
{
    pdu_tablelogin* pdu = (pdu_tablelogin*)(value_ + PDU_HEADER_SIZE);
    return ntohs(pdu->tablenumber);
}

inline CChips CpduTableLogin::getTableHigh() const
{
    pdu_tablelogin* pdu = (pdu_tablelogin*)(value_ + PDU_HEADER_SIZE);
    return CChips_n(pdu->tablehigh).ntohl();
}

inline CChips CpduTableLogin::getTableLow() const
{
    pdu_tablelogin* pdu = (pdu_tablelogin*)(value_ + PDU_HEADER_SIZE);
    return CChips_n(pdu->tablelow).ntohl();
}

inline u_int16_t CpduTableLogin::getPort() const
{
    pdu_tablelogin* pdu = (pdu_tablelogin*)(value_ + PDU_HEADER_SIZE);
    return ntohs(pdu->tableport);
}

#endif
