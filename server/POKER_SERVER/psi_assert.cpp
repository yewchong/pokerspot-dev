#include "psi_assert.h"
#include "sys.h"
#include "gamemain.h"
#include "table/table.h"
#include "plm/plm.h"

extern ThreadInfo g_ThreadInfo;
bool g_bShutAlready = false;

void DoShutdown(const char* msg)
{
    Sys_LogError("FATAL: assertion failed\n");
    Sys_LogError(msg);

    g_ThreadInfo.quit_ = TRUE;
    if (CTable::Instance())
        CTable::Instance()->setShutdown(1);

    if (!g_bShutAlready)
    {
        g_bShutAlready = true;
        // This will do a quick exit from where-ever we are
        // so the game logic thread knows to exit
        throw FatalRuntimeError();
    }
}

// Define a function to be called if new fails to allocate memory.
#ifdef _WIN32
int psi_new_handler(size_t size)
#else
void psi_new_handler()
#endif
{
    Sys_LogError("FATAL: Server out of memory\n");

    g_ThreadInfo.quit_ = TRUE;
    CTable::Instance()->setShutdown(1);

    // This will do a quick exit from where-ever we are
    // so the game logic thread knows to exit
    throw FatalRuntimeError();

#ifdef _WIN32
    return 0;
#endif
}
