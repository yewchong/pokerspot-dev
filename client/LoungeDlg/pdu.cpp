#include "stdafx.h"
#include "network/pdus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//
// Dummy versions of the PDU execute functions
// for those PDUs that the Lounge client does
// not see.
//

namespace Network
{
  void PDUGameNumber::execute(Base::GameState*)
  {}

  void PDUDealCardsFlex::execute(Base::GameState*)
  {}

  PDUDealCardsFlex::~PDUDealCardsFlex()
  {}

  void PDUAllIn::execute(Base::GameState*)
  {}
  
  void PDUSetupTableAccept::execute(Base::GameState*)
  {}

  void PDUSetupBuyInQuery::execute(Base::GameState*)
  {}

  void PDUSetupTableReject::execute(Base::GameState*)
  {}

  void PDUSetupTableLogoutConfirm::execute(Base::GameState*)
  {}

  void PDUButton::execute(Base::GameState*)
  {}

  void PDUBlindCard::execute(Base::GameState*)
  {}

  void PDUPocketCard::execute(Base::GameState*)
  {}

  void PDUAction::execute(Base::GameState*)
  {}

  void PDUActionEcho::execute(Base::GameState*)
  {}

  void PDUActionRequest::execute(Base::GameState*)
  {}

  void PDUCommunityCard::execute(Base::GameState*)
  {}

  void PDUDealCards::execute(Base::GameState*)
  {}

  void PDUSitInQuery::execute(Base::GameState*)
  {}

  void PDUShowCardsQuery::execute(Base::GameState*)
  {}

  PDUPlayerInfo::PDUPlayerInfo(const char*)
  : PDUHeader(PDU_Setup_PlayerInfo, sizeOf(), NULL, NULL)
  {}

  PDUShowdown::PDUShowdown(const char*)
  : PDUHeader(PDU_Play_Showdown, sizeOf(), NULL, NULL)
  {}

  PDUPlayerInfo::~PDUPlayerInfo() {}

  PDUDealCardsFlex::PDUDealCardsFlex(const char*)
  : PDUHeader(PDU_Play_DealCardsFlex, sizeOf(), NULL, NULL)
  {}

  PDUHandLogRequest::PDUHandLogRequest(u_int16_t, const char*, int)
    : PDUHeader(PDU_Setup_HandLogRequest, sizeOf(), NULL, NULL)
  {}

  u_byte_t* PDUHandLogRequest::write(char*) const { return NULL; }

  void PDUPlayerInfo::execute(Base::GameState*)
  {}

  void PDUPlayer::execute(Base::GameState*)
  {}

  void PDUAnnounce::execute(Base::GameState*)
  {}

  void PDUBlindRequest::execute(Base::GameState*)
  {}

  void PDUShowdown::execute(Base::GameState*)
  {}
}