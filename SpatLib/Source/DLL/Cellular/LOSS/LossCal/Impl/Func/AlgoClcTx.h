#pragma once
#include "AlgoBase.h"
#include "NalClc.h"
#include "FileConfig.h"
#include "IApiTxV2.h"

class CAlgoClcTx :
    public CAlgoBase
{
private:
    typedef CNalClc::Config Config;
    typedef CNalClc::Result Result;

    typedef IApiTxV2::BandData BandData;
    typedef IApiTxV2::ChannelData ChannelData;
    typedef IApiTxV2::SectionData SectionData;

public:
    CAlgoClcTx(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    ~CAlgoClcTx(void);

    virtual SPRESULT PreInit();
    virtual void __SetAntCa();
    virtual SPRESULT Init();
    virtual SPRESULT Run();
    SPRESULT Check();

private:
    SPRESULT InitData();
    static SPRESULT GetResultStatic(void*, BOOL*, uint32, uint32, uint32, uint32, uint32, uint32, uint32);
    SPRESULT GetResult(BOOL*, uint32, uint32, uint32, uint32, uint32, uint32, uint32);
    SPRESULT SetLoss(double dLoss);

private:
    CNalClc* m_pNal;
    CFileConfig* m_pFileConfig;

    LTE_CA_E m_Ca;

    IApiTxV2* m_pApi;

    vector<Config>* m_parrConfig;
    vector<Result>* m_parrResult;

    vector<vector<BandData>> m_arrarrBandData;

    RF_CABLE_LOSS m_Loss;
    IRFDevice* m_pRFTester; 

    uint32 m_uMaxRetryTime;
    uint32 m_uIndex;
};
