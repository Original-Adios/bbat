#pragma once
#include "dmrlossbase.h"
class CDMRClc :
	public CDMRLossBase
{
public:
	CDMRClc(CImpBase *pSpat);
	virtual ~CDMRClc(void);
	SPRESULT Run();



};

