/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : struct PDUTournamentDetailsReply
  Created  : 07/07/2000

  OVERVIEW : Lounge Tournament Details Reply PDU.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pdutournamentdetailsreply.h"
#include "loungedlg/resource.h"
#include "loungedlg/loungedlg.h"
#include "loungedlg/tournamentdetailsdlg.h"

#ifdef _DEBUG
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

BEGIN_NAMESPACE(Network)


PDUTournamentDetailsReply::PDUTournamentDetailsReply(const char* buf)
  :
  PDUHeader  (buf),
  buyin_     (0),
  starttime_ (0),
  numplayers_(0)
{
  CChips::chip_amount_t* pchips = (CChips::chip_amount_t*)&buf[PDU_HEADER_SIZE];
  buyin_ = CChips_n(*pchips++).ntohl();

  u_int16_t* pint = (u_int16_t*)pchips;

  numplayers_ = ntohs(*pint++);
  u_int32_t* plong = (u_int32_t*)pint;
  starttime_ = ntohl(*plong++);
  u_byte_t* pbyte = (u_byte_t*)plong;

  size_t msglen = length_ - sizeOf();
  if (msglen > 0)
  {
    descr_ = new char[msglen + 1];
    if (descr_)
    {
      memcpy(descr_, pbyte, msglen);
      descr_[msglen] = '\0';
    }
  }
}


PDUTournamentDetailsReply::~PDUTournamentDetailsReply()
{
  delete [] descr_;
  descr_ = NULL;
}


void PDUTournamentDetailsReply::execute(GameState*)
{
#ifndef PSPOT_TABLE_MODULE_
  CWnd* pWnd = AfxGetMainWnd();
  if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CLoungeDlg)))
  {
    CTournamentDetailsDlg* pDetailsDlg =
      static_cast<CLoungeDlg*>(pWnd)->getTournamentDlg();
    if (pDetailsDlg)
      populateDialog(pDetailsDlg);
  }
#endif
}


void PDUTournamentDetailsReply::populateDialog(CTournamentDetailsDlg* pDlg)
{
#ifndef PSPOT_TABLE_MODULE_
  pDlg->setData(buyin_, starttime_, numplayers_, descr_);
#endif
}


END_NAMESPACE(Network)
