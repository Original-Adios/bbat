#pragma once

#include "ILossFunc.h"

class CGsmBase : public ILossFunc
{
public:
	CGsmBase(CImpBase *pSpat);
	virtual ~CGsmBase(void);

	SPRESULT LoadAndCheckFeatureSupport();
	SPRESULT LoadBandAntInfo(int nBand, RF_ANT_E &eAnt);
	SPRESULT AntSwitch(RF_ANT_E eAnt);
};