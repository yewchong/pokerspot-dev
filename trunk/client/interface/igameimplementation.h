#ifndef __psi_igameimpl_h__
#define __psi_igameimpl_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : interface IGameImplementation
  Created  : 01/19/2000

  OVERVIEW : Declare interface IGameImplementation.
            
             NOTE: this is not a true COM interface but a
             simplified interface that allows the game shell
             to call into the game implementation DLL.

 ****************************************************************************/

#include <basetyps.h>

interface IGameDispatch;

interface IGameImplementation
{
  STDMETHOD(CreateMainWindow)(CWnd** ppWnd) PURE;
  STDMETHOD(GetGameDispatch) (IGameDispatch** ppGD) PURE;
  STDMETHOD(GetGameVersion)  (DWORD** ppdwVers) PURE;
  STDMETHOD(GetGameTitle)    (BSTR* pBSTR) PURE;
};

#endif
