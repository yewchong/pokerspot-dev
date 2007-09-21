#include "pdusitinquery.h"
#include "../table/table.h"
#include "../player/player.h"


CpduSitInQuery::CpduSitInQuery(CTable* table)
    :
    table_(table)
{
    type_ = PDU_Play_SitInQuery;
    length_ = PDU_HEADER_SIZE;
    stuffHeader();
}

int CpduSitInQuery::sendSitInQuery(CPlayer* player)
{
    // Sit-In queries are never broadcasted 
    // but sent to individual players
    return sendTo(player->getConnfd());
}
