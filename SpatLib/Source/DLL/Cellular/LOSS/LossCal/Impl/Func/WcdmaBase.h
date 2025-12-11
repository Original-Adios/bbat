#pragma once

#include "ILossFunc.h"

class CWcdmaBase : public ILossFunc
{
public:
	CWcdmaBase(CImpBase *pSpat);
	virtual ~CWcdmaBase(void);

	SPRESULT LoadAndCheckFeatureSupport();
	SPRESULT LoadBandAntInfo(int nBand, RF_ANT_E &eAnt);
	SPRESULT AntSwitch(RF_ANT_E eAnt);
};