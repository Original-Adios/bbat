#pragma once
#include "ImpBase.h"
//////////////////////////////////////////////////////////////////////////
class CCheckCrystal32K : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCheckCrystal32K)
public:
    CCheckCrystal32K(void);
    virtual ~CCheckCrystal32K(void);

private:
    virtual BOOL    LoadXMLConfig(void);
	virtual SPRESULT __PollAction(void);

private:
	int m_nLowLimit;
	int m_nHighLimit;
};
