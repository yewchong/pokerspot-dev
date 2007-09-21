#ifndef __network_pdudownloadrequest_h__
#define __network_pdudownloadrequest_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDUDownloadRequest
  Created  : 01/30/2000

  OVERVIEW : Download Request PDU.

             When the lounge server detects that the
             client has old versions of files, it sends
             a Download Request PDU that tells the
             client where to download new versions of
             the outdated files.

 ****************************************************************************/

#include "network/network.h"
#include "network/pdumoduleversion.h"


BEGIN_NAMESPACE(Network)

struct PDUDownloadRequest : public PDUHeader
{
  // Originated from server

  u_int16_t numdownloads_; // number of total requests

  // Data contains numdownloads_ * DownloadEntry
  struct DownloadEntry
  {
    u_byte_t module_[MODULE_SIZE];
    u_int32_t modulesize_;
    u_int16_t urlsize_;
    message_t url_;

    DownloadEntry()
      :
      modulesize_ (0),
      urlsize_    (0),
      url_        (0)
    {
      memset(module_, 0, sizeof(module_));
    }

    ~DownloadEntry()
    {
      delete [] url_;
      url_ = NULL;
    }
  };
  DownloadEntry* pDownloads_;

  void execute(GameState*);
  void extractDownloadEntries(u_int16_t* buf);
  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int16_t); }

  u_byte_t* write(char* buf) const
  {    
    assert(false); // not implemented
    u_int16_t* pint = (u_int16_t*)PDUHeader::write(buf);
    return (u_byte_t*)pint;
  }

  PDUDownloadRequest(const char* username,
                     const char* password,
                     DownloadEntry* pDownloads,
                     int numdownloads)
    :
    PDUHeader    (PDU_DownloadRequest,
                  sizeOf(),
                  username, password),
    numdownloads_(numdownloads),
    pDownloads_  (pDownloads)
  {
    pDownloads_ = new DownloadEntry[numdownloads_];
    if (pDownloads_)
      memcpy(pDownloads_, pDownloads, numdownloads * sizeof(DownloadEntry));
  }

  PDUDownloadRequest(const char* buf)
    :
    PDUHeader    (buf),
    numdownloads_(0),
    pDownloads_  (0)
  {
    u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];

    // Rest of the data is download entries
    extractDownloadEntries((u_int16_t*)&buf[PDU_HEADER_SIZE]);
  }

  ~PDUDownloadRequest()
  {
    delete [] pDownloads_;
    pDownloads_ = NULL;
  }
    
  void checkValid()
  {
    PDU_DEBUG("--- Recv: Download Request PDU ---");
    PDUHeader::checkValid();
#ifdef PCLIENT_MODULE_
    CString s;
    s.Format("  download requests: %d", numdownloads_);
    PDU_DEBUG((LPCSTR)s);
#endif
  }
};


END_NAMESPACE(Network)

#endif