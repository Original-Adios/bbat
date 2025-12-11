#pragma once

#include "ILossFunc.h"
#include "FuncCenter.h"
#include "FileConfig.h"
#include "AlgoGsTx.h"
#include "AlgoGsTxModV3.h"
#include "AlgoGsRx.h"
#include "AlgoGsRxModV3.h"
#include "NalGs.h"
#include "NvHelper.h"

#include <deque>

class CLteGs : public ILossFunc
{
public:
    CLteGs(CImpBase *pSpat);
    virtual ~CLteGs(void);

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
    CAlgoBase* m_pTxAlgoMainPcc;
    CAlgoBase* m_pTxAlgoDivScc;
    CAlgoBase* m_pRxAlgoMainPcc;
    CAlgoBase* m_pRxAlgoDivPcc;

    CNvHelper* m_pNvHelper;
    CNalGs*    m_pNal;

    LTE_CA_E    m_CurrentCa;
    CSpatBase*  m_pSpatBase;
};
