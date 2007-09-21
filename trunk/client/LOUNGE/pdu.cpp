#include "stdafx.h"
#include "network/pdus.h"

//
// Dummy versions of the PDU execute functions
// for those PDUs that the Lounge client does
// not see.
//

namespace Network
{
  void PDUGameNumber::execute(Base::GameState*)
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

  void PDUPlayer::execute(Base::GameState*)
  {}

  void PDUAnnounce::execute(Base::GameState*)
  {}

  void PDUBlindRequest::execute(Base::GameState*)
  {}

  void PDUShowdown::execute(Base::GameState*)
  {}
}