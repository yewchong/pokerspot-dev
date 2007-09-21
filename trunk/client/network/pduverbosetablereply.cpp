/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : struct PDUVerboseTableReply
  Created  : 02/25/2000

  OVERVIEW : Lounge Verbose Table Reply PDU.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pduverbosetablereply.h"
#include "loungedlg/resource.h"
#include "loungedlg/loungedlg.h"
#include "loungedlg/tabledetailsdlg.h"
#include "common/stringoutput.h"

#ifdef _DEBUG
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

BEGIN_NAMESPACE(Network)


PDUVerboseTableReply::PDUVerboseTableReply(const char* buf)
  :
  PDUHeader    (buf),
  avg_pot_size_(0),
  flop_prct_   (0),
  num_players_ (0),
  pPlayers_    (0)
{
  PDU_ASSERT(sizeof(PlayerInfo) == 36);

  u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];

  // The first field is not used by client
  unused_ = *pint++;

  CChips::chip_amount_t* pchips = (CChips::chip_amount_t*)pint;
  avg_pot_size_ = CChips_n(*pchips++).ntohl();

  pint = (u_int16_t*)pchips;
  flop_prct_ = ntohs(*pint++);
  num_players_ = ntohs(*pint++);

  // Rest of the data is PlayerInfos - extract
  // from the buffer
  size_t extraBytes = length_ - sizeOf();
  u_int16_t numplayers = extraBytes / sizeof(PlayerInfo);

  PDU_ASSERT(numplayers == num_players_); 
  num_players_ = numplayers;

  if (num_players_ > 0)
    pPlayers_ = new PlayerInfo[num_players_];

  if (pPlayers_)
  { 
    memset(pPlayers_, 0, num_players_ * sizeof(PlayerInfo));
    PlayerInfo* pP = pPlayers_;

    for (int i = 0; i < num_players_; i++, pP++)
    {
      u_byte_t* pbyte = (u_byte_t*)pint;
      memcpy(pP->username_, pbyte, PDU_STRINGSIZE);
      pbyte += PDU_STRINGSIZE;
      memcpy(pP->homecity_, pbyte, PDU_STRINGSIZE);
      pbyte += PDU_STRINGSIZE;

      CChips::chip_amount_t* pchips = (CChips::chip_amount_t*)pbyte;
      pP->chips_ = *pchips++; // XXX NOTE: in network byte order

      pint = (u_int16_t*)pchips;
    }
  }      
}


PDUVerboseTableReply::~PDUVerboseTableReply()
{
  delete [] pPlayers_;
  pPlayers_ = 0;
}


void PDUVerboseTableReply::execute(GameState*)
{
#ifndef PSPOT_TABLE_MODULE_
  CWnd* pWnd = AfxGetMainWnd();
  if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CLoungeDlg)))
  {
    CTableDetailsDlg* pDetailsDlg =
      static_cast<CLoungeDlg*>(pWnd)->getDetailsDlg();
    if (pDetailsDlg)
      populateDialog(pDetailsDlg);
  }
#endif
}


void PDUVerboseTableReply::populateDialog(CTableDetailsDlg* pDlg)
{
#ifndef PSPOT_TABLE_MODULE_
  pDlg->setFinishedTitle();

  pDlg->setAvgPot(avg_pot_size_);
  pDlg->setFlopPrct(flop_prct_);
//  pDlg->setQueueSize(queue_size_);
  pDlg->setNumPlayers(num_players_);

  pDlg->resetListContents();

  PlayerInfo* pP = pPlayers_;
  if (pP != NULL)
  {
    CString n, c;
    CStrOut s;
    for (int i = 0; i < num_players_; i++)
    {
      n = MakeString(pP->username_);
      c = MakeString(pP->homecity_);
      CChips chips = CChips_n(pP->chips_).ntohl();
      s << chips;
      pDlg->addListItem(n, c, s.str());
      s.clear();
      pP++;
    }
  }
#endif
}


END_NAMESPACE(Network)
