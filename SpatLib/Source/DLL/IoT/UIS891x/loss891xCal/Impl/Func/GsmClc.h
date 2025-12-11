#pragma once

#include "ILossFunc_UIS8910.h"
#include "FunUIS8910.h"

class CGsmClc : public ILossFunc_UIS8910
{
public:
    CGsmClc(CImpBaseUIS8910 *pSpat);
    virtual ~CGsmClc(void);
   
	virtual SPRESULT Run();

	virtual SPRESULT Init();
	virtual SPRESULT Release();

	SPRESULT InitCal();
	SPRESULT TxOn( SP_BAND_INFO nBand, int nArfcn, int nPcl, BOOL bOn );
	SPRESULT MeaPower( SP_BAND_INFO nBand, int nArfcn, int nPcl , double &dPwr);
	SPRESULT UninitCal();
private:
    CFunUIS8910 *m_pFuncal;
	CImpBaseUIS8910  *m_pImpcal;
};
