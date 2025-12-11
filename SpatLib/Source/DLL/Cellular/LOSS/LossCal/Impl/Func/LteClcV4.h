#pragma once

#include "ILossFunc.h"
#include "FuncCenter.h"
#include "AlgoClcTx.h"
#include "AlgoClcRx.h"
#include "AlgoClcTxModV4.h"
#include "AlgoClcRxModV4.h"
#include "NalClcV4.h"
#include <deque>
#include "NvHelperV4.h"

class CLteClcV4 : public ILossFunc
{
public:
    CLteClcV4(CImpBase *pSpat);
    virtual ~CLteClcV4(void);

    virtual SPRESULT Run();

    virtual SPRESULT Init();
    virtual SPRESULT Release();
    virtual void Deserialization(std::vector<uint8>* parrData);
    virtual SPRESULT LoadBandInfo();

private:
    CFuncCenter* m_pFuncCenter;
	CFileConfig* m_pFileConfig;
    CFuncBase* m_pTxApi;
    CFuncBase* m_pRxApi;
    CAlgoBase* m_pTxAlgo;
    CAlgoBase* m_pRxAlgo;
    CNalClcV4*    m_pNal;

    LTE_CA_E m_CurrentCa;
    CSpatBase*  m_pSpatBase;

	CNvHelperV4* m_pNvHelper;
};
