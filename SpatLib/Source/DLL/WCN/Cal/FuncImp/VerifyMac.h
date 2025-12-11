#pragma once
#include "impbase.h"
#include "IWlanApi.h"


class CVerifyMac :
	public CImpBase
{
	DECLARE_RUNTIME_CLASS(CVerifyMac)
public:
	CVerifyMac(void);
	virtual ~CVerifyMac(void);
protected:
	virtual SPRESULT __InitAction (void);
	virtual SPRESULT __PollAction (void);
	virtual SPRESULT __FinalAction(void);
	
	ICWlanApi* m_pWlanApi;
};

