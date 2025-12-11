#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CCheckOTP : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCheckOTP)
public:
    CCheckOTP(void);
    virtual ~CCheckOTP(void);

protected:
    virtual SPRESULT __PollAction(void);
	virtual BOOL    LoadXMLConfig(void);

private:
	BOOL m_bTriad;

};
