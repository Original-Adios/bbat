#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CLoadMacAddr : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CLoadMacAddr)
public:
    CLoadMacAddr(void);
    virtual ~CLoadMacAddr(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL    LoadXMLConfig(void);
    
private:
    BOOL m_bWIFI;
    BOOL m_bBT;
};
