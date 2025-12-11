#pragma once
#include "ImpBase.h"
//////////////////////////////////////////////////////////////////////////
/* 
T+CEID
Description: 
Execution command causes the TA to return <EID>, which is the ID of CMCC eUICC
Execution Command:
AT+CEID
Return:
+CEID: <EID>
OK
Test Command:
AT+CEID=?
Return:
OK
Parameter:
<EID>: eUICC Identifier (string without double quotes)
Example:
AT+CEID
+CEID: 908106031C0102345678
OK
*/
class CLoadEID : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CLoadEID)
public:
    CLoadEID(void);
    virtual ~CLoadEID(void);

protected:
    virtual SPRESULT __PollAction(void);
};
