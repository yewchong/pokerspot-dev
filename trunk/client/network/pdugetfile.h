#ifndef __network_pdugetfile_h__
#define __network_pdugetfile_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDUGetFile
  Created  : 092200

  OVERVIEW : Upgrade PDU: client requests for one file.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)

struct PDUGetFile : public PDUHeader
{
  // Originated from client 
  message_t file_name_;

  PDUGetFile(const char* fname)
    :
    PDUHeader(PDU_Upgrade_GetFile,
              sizeOf(),
              NULL, NULL),
    file_name_(NULL)
  {
    int numBytes = strlen(fname);
    if (numBytes > 0)
    {
      file_name_ = new char[numBytes + 1];
      if (file_name_)
      {
        memcpy(file_name_, (char*)pbyte, numBytes);
        file_name_[numBytes] = '\0';
      }
    }
  }

  virtual u_byte_t* write(char* buffer) const
  {
    u_byte_t* pbyte = (u_byte_t*)PDUHeader::write(buffer);

    if (file_name_)
    {
        memcpy(pbyte, file_name_, strlen(file_name_));
        pbyte += strlen(file_name_);
    }
    *pbyte++ = '\0';
    return pbyte;
  }


  ~PDUGetFile()
  {
    delete [] file_name_;
    file_name_ = NULL;
  }    
};


END_NAMESPACE(Network)

#endif