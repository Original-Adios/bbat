#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CSaveToFlash : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CSaveToFlash)
public:
    CSaveToFlash(void);
    virtual ~CSaveToFlash(void);

protected:
    virtual BOOL LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    SP_MODE_INFO  m_eModule;
    uint32     m_u32TimeOut;
};
