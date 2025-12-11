#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CSwitchOff : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CSwitchOff)
public:
    CSwitchOff(void);
    virtual ~CSwitchOff(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __InitAction(void);
    virtual SPRESULT __PollAction(void);

private:
    uint32 m_u32DelayTime;
    BOOL   m_bAutoMode;
};
