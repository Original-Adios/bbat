#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CCheckDiagConnect : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCheckDiagConnect)
public:
    CCheckDiagConnect(void);
    virtual ~CCheckDiagConnect(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

    enum
    {
        LoadSwVer = 0,
        AT,
        MAX_CMD
    } m_eCmd;
    DWORD m_dwTimeOut;
};
