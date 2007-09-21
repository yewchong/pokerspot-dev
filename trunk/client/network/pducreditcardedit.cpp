/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDUCreditCardEdit
  Created  : 04/24/2001

  OVERVIEW : Credit Card Edit PDU; sent by the lounge client
             to the server to specify credit card data.

 ****************************************************************************/


#include "stdafx.h"
#include "network/pducreditcardedit.h"
#include "loungedlg/resource.h"
#include "loungedlg/creditcarddlg.h"


BEGIN_NAMESPACE(Network)

struct pdu_ccdata
{
  u_int16_t typeIndex_;
  char digits_[16];
  char expmo_[2];
  char expyr_[2];
};



PDUCreditCardEdit::PDUCreditCardEdit(const std::list<CreditCardData>& d)
  :
  PDUHeader(PDU_Lounge_CreditCardEdit,
            PDU_HEADER_SIZE + sizeof(u_int16_t) + d.size() * sizeof(pdu_ccdata), 
            NULL, NULL),
  ccards_(d)
{}


size_t PDUCreditCardEdit::sizeOf() const
{
  return PDU_HEADER_SIZE +
           sizeof(u_int16_t) +
           ccards_.size() * sizeof(pdu_ccdata);
}


u_byte_t* PDUCreditCardEdit::write(char* buf) const
{    
#ifndef PSPOT_TABLE_MODULE_
  u_int16_t* pint = (u_int16_t*)PDUHeader::write(buf);
  *pint++         = htons(ccards_.size());
  buf = (char*)pint;

  // Serialize credit card datas into buffer
  for (std::list<CreditCardData>::const_iterator it = ccards_.begin(),
                                                 end = ccards_.end();
       it != end;
       ++it)
  {
    CreditCardData cc = (*it);
    pdu_ccdata ccdata;
    memset(&ccdata, 0, sizeof(ccdata));

    ccdata.typeIndex_ = htons(CCreditCardDlg::GetCCTypeIndex(cc.cctype_));

    for (int i = 0; i < 16; i++)
    {
      if (i >= cc.ccnumber_.GetLength())
        break;

      ccdata.digits_[i] = cc.ccnumber_[i];
    }

    if (cc.ccexpmo_.GetLength() == 2)
    {
      ccdata.expmo_[0] = cc.ccexpmo_[0];
      ccdata.expmo_[1] = cc.ccexpmo_[1];
    }

    if (cc.ccexpyr_.GetLength() == 4)
    {
      ccdata.expyr_[0] = cc.ccexpyr_[2];
      ccdata.expyr_[1] = cc.ccexpyr_[3];
    }

    memcpy(buf, &ccdata, sizeof(ccdata));
    buf += sizeof(ccdata);
  }
#endif // ifndef PSPOT_TABLE_MODULE_
  return (u_byte_t*)buf;
}

END_NAMESPACE(Network)

