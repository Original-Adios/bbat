#pragma once
#include "ImpBase.h"
//////////////////////////////////////////////////////////////////////////
class CFinalize : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CFinalize)
public:
    CFinalize(void);
    virtual ~CFinalize(void);

private:
     virtual BOOL   LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    uint32 m_u32PowerOffDelayTime;
    BOOL   m_bAutoPowerOff;
};
