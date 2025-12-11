#pragma once
#include "AlgoBase.h"
#include "NalClcV4.h"
#include "FileConfig.h"
#include "IApiTxModV3.h"

class CAlgoClcTxModV4 :
    public CAlgoBase
{
private:
    typedef CNalClcV4::Config Config;
    typedef CNalClcV4::Result Result;

    typedef IApiTxModV3::BandData BandData;
    typedef IApiTxModV3::ChannelData ChannelData;
    typedef IApiTxModV3::SectionData SectionData;

public:
    CAlgoClcTxModV4(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    ~CAlgoClcTxModV4(void);

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
    CNalClcV4* m_pNal;
    CFileConfig* m_pFileConfig;

    IApiTxModV3* m_pApi;

    vector<Config>* m_parrConfig;
    vector<Result>* m_parrResult;

    vector<vector<BandData>> m_arrarrBandData;

    RF_CABLE_LOSS m_Loss;
    IRFDevice* m_pRFTester; 

    uint32 m_uMaxRetryTime;
    uint32 m_uIndex;

	CNvHelperV4* m_pNv;
};
