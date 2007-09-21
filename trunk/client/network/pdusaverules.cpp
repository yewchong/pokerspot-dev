/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUSaveRules
  Created  : 08/29/2000

  OVERVIEW : 

 ****************************************************************************/

#include "stdafx.h"
#include "network/pdusaverules.h"

BEGIN_NAMESPACE(Network)

PDUSaveRules::PDUSaveRules(bool b, const char* buf)
  :
  PDUHeader(b ? PDU_Tournament_SaveRules : PDU_Tournament_SaveFormat,
            sizeOf(), NULL, NULL),
    flags_(0),
    data_ (0)
{
      int len = strlen(buf);
      data_ = new char[len + 1];
      if (data_)
      {
          memcpy(data_, buf, len);
          data_[len] = '\0';
          length_ = sizeOf() + len + 1;
      }
      else
      {
          AfxMessageBox("Out of memory");
      }
}


int PDUSaveRules::sendChunk(SOCKET sd)
{
    if (data_)
    {
        // XXX replace with loop
        return Network::SendPDU(sd, *this);
    }
    else
    {
        return 0;
    }
}


PDUSaveRules::~PDUSaveRules()
{
    if (data_)
    {
        delete [] data_;
        data_ = NULL;
    }
}

END_NAMESPACE(Network)