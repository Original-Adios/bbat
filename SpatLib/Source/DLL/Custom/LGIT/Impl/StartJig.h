#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CStartJig : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CStartJig)
public:
    CStartJig(void);
    virtual ~CStartJig(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    DWORD m_dwPort;
    DWORD m_dwBaud;
};
