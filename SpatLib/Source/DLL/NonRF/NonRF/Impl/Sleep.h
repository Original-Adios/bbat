#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CSleep : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CSleep)
public:
    CSleep(void);
    virtual ~CSleep(void);

protected:
    virtual BOOL LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    uint32 m_u32SleepTime;
};
