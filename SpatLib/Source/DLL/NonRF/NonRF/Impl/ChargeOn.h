#pragma once
#include "ImpBase.h"
//////////////////////////////////////////////////////////////////////////
class CChargeOn : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CChargeOn)
public:
    CChargeOn(void);
    virtual ~CChargeOn(void);

private:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);
    
    long m_lMaxChargeCurrent;
};
