#pragma once
#include "AlgoBase.h"
#include "NalClc.h"
#include "FileConfig.h"
#include "IApiRxV3.h"

class CAlgoClcRx :
    public CAlgoBase
{
private:
    typedef CNalClc::Config Config;
    typedef CNalClc::Result Result;

    typedef IApiRxV3::BandData BandData;
    typedef IApiRxV3::ChannelData ChannelData;
    typedef IApiRxV3::PointData PointData;

public:
    CAlgoClcRx(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    ~CAlgoClcRx(void);

    virtual SPRESULT PreInit();
    virtual void __SetAntCa();
    virtual SPRESULT Init();
    virtual SPRESULT Run();
    SPRESULT Check();

private:
    SPRESULT InitData();
    SPRESULT AdjustLoss(BOOL &ret, uint32 uIndex);
    SPRESULT CheckLoss(uint32 uIndex);
    SPRESULT SetLoss(double dLoss);

private:
    CNalClc* m_pNal;
    CFileConfig* m_pFileConfig;

    LTE_CA_E m_Ca;

    IApiRxV3* m_pApi;

    vector<Config>* m_parrConfig;
    vector<Result>* m_parrResult;

    vector<vector<BandData>> m_arrarrBandData;
    uint32 m_uMaxRetryTime;
    uint32 m_uRetryTime;

    RF_CABLE_LOSS m_Loss;
    IRFDevice* m_pRFTester; 

};
