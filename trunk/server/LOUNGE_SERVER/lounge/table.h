#ifndef __lounge_table_h__
#define __lounge_table_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CTable
  Created  : 04/16/2000

  OVERVIEW : This struct maintains information about a
             table server.

 ****************************************************************************/

#include <list>
#include <string>
#include "compat.h"
#include "psi_win32.h"
#include "lounge/playerptr.h"
USING_NS_T(std, string);
USING_NS_T(std, list);
#include "common/chips.h"


class CTable
{
public:
    CTable(SOCKET sd, u_long addr);
    CTable(const CTable& rhs);
    CTable& operator = (const CTable& rhs);
    virtual ~CTable() {}

    SOCKET getSocket() const;
    u_int16_t getGameType() const;
    u_int16_t getNumber() const;
    u_int16_t getNumPlayers() const;
    CChips getLo() const;
    CChips getHi() const;
    u_long getAddr() const;
    u_short getPort() const;
    string getTitle() const;

    void setNumPlayers(u_int16_t);

    void setData(u_int16_t gametype_,
                 u_int16_t number,
                 const CChips& lo,
                 const CChips& hi,
                 u_short   port);

    void playerSeated(const char* username);
    void playerUnseated(const char* username);
    bool isPlayerSeated(const char* username);
    int  getNumPlayersSeated() const;

    enum TournamentState
    {
        TS_Idle      = 0,
        TS_Running   = 1,
        TS_Paused    = 2,
        TS_Dissolved = 3
    };
    TournamentState getTournamentState() const;
    void setTournamentState(TournamentState ts);
    bool isPaused() const { return state_ == TS_Paused; }
    bool isDissolved() const { return state_ == TS_Dissolved; }
    list<string>& getPlayers() { return players_; }

    virtual int send(Cpdu&);

private:
    SOCKET sd_;
    u_int16_t gametype_;
    u_int16_t number_;
    CChips lo_, hi_;
    u_int16_t numPlayers_;

    u_long addr_;
    u_short port_;

    TournamentState state_;

    // Players seated at this table:
    list<string> players_;

    friend bool operator == (const CTable& lhs, const CTable& rhs);
};

inline SOCKET CTable::getSocket() const
{
    return sd_;
}

inline u_int16_t CTable::getGameType() const
{
    return gametype_;
}

inline u_int16_t CTable::getNumber() const
{
    return number_;
}

inline u_int16_t CTable::getNumPlayers() const
{
    return numPlayers_;
}

inline CChips CTable::getLo() const
{
    return lo_;
}

inline CChips CTable::getHi() const
{
    return hi_;
}

inline void CTable::setData(u_int16_t gametype,
                            u_int16_t number,
                            const CChips& lo,
                            const CChips& hi,
                            u_short   port)
{
    gametype_ = gametype;
    number_ = number;
    lo_ = lo;
    hi_ = hi;
    port_ = port;
}

inline u_long CTable::getAddr() const
{
    return addr_;
}

inline u_short CTable::getPort() const
{
    return port_;
}

inline void CTable::setNumPlayers(u_int16_t numPlayers)
{
    numPlayers_ = numPlayers;
}

inline bool operator == (const CTable& lhs, const CTable& rhs)
{
    return lhs.number_ == rhs.number_;
}

inline int CTable::getNumPlayersSeated() const
{
    return players_.size();
}

inline CTable::TournamentState CTable::getTournamentState() const
{
    return state_;
}

inline void CTable::setTournamentState(CTable::TournamentState ts)
{
    state_ = ts;
}


#endif
