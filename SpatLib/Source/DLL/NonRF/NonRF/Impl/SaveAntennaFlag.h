#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CSaveAntennaFlag : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CSaveAntennaFlag)
public:
    CSaveAntennaFlag(void);
    virtual ~CSaveAntennaFlag(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);
};
