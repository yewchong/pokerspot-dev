
#include "pgl/pgl.h"
#include "sys.h"


// Non-Class Functions
bool reportStrCmp(int retval)
{
    if (retval == 0)
        return true;
    else
        return false;
};

void unknownCmd(char *script, int *offset, int lineNum)
{
    char command[MAXCOMMANDSIZE] = { '\0' };
    int tmp;

    tmp = *offset;

    while ((script[tmp] != 0xa) &&
           (script[tmp] != 0x0) &&
           (script[tmp] != ' ')) { tmp++; };

    memcpy(command, script + *offset, tmp - *offset);
    printf("Unknown Command '%s': line %d\n", command, lineNum);

    // remove excess (paramaters and whitespace)
    while ((script[tmp] != 0xa) && (script[tmp] != 0x0))
    { tmp++; };

    *offset = tmp;
};

void syntaxErr(char *script, int *offset, int lineNum)
{

    char command[MAXCOMMANDSIZE] = { '\0' };
    int tmp;

    tmp = *offset;

    while ((script[tmp] != 0xa) &&
           (script[tmp] != 0x0) &&
           (script[tmp] != ' ')) { tmp++; };

    memcpy(command, script + *offset, tmp - *offset);
    printf("Syntax Error in Command '%s': line %d\n", command, lineNum);

    // remove excess (paramaters and whitespace)
    while ((script[tmp] != 0xa) && (script[tmp] != 0x0))
    { tmp++; };

    *offset = tmp;
};

void getNextLine(char *script, int *i)
{
    int j = *i;

    while ((script[j] != '\0') && (script[j] != 0xa)) { j++; };

    *i = j;
};

void getParameter(char *script, int *i, char *params)
{
    int j = *i;
    
    while ((script[j] != 0xa) && (script[j] != ' ')) { j++; };

	memcpy(params, script + *i, j - *i);
    params[(j - *i) + 1] = '\0';

    *i = j;

};

// Class Functions
Cpgl::Cpgl(char *script)
{
	strncpy(script_, script, MAXSCRIPTSIZE);
	instrPtr_ = (pgl_instr_t *)malloc(sizeof(pgl_instr_t));
    instrPtr_->opcode = Instr_NOP;
    instrPtr_->operand = 0x0;
    instrPtr_->next = NULL;
    programCounter_ = instrPtr_;
};

Cpgl::Cpgl(FILE *file)
{
	char *offset = script_;
	memset(script_, 0x0, MAXSCRIPTSIZE);
	instrPtr_ = (pgl_instr_t *)malloc(sizeof(pgl_instr_t));
	instrPtr_->opcode = Instr_NOP;
    instrPtr_->operand = 0x0;
    instrPtr_->next = NULL;
    programCounter_ = instrPtr_;

    ASSERT(file != NULL);

	// Open file, and read until EOF or MAXSCRIPTSIZE into script_
	while (!feof(file))
	{
		fread(offset, 1, 1, file);
		offset++;
	}

	parseScript();
}

Cpgl::~Cpgl()
{
    free(instrPtr_);
    instrPtr_ = NULL;
}

bool Cpgl::checkScript(void)
{
    // Use set of rules to check validity of format of script (currently not implemented)

    /* Mainly this will be used for Dealer's Choice, to make sure that the script is
       good before we try to run it..  We'll return an error to the dealer if the
       script he requests is faulty.  The client shouldn't allow for faulty scripts
       to be created in the first place, but in the off chance that a hacker tries to
       do this, this should stop him dead in his tracks. */

	return true;
};

bool Cpgl::addInstr(enum Instr_Type type, u_int16_t operand)
{
    u_int16_t opcode = (u_int16_t)type;
    pgl_instr_t *iPtr;

    if (operand == 0x0)     // no need to add NOPs
        return false;

    iPtr = programCounter_;

    iPtr->next = (pgl_instr_t *)malloc(sizeof(pgl_instr_t));

    if (iPtr == NULL)
    {
        printf("Couldn't allocate memory for new operand!\n");
        return false;
    }
    
    iPtr = iPtr->next;
    iPtr->opcode = opcode;
    iPtr->operand = operand;
    iPtr->next = NULL;

    programCounter_ = iPtr;

    return true;
};

pgl_instr_t *Cpgl::fetchInstr(void)
{
    // Since the first node of the instruction ring is set to "NOP", this will always work
    programCounter_ = programCounter_->next;
	return programCounter_;
};

void Cpgl::wrapInstrRing(void)
{
    pgl_instr_t *iPtr;

    iPtr = instrPtr_;

    while (iPtr->next != NULL)
    {
        iPtr = iPtr->next;
    }

    iPtr->next = instrPtr_;
    programCounter_ = instrPtr_;
};

void Cpgl::parseScript(void)
{
	// Parse script_ value into multiple pgl_instrs
	// set the last pgl_instr->next to the first (so fetchInstr continues to work)

	int i = 0, j = 0, lineNum = 1;
	char ch = script_[i];
    char param[MAXPARAMSIZE] = { '\0' };
	u_int16_t operand = 0x0;

	for (; ((ch != '\0') && (i < MAXSCRIPTSIZE)); ch = script_[i], 
                                                  memset(param, 0x0, MAXPARAMSIZE),
                                                  operand = 0x0)
	{
		switch (ch)
		{
		case (0xa):	while (script_[i] == 0xa) { i++; lineNum++;}; break;
		case ('#'): getNextLine(script_, &i); break;
		case (' '):	getNextLine(script_, &i); break;
        case ('A'): if (MATCH_TOKEN(ANTE_TOKEN)) {
                        i += strlen(ANTE_TOKEN);

                        if (MATCH_TOKEN(ANTE_BLINDS))
                        {
                            i += strlen(ANTE_BLINDS);
                            // Just collect the blinds
                            operand |= iANTE_BLINDS;
                        }
                        else if (MATCH_TOKEN(ANTE_LO_LIMIT))
                        {
                            i += strlen(ANTE_LO_LIMIT);
                            operand |= iANTE_LO_LIMIT;
                        }
                        else if (MATCH_TOKEN(ANTE_HI_LIMIT))
                        {
                            i += strlen(ANTE_HI_LIMIT);
                            operand |= iANTE_HI_LIMIT;
                        }
                        else
                        {
                            // ANTE Amount is specified as parameter
                            getParameter(script_, &i, param);
                            operand = atoi(param);                            
                        }

                        addInstr(Instr_Ante, operand);
                    }
                    else {
                        unknownCmd(script_, &i, lineNum);
                    } break;
        case ('B'): if (MATCH_TOKEN(BET_TOKEN)) {
                        i += strlen(BET_TOKEN);

                        if (MATCH_TOKEN(LO_LIMIT))
                        {
                            i += strlen(LO_LIMIT);
                            operand |= iBET_LO_LIMIT;
                        }
                        else if (MATCH_TOKEN(HI_LIMIT))
                        {
                            i += strlen(HI_LIMIT);
                            operand |= iBET_HI_LIMIT;
                        }
                        else if (MATCH_TOKEN(POT_LIMIT))
                        {
                            i += strlen(POT_LIMIT);
                            operand |= iBET_POT_LIMIT;
                        }
                        else if (MATCH_TOKEN(NO_LIMIT))
                        {
                            i += strlen(NO_LIMIT);
                            operand |= iBET_NO_LIMIT;
                        }
                        else {
                            i -= strlen(BET_TOKEN);
                            syntaxErr(script_, &i, lineNum);
                        }

                        if (MATCH_TOKEN(BET_CAP))
                        {
                            // CAP value is specified as parameter

                            i += strlen(BET_CAP);
                            operand |= iBET_CAP;
                            getParameter(script_, &i, param);
                            operand |= atoi(param);
                        }
                        else
                        {
                            i -= strlen(BET_TOKEN);
                            syntaxErr(script_, &i, lineNum);
                            operand = 0x0;
                        }

                        addInstr(Instr_Bet, operand);
                    }
                    else {
                        unknownCmd(script_, &i, lineNum);
                    } break;
		case ('D'): if (MATCH_TOKEN(DEAL_TOKEN)) {
						i += strlen(DEAL_TOKEN);

						if (MATCH_TOKEN(DEAL_UP))
						{
						// DEAL UP <#>
                            i += strlen(DEAL_UP);

                            // Fetch Parameters / Insert Into Operand
                            getParameter(script_, &i, param);
                            operand = atoi(param) | iDEAL_UP;
						}

						else if (MATCH_TOKEN(DEAL_DOWN))
						{
						// DEAL DOWN <#>
                            i += strlen(DEAL_DOWN);

                            // Fetch Parameters / Insert Into Operand
                            getParameter(script_, &i, param);
                            operand = atoi(param) | iDEAL_DOWN;
						}

						else if (MATCH_TOKEN(DEAL_INDIAN))
						{
                        // DEAL INDIAN <#>
                            i += strlen(DEAL_INDIAN);

                            // Fetch Parameters / Insert Into Operand
                            getParameter(script_, &i, param);
                            operand = atoi(param) | iDEAL_INDIAN;
                        } 
                        else if (MATCH_TOKEN(DEAL_COMMUNITY))
                        {
                        // DEAL COMMUNITY <#>
                            i += strlen(DEAL_COMMUNITY);

                            // Fetch Parameters / Insert Into Operand
                            getParameter(script_, &i, param);
                            operand = atoi(param) | iDEAL_COMMUNITY;

                        } else {
							printf("Error in DEAL statement: line %d!\t", lineNum);
							printf("Statement IGNORED!\n");
                            getNextLine(script_, &i);
                            break;
						};

                        addInstr(Instr_Deal, operand);

                    } else if (MATCH_TOKEN(DRAW_TOKEN)) {
					// DRAW <#>
					} else {
                        unknownCmd(script_, &i, lineNum);
                    }; break;
        case ('S'): if (MATCH_TOKEN(SHOWDOWN_TOKEN))
                    {
                        i += strlen(SHOWDOWN_TOKEN);

                        if (MATCH_TOKEN(SHOWDOWN_HI))
                        {
                            i += strlen(SHOWDOWN_HI);
                            operand |= iSHOWDOWN_HI;

                        } else if (MATCH_TOKEN(SHOWDOWN_LO))
                        {
                            i += strlen(SHOWDOWN_LO);
                            operand |= iSHOWDOWN_LO;

                        } else if (MATCH_TOKEN(SHOWDOWN_HILO))
                        {
                            i += strlen(SHOWDOWN_HILO);
                            operand |= iSHOWDOWN_HILO;
                        } else {
                            i -= strlen(SHOWDOWN_TOKEN);
                            syntaxErr(script_, &i, lineNum);
                            operand = 0x0;
                        }

                        if (MATCH_TOKEN(SHOWDOWN_GUTS))
                        {
                            i += strlen(SHOWDOWN_GUTS);
                            operand |= iSHOWDOWN_GUTS;
                        } else if (MATCH_TOKEN(SHOWDOWN_NOGUTS))
                        {
                            i += strlen(SHOWDOWN_NOGUTS);
                            // no operand modification needed
                        } else {
                            i -= strlen(SHOWDOWN_TOKEN);
                            syntaxErr(script_, &i, lineNum);
                            operand = 0x0;
                        }

                        addInstr(Instr_Showdown, operand);

                    } else {
                        unknownCmd(script_, &i, lineNum);
                    }; break;
        default:	unknownCmd(script_, &i, lineNum); break;
		}
	}

    // The following commands will make a "ring" out of the instructions, so that calls
    // to fetchInstr() will never fail!
    wrapInstrRing();

    if (DEBUG & DEBUG_PGL)
		printf("Finished parsing script.  %d lines read.\n", lineNum);

};

