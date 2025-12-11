#pragma once

#include "ILossFunc_UIS8910.h"
#include <list>
#include "FunUIS8910.h"

class CGsmGs : public ILossFunc_UIS8910
{
public:
    CGsmGs(CImpBaseUIS8910 *pImp);
    virtual ~CGsmGs(void);

    virtual SPRESULT Run();

    virtual SPRESULT Init();
    virtual SPRESULT Release();
    virtual void ConfigFreq();
   
    SPRESULT InitCal();
    SPRESULT TxOn( SP_BAND_INFO nBand, int nArfcn, int nPcl, BOOL bOn );
    SPRESULT MeaPower( SP_BAND_INFO nBand, int nArfcn, int nPcl , double &dPwr);
    SPRESULT UninitCal();

private:
	std::list<double> m_listFreq;
    CFunUIS8910 *m_pFuncal;
	CImpBaseUIS8910  *m_pImpcal;
};
