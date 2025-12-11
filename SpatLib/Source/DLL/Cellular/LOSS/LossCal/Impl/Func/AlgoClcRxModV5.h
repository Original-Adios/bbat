#pragma once
#include "AlgoBase.h"
#include "NalClcV5.h"
#include "FileConfig.h"
#include "IApiRxModV3.h"

class CAlgoClcRxModV5 :
    public CAlgoBase
{
private:
    typedef CNalClcV5::Config Config;
    typedef CNalClcV5::Result Result;

    typedef IApiRxModV3::BandData BandData;
    typedef IApiRxModV3::ChannelData ChannelData;
    typedef IApiRxModV3::PointData PointData;

public:
    CAlgoClcRxModV5(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    ~CAlgoClcRxModV5(void);

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
    CNalClcV5* m_pNal;
    CFileConfig* m_pFileConfig;

    IApiRxModV3* m_pApi;

    vector<Config>* m_parrConfig;
    vector<Result>* m_parrResult;

    vector<vector<BandData>> m_arrarrBandData;
    uint32 m_uMaxRetryTime;
    uint32 m_uRetryTime;

    RF_CABLE_LOSS m_Loss;
    IRFDevice* m_pRFTester; 

    CNvHelperV5* m_pNv;
};
