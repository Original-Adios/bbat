#pragma once
#include "FuncBase.h"
#include "NalClc_UIS8910.h"
#include "FileConfig_UIS8910.h"
#include "IAlgo.h"
#include "IApiTx.h"

class CAlgoClcTx :
    public CFuncBase,
    public IAlgo
{
private:
    typedef CNalClc_UIS8910::Config Config;
    typedef CNalClc_UIS8910::Result Result;

    typedef IApiTx::BandData BandData;
    typedef IApiTx::ChannelData ChannelData;
    typedef IApiTx::SectionData SectionData;
    typedef IApiTx::TxSetPara TxSetPara;

public:
    CAlgoClcTx(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    ~CAlgoClcTx(void);

    virtual SPRESULT PreInit();
    virtual void SetAntCa( RF_ANT_E Ant, LTE_CA_E Ca );
    virtual SPRESULT Init();
    virtual SPRESULT Run();
    SPRESULT Check();

private:
    SPRESULT InitData();
    static SPRESULT GetResultStatic(void*, BOOL*, TxSetPara&, uint32);
    SPRESULT GetResult(BOOL*, TxSetPara&, uint32);
    void SetLoss(double dLoss);

private:
    CNalClc_UIS8910* m_pNal;
    CFileConfig_UIS8910* m_pFileConfig;

    RF_ANT_E m_Ant;
    LTE_CA_E m_Ca;

    IApiTx* m_pApi;

    vector<Config>* m_parrConfig;
    vector<Result>* m_parrResult;

    vector<vector<BandData>> m_arrarrBandData;

    RF_CABLE_LOSS m_Loss;
    IRFDevice* m_pRFTester; 

    uint32 m_uMaxRetryTime;
    uint32 m_uIndex;
};
