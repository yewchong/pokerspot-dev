/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduLoginAccept
  Created  : 04/18/2000

  OVERVIEW : Login accept pdu.

 ****************************************************************************/

#include "pduloginaccept.h"
#include "pdulogin.h"
#include "sys.h"


CpduLoginAccept::CpduLoginAccept(const char* username,
                                 const char* password,
                                 const char* motd)
{
    type_ = PDU_Lounge_Accept;
    length_ = PDU_HEADER_SIZE + sizeof(pdu_login);
    if (motd != NULL)
        length_ += strlen(motd) + 1;
    if (length_ < MAXBUFFERSIZE)
    {
        stuffHeader();

        pdu_login pdu;
        strncpy(pdu.username, username, PDU_STRINGSIZE);
        strncpy(pdu.password, password, PDU_STRINGSIZE);
        char* buf = (value_ + PDU_HEADER_SIZE);
        memcpy(buf, &pdu, sizeof(pdu));
        buf += sizeof(pdu_login);

        if (motd != NULL)
        {
            memcpy(buf, motd, strlen(motd));
            buf += strlen(motd);
            *buf = '\0';
        }
    }
    else
    {
        Sys_LogError("LoginAccept: buffer size too small");
    }
}
