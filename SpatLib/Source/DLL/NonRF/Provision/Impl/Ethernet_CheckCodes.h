#pragma once
#include "CheckCodes.h"

class CEthernet_CheckCodes : public CCheckCodes
{
    DECLARE_RUNTIME_CLASS(CEthernet_CheckCodes)
public:
    CEthernet_CheckCodes(void);
    virtual ~CEthernet_CheckCodes(void);

protected:
    virtual BOOL LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);
};
