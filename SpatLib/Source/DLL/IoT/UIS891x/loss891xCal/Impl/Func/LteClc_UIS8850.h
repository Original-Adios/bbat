#pragma once

#include "ILossFunc_UIS8910.h"
#include "FuncCenter.h"
#include "AlgoClcTx.h"
#include "AlgoClcRx.h"
#include "Nv_UIS8910.h"
#include "NalClc_UIS8910.h"
#include <deque>

class CLteClc_UIS8850 : public ILossFunc_UIS8910
{
public:
    CLteClc_UIS8850(CImpBaseUIS8910 *pSpat);
    virtual ~CLteClc_UIS8850(void);

    virtual SPRESULT Run();

    virtual SPRESULT Init();
    virtual SPRESULT Release();
    virtual void Deserialization(std::vector<uint8>* parrData);
private:
	SPRESULT IniFuncMod(void);

private:
    CFuncCenter* m_pFuncCenter;
	CFileConfig_UIS8910* m_pFileConfig;
    CFuncBase* m_pTxApi;
    CFuncBase* m_pRxApi;
    CAlgoClcTx* m_pTxAlgoMainPcc;
    CAlgoClcTx* m_pTxAlgoDivScc;
    CAlgoClcRx* m_pRxAlgoMainPcc;
    CAlgoClcRx* m_pRxAlgoDivPcc;
    CNalClc_UIS8910*    m_pNal;

    LTE_CA_E m_CurrentCa;
    CSpatBase*  m_pSpatBase;

};
