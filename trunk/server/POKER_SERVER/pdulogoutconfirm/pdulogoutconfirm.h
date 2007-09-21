#ifndef __table_pdulogoutconfirm_h__
#define __table_pdulogoutconfirm_h__

#include "../pduhandler/pduhandler.h"

class CTable;
class CPlayer;

class CpduLogoutConfirm : public Cpdu
{
public:
    CpduLogoutConfirm();
    static int SendLogoutConfirmPdu(CPlayer*);
};

#endif