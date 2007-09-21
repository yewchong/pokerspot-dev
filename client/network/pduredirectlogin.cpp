/****************************************************************************

  Copyright (c) PSI Corp, 2001

  Title    : class CPDURedirectLogin
  Created  : 09/29/2001

  OVERVIEW : Load Balancer sends this PDU to the Lounge Client
             to redirect him to the appropriate Lounge Server.

 ****************************************************************************/


#include "stdafx.h"
#include "network/pduredirectlogin.h"
#include "network/server.h"
#include "ui/global.h"


BEGIN_NAMESPACE(Network)


CPDURedirectLogin::CPDURedirectLogin(const char* buf)
  :
  PDUHeader(buf),
  host_    (0),
  port_    (0)
{
  u_int32_t* plong = (u_int32_t*)&buf[PDU_HEADER_SIZE];
  host_ = ntohl(*plong++);
  u_int16_t* pint = (u_int16_t*)plong;
  port_ = ntohs(*pint++);
}


void CPDURedirectLogin::execute(GameState* pS) 
{
#ifndef PSPOT_TABLE_MODULE_

  HWND hWnd = NULL;
  CWnd* wnd = AfxGetMainWnd();
  if (wnd)
    hWnd = wnd->GetSafeHwnd();

  // Close old connection, connect to new address
  Global::Instance().closeConnections();
  Global::Instance().loginToLoungeServer(hWnd,
                                         host_, port_,
                                         Server::Username(),
                                         Server::Password());

#endif
}

END_NAMESPACE(Network)


