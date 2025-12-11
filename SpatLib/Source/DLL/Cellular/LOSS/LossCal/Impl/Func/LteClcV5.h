#pragma once

#include "ILossFunc.h"
#include "FuncCenter.h"
#include "AlgoClcTx.h"
#include "AlgoClcRx.h"
#include "AlgoClcTxModV5.h"
#include "AlgoClcRxModV5.h"
#include "NalClcV5.h"
#include <deque>
#include "NvHelperV5.h"

class CLteClcV5 : public ILossFunc
{
public:
    CLteClcV5(CImpBase *pSpat);
    virtual ~CLteClcV5(void);

    virtual SPRESULT Run();

    virtual SPRESULT Init();
    virtual SPRESULT Release();
    virtual void Deserialization(std::vector<uint8>* parrData);
    //virtual SPRESULT LoadBandInfo();

private:
    CFuncCenter* m_pFuncCenter;
	CFileConfig* m_pFileConfig;
    CFuncBase* m_pTxApi;
    CFuncBase* m_pRxApi;
    CAlgoBase* m_pTxAlgo;
    CAlgoBase* m_pRxAlgo;
    CNalClcV5*    m_pNal;

    LTE_CA_E m_CurrentCa;
    CSpatBase*  m_pSpatBase;

	CNvHelperV5* m_pNvHelper;
};
