#ifndef __lounge_tournamentevent_h__
#define __lounge_tournamentevent_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : enum TournamentEvent
  Created  : 07/25/2000

  OVERVIEW : Tournament event types

 ****************************************************************************/


enum TournamentEvent
{
    TE_Tournament  = 1,
    TE_Table       = 2,
    TE_PlayerJoin  = 3,
    TE_PlayerLeave = 4,
    TE_Winner      = 5
};

#endif