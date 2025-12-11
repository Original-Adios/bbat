#pragma once
#include "AlgoBase.h"
#include "NalClcV5.h"
#include "FileConfig.h"
#include "IApiTxModV3.h"

class CAlgoClcTxModV5 :
    public CAlgoBase
{
private:
    typedef CNalClcV5::Config Config;
    typedef CNalClcV5::Result Result;

    typedef IApiTxModV3::BandData BandData;
    typedef IApiTxModV3::ChannelData ChannelData;
    typedef IApiTxModV3::SectionData SectionData;

public:
    CAlgoClcTxModV5(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    ~CAlgoClcTxModV5(void);

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
    CNalClcV5* m_pNal;
    CFileConfig* m_pFileConfig;

    IApiTxModV3* m_pApi;

    vector<Config>* m_parrConfig;
    vector<Result>* m_parrResult;

    vector<vector<BandData>> m_arrarrBandData;

    RF_CABLE_LOSS m_Loss;
    IRFDevice* m_pRFTester; 

    uint32 m_uMaxRetryTime;
    uint32 m_uIndex;

	CNvHelperV5* m_pNv;
};
