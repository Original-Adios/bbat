#pragma once
#include "impbase.h"
#include "IWlanApi.h"
#include "IBTApi.h"

class CWriteEfuse :
	public CImpBase
{
	DECLARE_RUNTIME_CLASS(CWriteEfuse)
public:
	CWriteEfuse(void);
	~CWriteEfuse(void);
protected:
	virtual SPRESULT __InitAction (void);
	virtual SPRESULT __PollAction (void);
	virtual SPRESULT __FinalAction(void);
	virtual BOOL       LoadXMLConfig(void);

	ICWlanApi* m_pWlanApi;
	int m_nEfuseLimit;

	BOOL m_bWriteMac;
	BOOL m_bWriteAfc;
	BOOL m_bWriteApc;
	BOOL m_bWriteOTTSN;
};

