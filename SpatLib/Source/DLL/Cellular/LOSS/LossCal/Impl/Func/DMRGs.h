#pragma once
#include "dmrlossbase.h"
class CDMRGs :
	public CDMRLossBase
{
public:
	CDMRGs(CImpBase *pSpat);
	virtual ~CDMRGs(void);

public:
	virtual SPRESULT Run();
	virtual void ConfigFreq();
};

