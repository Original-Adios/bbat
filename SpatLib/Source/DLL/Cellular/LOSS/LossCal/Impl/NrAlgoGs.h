#pragma once
#include "CommonAlgoGsBase.h"
#include "NrApi.h"
#include "ImpBase.h"
#include "NrFileConfig.h"

class CNrAlgoGs : public CCommonAlgoGsBase, public ILossFunc
{
public:
    CNrAlgoGs(CImpBase *pImp);
    virtual ~CNrAlgoGs(void);

    virtual SPRESULT MakeGs();
    virtual SPRESULT Run();
    virtual SPRESULT Init();
    virtual SPRESULT Release();
    virtual void Serialization(std::vector<uint8> *parrData);

protected:
    CNrFileConfig m_FileConfig;

private:
    CNrApi m_Api;
};
