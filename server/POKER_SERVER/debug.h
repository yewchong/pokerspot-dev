#ifndef __debug_h__
#define __debug_h__

// Debug bits are : 0x0PPM (where P = PLM, M = Main)
#define DEBUG_MAIN_ALL      0x000F
#define	DEBUG_PLM_ALL       0x0FF0

// Main Debugs
#define DEBUG_MAIN          0x0001
#define DEBUG_POLLER        0x0002
#define	DEBUG_TABLE         0x0004
#define	DEBUG_DATABASE      0x0008

// PLM Debugs
#define DEBUG_PLM_ANTE      0x0010
#define DEBUG_PLM_DEAL      0x0020
#define DEBUG_PLM_DRAW      0x0040
#define DEBUG_PLM_BET       0x0080
#define DEBUG_PLM_SHOWDOWN  0x0100
#define DEBUG_PLM           0x0FF0

// PGL Debugs
#define DEBUG_PGL           0x1000

// Other Debugs


#define DEBUG_ASSERT        0x8000
#define DEBUG_ALL           0xFFFF

//#define DEBUG               (DEBUG_PLM | DEBUG_DATABASE)
#define DEBUG               (DEBUG_PLM)
//#define DEBUG               (0)

#endif
