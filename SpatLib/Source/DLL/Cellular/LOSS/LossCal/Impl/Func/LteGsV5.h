#pragma once

#include "ILossFunc.h"
#include "FuncCenter.h"
#include "FileConfig.h"
#include "AlgoGsTx.h"
#include "AlgoGsTxModV5.h"
#include "AlgoGsRx.h"
#include "AlgoGsRxModV5.h"
#include "NalGsV5.h"
#include "NvHelperV5.h"

#include <deque>

class CLteGsV5 : public ILossFunc
{
public:
    CLteGsV5(CImpBase *pSpat);
    virtual ~CLteGsV5(void);

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
    CNvHelperV5* m_pNvHelper;
    CNalGsV5*    m_pNal;

    LTE_CA_E    m_CurrentCa;
    CSpatBase*  m_pSpatBase;
};
