#pragma once

#include "GsmBase.h"

class CGsmClc : public CGsmBase
{
public:
    CGsmClc(CImpBase *pSpat);
    virtual ~CGsmClc(void);
   
	virtual SPRESULT Run();

	virtual SPRESULT Init();
	virtual SPRESULT Release();

	SPRESULT InitCal();
	SPRESULT TxOn( SP_BAND_INFO nBand, int nArfcn, int nPcl, BOOL bOn );
	SPRESULT MeaPower( SP_BAND_INFO nBand, int nArfcn, int nPcl , double &dPwr);
	SPRESULT UninitCal();
};
