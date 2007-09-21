#include "gamemain.h"
#include "plm/plm.h"
#include "table/table.h"
#include "sys.h"



/*
 * GameMain
 *
 * This is the game logic function that
 * runs in its own thread.
 */
THREADPROC_RETURN CDECL GameMain(VOID* pV)
{
    ThreadInfo* threadinfo = reinterpret_cast<ThreadInfo*>(pV);

    try
    {
        while (!threadinfo->quit_)
        {
            threadinfo->plm_->mainLoop();
        }
    }
    catch(const FatalRuntimeError&)
    {
        Sys_LogError("Runtime exception in logic thread!\n");
        if (CTable::Instance())
            CTable::Instance()->setShutdown(1);
    }
    catch(...)
    {
        Sys_LogError("Unknown exception in logic thread!\n");
        if (CTable::Instance())
            CTable::Instance()->setShutdown(1);
    }

#ifdef _POSIX_THREADS
    return (THREADPROC_RETURN)0;
#endif
}

