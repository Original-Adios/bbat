#pragma once
#include "ilossfunc.h"

class CDMRLossBase :
	public ILossFunc
{
public:
	CDMRLossBase(CImpBase *pSpat);
	virtual ~CDMRLossBase(void);
	virtual SPRESULT Init();
	virtual SPRESULT Release();

	SPRESULT InitCal();

protected:
	SPRESULT MeaPower( double dFreq, int nPcl ,double dTargetPwr, double &dPwr);
	SPRESULT MeaPowerByCal( double dFreq, int nPcl ,double dTargetPwr, double &dPwr);
	SPRESULT MeaPowerByFt( double dFreq, int nPcl ,double dTargetPwr, double &dPwr);
	SPRESULT InitCalByCal();
	SPRESULT InitCalByFt();

};

