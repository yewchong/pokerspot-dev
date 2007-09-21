/**********************************************************************

  NAME:		pgl.h
  DATE:		4/8/00
  PURPOSE:	Class Cpgl implements the methods needed to parse and run
			a PGL ("Poker Gaming Languange") script.  Class Cplm calls
			Cpgl's routines.  Cpgl can either derive its script from
			a FILE, or from a character pointer (this allows the
			functionality for us to extend script creation to the
			players, for Dealer's Choice)

**********************************************************************/

#ifndef __pgl_h__
#define __pgl_h__

#include "../includes.h"

#define MAXSCRIPTSIZE		1024
#define MAXCOMMANDSIZE      20
#define	MAXPARAMSIZE        80
#define MAXLINESIZE         MAXCOMMANDSIZE + MAXPARAMSIZE

// Define Tokens
#define ANTE_TOKEN      "ANTE "
    #define ANTE_BLINDS         "BLINDS"
    #define ANTE_LO_LIMIT       "LO_LIMIT"
    #define ANTE_HI_LIMIT       "HI_LIMIT"

#define DEAL_TOKEN              "DEAL "
    #define DEAL_UP             "UP "
    #define DEAL_DOWN           "DOWN "
    #define DEAL_INDIAN         "INDIAN "
    #define DEAL_COMMUNITY      "COMMUNITY "

#define DRAW_TOKEN      "DRAW "

#define BET_TOKEN       "BET "
    #define HI_LIMIT            "LIMIT=HI_LIMIT "
    #define LO_LIMIT            "LIMIT=LO_LIMIT "
    #define POT_LIMIT           "LIMIT=POT "
    #define NO_LIMIT            "LIMIT=NONE "
    #define BET_CAP             "CAP="

#define SHOWDOWN_TOKEN  "SHOWDOWN "
    #define SHOWDOWN_HI         "HI "
    #define SHOWDOWN_LO         "LO "
    #define SHOWDOWN_HILO       "HI/LO "
    #define SHOWDOWN_GUTS       "GUTS=YES "
    #define SHOWDOWN_NOGUTS     "GUTS=NO "      // no guts, no glory


// Function Prototypes
bool reportStrCmp(int);
void unknownCmd(char *, int *, int);
void syntaxErr(char *, int *, int);
void getNextLine(char *, int *);
void getParameter(char *, int *, char *);

// Define Macros
#define MATCH_TOKEN(x)  reportStrCmp(strncmp(script_ + i, x, strlen(x)))

typedef struct pgl_instr pgl_instr_t;


struct pgl_instr
{
	u_int16_t opcode;
	u_int16_t operand;
	pgl_instr *next;
};

// Define Opcodes

enum Instr_Type
{
	Instr_NOP			=	0,
	Instr_Ante			=	1,
	Instr_Deal          =   2,	
	Instr_Draw			=	3,
	Instr_Bet           =   4,
    Instr_Showdown		=	5,
};

// Define Operands
#define iANTE_BLINDS             0x8000      // Do blinds collection rather than ante?
#define iANTE_HI_LIMIT           0x4000      // Collect HI LIMIT for Ante?
#define iANTE_LO_LIMIT           0x2000      // Collect LO LIMIT for Ante?
#define iANTE_MASK               0x1FFF      // How much is the ante?  (Max: 2^13 = 4096)
#define iDEAL_UP                 0x8000      // Dealing HOLE cards face UP?
#define iDEAL_DOWN               0x4000      // Dealing HOLE cards face DOWN?
#define iDEAL_INDIAN             0x2000      // Dealing INDIAN cards?
#define iDEAL_COMMUNITY          0x1000      // Dealing COMMUNITY cards?
#define iDEAL_MASK               0x0FFF      // How Many Cards To Deal?
#define iBET_NO_LIMIT            0x8000      // Enforce NO limit?
#define iBET_POT_LIMIT           0x4000      // Enforce POT limit as maximum bet?
#define iBET_HI_LIMIT            0x2000      // Enforce HI limit as maximum bet?
#define iBET_LO_LIMIT            0x1000      // Enforce LO limit as maximum bet?
#define iBET_CAP                 0x0800      // Is the bet capped?
#define iBET_CAP_MASK            0x07FF      // How many bets can be made?
#define iSHOWDOWN_HI             0x8000      // Is the HI hand awarded? 1 = yes, 0 = no
#define iSHOWDOWN_LO             0x4000      // Is the LO hand awarded? 1 = yes, 0 = no
#define iSHOWDOWN_HILO           (iSHOWDOWN_HI | iSHOWDOWN_LO)
#define iSHOWDOWN_GUTS           0x2000      // Do a GUTS showdown?
#define iSHOWDOWN_ENFORCE        0x1000      // Enforce showdown rules? (see following lines)
#define iSHOWDOWN_INDIAN_MASK    0x0F00      // How many INDIAN cards must be used
#define iSHOWDOWN_COMMUNITY_MASK 0x00F0      // How many COMMUNITY cards must be used
#define iSHOWDOWN_HOLE_MASK      0x000F      // How many HOLE cards must be used


class Cpgl
{
public:
	Cpgl(FILE *);	// In its most basic form, Cpgl will read script from a file
	Cpgl(char *);	// In Dealer's Choice, Cpgl will read script from a char *
    ~Cpgl();
	bool checkScript(void);	// returns false if script is poorly formatted
	void parseScript(void);	// parses script into multiple pgl_instrs
    bool addInstr(enum Instr_Type, u_int16_t);    // add an instruction to our "ring"
    void wrapInstrRing(void);
	pgl_instr_t	*fetchInstr(void);	// fetches the next instruction

private:
	char script_[MAXSCRIPTSIZE];
	pgl_instr_t	*instrPtr_, *programCounter_;
};

#endif // #ifndef __pgl_h__