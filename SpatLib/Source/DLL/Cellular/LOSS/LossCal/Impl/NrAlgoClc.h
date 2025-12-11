#pragma once
#include "CommonAlgoClcBase.h"
#include "NrApi.h"
#include "ImpBase.h"

class CNrAlgoClc : public CCommonAlgoClcBase, public ILossFunc
{
public:
    CNrAlgoClc(CImpBase *pImp);
    virtual ~CNrAlgoClc(void);

    virtual SPRESULT Run();
    virtual SPRESULT Init();
    virtual SPRESULT Release();
    virtual void Deserialization( std::vector<uint8>* parrData );

protected:
    virtual LPCSTR GetBandName( int nBand );


private:
    CNrApi m_Api;
};

