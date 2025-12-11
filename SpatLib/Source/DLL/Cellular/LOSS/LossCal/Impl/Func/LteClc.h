#pragma once

#include "ILossFunc.h"
#include "FuncCenter.h"
#include "AlgoClcTx.h"
#include "AlgoClcRx.h"
#include "AlgoClcTxModV3.h"
#include "AlgoClcRxModV3.h"
#include "NalClc.h"
#include <deque>
#include "NvHelper.h"

class CLteClc : public ILossFunc
{
public:
    CLteClc(CImpBase *pSpat);
    virtual ~CLteClc(void);

    virtual SPRESULT Run();

    virtual SPRESULT Init();
    virtual SPRESULT Release();
    virtual void Deserialization(std::vector<uint8>* parrData);

private:
    CFuncCenter* m_pFuncCenter;
	CFileConfig* m_pFileConfig;
    CFuncBase* m_pTxApi;
    CFuncBase* m_pRxApi;
    CAlgoBase* m_pTxAlgoMainPcc;
    CAlgoBase* m_pTxAlgoDivScc;
    CAlgoBase* m_pRxAlgoMainPcc;
    CAlgoBase* m_pRxAlgoDivPcc;
    CNalClc*    m_pNal;

    LTE_CA_E m_CurrentCa;
    CSpatBase*  m_pSpatBase;

	CNvHelper* m_pNvHelper;
};
