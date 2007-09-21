#ifndef __lounge_tableptr_h__
#define __lounge_tableptr_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class TablePtr
  Created  : 04/16/2000

  OVERVIEW : Semi-smart pointer class. The CPlayer and CTable objects
             are maintained in primary containers. Other places have
             containers of PlayerPtrs and TablePtrs that refer to objects
             in the primary containers. This avoids duplication of CPlayer
             and CTable objects.

 ****************************************************************************/

#include "lounge/table.h"


struct TablePtr
{
    TablePtr()
        : table_(0) {}

    TablePtr(const TablePtr& rhs)
        : table_(rhs.table_) {}

    TablePtr(CTable& table)
        : table_(&table) {}

    TablePtr& operator = (const TablePtr& rhs)
    {
        if (this != &rhs)
        {
            table_ = rhs.table_;
        }
        return *this;
    }

    CTable* operator->() const
    {
        return table_;
    }
    CTable* get() const
    { 
        return table_;
    }

    CTable* table_;

    friend bool operator == (const TablePtr& lhs, const TablePtr& rhs);
};


inline bool operator == (const TablePtr& lhs, const TablePtr& rhs)
{
    return *lhs.table_ == *rhs.table_;
}


struct same_socket
{
    same_socket(SOCKET sd)
        : sd_(sd)
    {}
    bool operator()(const CLoginPlayer& plr) const
    {
        return plr.player_.getSocket() == sd_;
    }
    bool operator()(const CTable& tbl) const
    {
        return tbl.getSocket() == sd_;
    }
    bool operator()(const TablePtr& ptr) const
    {
        return ptr.table_->getSocket() == sd_;
    }

    SOCKET sd_;
};


#endif
