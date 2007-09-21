#ifndef __psi_cardsdef_h__
#define __psi_cardsdef_h__


#ifdef EXP_CARDS_DLL_
#define CARDS_DLL_DECL __declspec(dllexport)
#else
#define CARDS_DLL_DECL __declspec(dllimport)
#endif


#endif