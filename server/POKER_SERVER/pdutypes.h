#ifndef __psi_pdutypes_h__
#define __psi_pdutypes_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : pdutypes.h
  Created  : 02/11/2000

  OVERVIEW : PDU Type definitions.

 ****************************************************************************/

/*
 * PDU type identifiers
 */

enum PDU_Type
{
  PDU_Unknown                = 0,

  /* Lounge messages */
  PDU_Lounge_Login           = 1,
  PDU_Lounge_Accept          = 2,
  PDU_Lounge_Reject          = 3,
  PDU_Lounge_Survey          = 4,
  PDU_Lounge_Status          = 5,
  PDU_Lounge_SitDown         = 6,
  PDU_Lounge_TableFull       = 7,
  PDU_Lounge_Sitting         = 8,
  PDU_Lounge_Logout          = 9,
  PDU_Lounge_LogoutWarning   = 10,
  PDU_Lounge_LogoutCertain   = 11,
  PDU_Lounge_LogoutConfirmed = 12,
  PDU_Lounge_TableLogout     = 13,
  PDU_Lounge_LoginStats      = 14,
  PDU_Lounge_BroadTableQuery = 15,
  PDU_Lounge_BroadTableReply = 16,
  PDU_Lounge_CancelSitDown   = 19,

  // Credit card, Buy-in & Cash-out pdus
  PDU_Lounge_CreditCardQuery    = 20,
  PDU_Lounge_CreditCardReply    = 21,
  PDU_Lounge_BuyinRequest       = 22,
  PDU_Lounge_CashoutRequest     = 23,
  PDU_Lounge_QueueUpdate        = 24,
  PDU_Lounge_BuyinReply         = 25,
  PDU_Lounge_CashoutReply       = 26,
  PDU_Lounge_BalanceQuery       = 27,
  PDU_Lounge_AccountBalance     = 28,
  PDU_Lounge_TransactionQuery   = 29,
  PDU_Lounge_TransactionReply   = 30,
  PDU_Lounge_Observe            = 31,
  PDU_Lounge_Observing          = 32,

  /* Lounge server - table server pdus */
  PDU_Server_SpawnTable         = 33,
  PDU_Server_TableLogin         = 34,
  PDU_Server_TableLogout        = 35,
  PDU_Server_PlayerLoginQuery   = 36,
  PDU_Server_PlayerLoginReply   = 37,
  PDU_Server_PlayerObserveQuery = 38,
  PDU_Server_PlayerObserveReply = 39,
  PDU_Server_TableUpdate        = 40,

  /* Lounge server sends game names to client */
  PDU_Lounge_GameNames          = 41,
  PDU_Lounge_QueueFull          = 42,

  /* Tournament PDU types - lounge server<->table server */
  PDU_Tournament_SeatPlayer     = 60,
  PDU_Tournament_ReseatPlayer   = 61,
  PDU_Tournament_PlayerUnseated = 62,
  PDU_Tournament_Start          = 63,
  PDU_Tournament_Freeze         = 64,
  PDU_Tournament_Params         = 65,
  PDU_Tournament_DissolveTable  = 66,
  PDU_Tournament_DissolveInfo   = 67,
  PDU_Tournament_PlayerSeated   = 68,
  PDU_Tournament_Synchronize    = 69,
  PDU_Tournament_ReseatAck      = 70,
  PDU_Tournament_ReseatNack     = 71,

  /* Tournament PDU types - lounge client<->lounge server */
  PDU_Tournament_DetailsQuery   = 80,
  PDU_Tournament_DetailsReply   = 81,
  PDU_Tournament_EditRules      = 82,
  PDU_Tournament_SaveRules      = 83,
  PDU_Tournament_EditFormat     = 84,
  PDU_Tournament_SaveFormat     = 85,

  PDU_Tournament_AnteAllIn      = 90,
  PDU_Tournament_PlayerFinished = 91,

  /* Table setup messages */
  PDU_Setup_TableLogin       = 113,
  PDU_Player                 = 114,
  PDU_Setup_TableReject      = 115,
  PDU_Setup_BuyInQuery       = 116,
  PDU_Setup_BuyInRequest     = 117,
  PDU_Setup_TableLogout      = 118,
  PDU_Setup_TableLogoutConfirm = 119,

  /* Play messages */
  PDU_Play_Button            = 120, 
  PDU_Play_BlindRequest      = 220,
  PDU_Play_PostBlind         = 221,
  PDU_Play_SitInQuery        = 222,
  PDU_Play_SitIn             = 223,
  PDU_Play_SitOut            = 224,
  PDU_Play_CommunityCard     = 225,
  PDU_Play_PocketCard        = 226,
  PDU_Play_BlindCard         = 227,
  PDU_Play_ActionRequest     = 228,
  PDU_Play_Action            = 229,
  PDU_Play_Announce          = 230,

  PDU_Play_DealCards         = 231, 
  PDU_Play_Showdown          = 232,
  PDU_Play_ActionEcho        = 233,
  PDU_Setup_TableAccept      = 234, 
  PDU_Play_DealCardsFlex     = 235, 
  PDU_Play_RefuseBlind       = 236,

  PDU_Play_ActionRequestSpreadLimit = 237, // NEW
  PDU_Play_ActionSpreadLimit = 238, // NEW

  PDU_Play_ShowCardsQuery    = 240, 
  PDU_Play_ShowCards         = 241, 
  PDU_Play_AllIn             = 242, 
  PDU_Setup_PlayerInfo       = 243, 
  PDU_Setup_HandLogRequest   = 244, 
  PDU_Setup_BuyinMore        = 245, 

  PDU_Setup_ObserverLogin    = 246,
  PDU_Setup_ObserverLogout   = 247,

  /* General PDU's */
  PDU_Chat                   = 502,
  PDU_Ping                   = 503,
  PDU_Pong                   = 504,
  PDU_GameNumber             = 505,
  PDU_UpgradeRequest         = 506,
  PDU_HeartBeat              = 507,

  /* Error & Notify PDU's */
  PDU_Error_Client           = 65000,
  PDU_Error_Server           = 65001,
  PDU_Server_Notify          = 65002,
  PDU_Error_Generic          = 65003
};

//END_NAMESPACE(Network)


#endif