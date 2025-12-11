#pragma once

#include "ILossFunc_UIS8910.h"
#include "FuncCenter.h"
#include "FileConfig_UIS8910.h"
#include "AlgoGsTx.h"
#include "AlgoGsRx.h"
#include "Nv_UIS8910.h"
#include "NalGs_UIS8910.h"
#include <deque>

class CLteGs_UIS8850 : public ILossFunc_UIS8910
{
public:
    CLteGs_UIS8850(CImpBaseUIS8910 *pSpat);
    virtual ~CLteGs_UIS8850(void);

    virtual SPRESULT Run();

    virtual SPRESULT Init();
    virtual SPRESULT Release();
    virtual void Serialization(std::vector<uint8>* parrData);

public:

private:
    CFuncCenter* m_pFuncCenter;
    CFileConfig_UIS8910* m_pFileConfig;
    CFuncBase* m_pTxApi;
    CFuncBase* m_pRxApi;
    CAlgoGsTx* m_pTxAlgoMainPcc;
    CAlgoGsTx* m_pTxAlgoDivScc;
    CAlgoGsRx* m_pRxAlgoMainPcc;
    CAlgoGsRx* m_pRxAlgoDivPcc;
    CNv_UIS8910*       m_pNv;
    CNalGs_UIS8910*    m_pNal;

    LTE_CA_E    m_CurrentCa;
    CSpatBase*  m_pSpatBase;
};
