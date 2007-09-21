/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUCreditCardTypes
  Created  : 04/24/2001

  OVERVIEW : Credit Card Types PDU; sent by the lounge server
             to the client to enumerate supported credit card types.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pducreditcardtypes.h"
#ifndef PSPOT_TABLE_MODULE_
#include "loungedlg/cashierdlg.h"
#include "loungedlg/creditcarddlg.h"
#endif


BEGIN_NAMESPACE(Network)


PDUCreditCardTypes::PDUCreditCardTypes(const char* buf)
  :
  PDUHeader (buf),
  num_types_(0)
{
#ifndef PSPOT_TABLE_MODULE_
  u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];
  num_types_ = ntohs(*pint++);

  // Rest of the data is credit card type strings
  // Extract strings from network data and save them
  // to list 

  if (num_types_ > 0)
  {
    const int MaxName = 1024;
    char* namebuf = new char[MaxName];

    if (namebuf)
    {
      for (int i = 0; i < num_types_; i++)
      {
        u_int16_t length = ntohs(*pint++);
        u_byte_t* pbyte = (u_byte_t*)pint;
        if (length < MaxName)
        {
          memcpy(namebuf, pbyte, length);
          namebuf[length] = 0;
          pbyte += length;
        }
        else
        {
          memcpy(namebuf, pbyte, MaxName-1);
          namebuf[MaxName-1] = 0;
          pbyte += length;
        }

        string cctype = (char*)namebuf;
        cctypes_.push_back(cctype);

        pint = (u_int16_t*)pbyte;
      }

      delete [] namebuf;
    }
  }
#endif
}


void PDUCreditCardTypes::execute(GameState*)
{
#ifndef PSPOT_TABLE_MODULE_
  CCreditCardDlg::AddCreditCardTypes(cctypes_);
#endif
}


END_NAMESPACE(Network)

