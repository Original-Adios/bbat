#pragma once
#include "AlgoBase.h"
#include "NalClc.h"
#include "FileConfig.h"
#include "IApiRxModV3.h"

class CAlgoClcRxModV3 :
    public CAlgoBase
{
private:
    typedef CNalClc::Config Config;
    typedef CNalClc::Result Result;

    typedef IApiRxModV3::BandData BandData;
    typedef IApiRxModV3::ChannelData ChannelData;
    typedef IApiRxModV3::PointData PointData;

public:
    CAlgoClcRxModV3(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    ~CAlgoClcRxModV3(void);

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

    IApiRxModV3* m_pApi;

    vector<Config>* m_parrConfig;
    vector<Result>* m_parrResult;

    vector<vector<BandData>> m_arrarrBandData;
    uint32 m_uMaxRetryTime;
    uint32 m_uRetryTime;

    RF_CABLE_LOSS m_Loss;
    IRFDevice* m_pRFTester; 

	CNvHelper* m_pNv;
};
