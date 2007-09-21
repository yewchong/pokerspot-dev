/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUReseat
  Created  : 07/26/2000

  OVERVIEW : Reseat PDU. Sent by the table server when
             the table is dissolved and the player is reseated
             at another table.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pdureseat.h"
#include "network/server.h"
#include "ui/global.h"
#include "ui/tableview.h"

BEGIN_NAMESPACE(Network)

void PDUReseat::execute(GameState* pS) 
{
#ifdef PSPOT_TABLE_MODULE_
    CTableView* table = CTableView::Instance();
    if (table)
    {
        table->resetCards();
        table->announce("");
        table->setDealer(-1, false);

        // fgure out new table number from
        // port number
        table->setTableNumber(port_ - 8501);
        table->setCaption();

        for (int i = 0; i < 10; i++)
            table->removePlayer(i); 
    }

    // Close old connection, connect to new address
    Global::Instance().closeConnections();
    Global::Instance().loginToTableServer(host_, port_,
                                          Server::Username(),
                                          Server::Password());
#endif
}

END_NAMESPACE(Network)


