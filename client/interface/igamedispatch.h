#ifndef __psi_igamedispatch_h__
#define __psi_igamedispatch_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : interface IGameDispatch
  Created  : 01/19/2000

  OVERVIEW : Declare interface IGameDispatch.

             This interface allows the game shell to call
             into the implementation's GameLogic system.
            
             NOTE: this is not a true COM interface but a
             simplified interface that allows the game shell
             to call into the game implementation DLL.
             For instance, there is no reference counting since
             there is single instance alive throughout the
             application's life span.


 ****************************************************************************/

#include <basetyps.h> 

interface IGameDispatch
{
  STDMETHOD(Tick)(DWORD dt, BOOL* pRC) PURE;
};

#endif
