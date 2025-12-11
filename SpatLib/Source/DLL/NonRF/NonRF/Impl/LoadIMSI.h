#pragma once
#include "ImpBase.h"
//////////////////////////////////////////////////////////////////////////
/* 
AT+CIMI
Description: 
    Execution command causes the TA to return <IMSI>, which is intended to permit the TE to identify the individual SIM card or active application in the UICC (GSM or USIM) which is attached to MT.
    Execution Command:
AT+CIMI
Return:
<imsi>
OK
Test Command:
AT+CIMI
Return:
OK
Parameter:
<IMSI>: International Mobile Subscriber Identity (string without double quotes)
Example:
AT+CIMI
460006963106758
OK
*/
class CLoadIMSI : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CLoadIMSI)
public:
    CLoadIMSI(void);
    virtual ~CLoadIMSI(void);

protected:
    virtual SPRESULT __PollAction(void);
};
