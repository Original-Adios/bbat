#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CReadSN : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CReadSN)
public:
    CReadSN(void);
    virtual ~CReadSN(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL    LoadXMLConfig(void);

private:
    BOOL    m_bSN1;
    BOOL    m_bSN2;
    CHAR    m_szSN1[64];
    CHAR    m_szSN2[64];
};
