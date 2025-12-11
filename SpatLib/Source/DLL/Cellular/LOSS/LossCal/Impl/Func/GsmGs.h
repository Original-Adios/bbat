#pragma once

#include "GsmBase.h"
#include <list>

class CGsmGs : public CGsmBase
{
public:
    CGsmGs(CImpBase *pSpat);
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
};
