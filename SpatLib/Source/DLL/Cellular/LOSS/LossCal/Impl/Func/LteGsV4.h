#pragma once

#include "ILossFunc.h"
#include "FuncCenter.h"
#include "FileConfig.h"
#include "AlgoGsTx.h"
#include "AlgoGsTxModV4.h"
#include "AlgoGsRx.h"
#include "AlgoGsRxModV4.h"
#include "NalGsV4.h"
#include "NvHelperV4.h"

#include <deque>

class CLteGsV4 : public ILossFunc
{
public:
    CLteGsV4(CImpBase *pSpat);
    virtual ~CLteGsV4(void);

    virtual SPRESULT Run();

    virtual SPRESULT Init();
    virtual SPRESULT Release();
    virtual void Serialization(std::vector<uint8>* parrData);

public:

private:
    CFuncCenter* m_pFuncCenter;
    CFileConfig* m_pFileConfig;
    CFuncBase* m_pTxApi;
    CFuncBase* m_pRxApi;
    CAlgoBase* m_pTxAlgo;
    CAlgoBase* m_pRxAlgo;
    CNvHelperV4* m_pNvHelper;
    CNalGsV4*    m_pNal;

    LTE_CA_E    m_CurrentCa;
    CSpatBase*  m_pSpatBase;
};
