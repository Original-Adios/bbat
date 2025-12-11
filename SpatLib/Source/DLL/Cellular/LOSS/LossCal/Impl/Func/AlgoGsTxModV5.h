#pragma once
#include "AlgoBase.h"
#include "NalGsV5.h"
#include "FileConfig.h"
#include "IApiTxModV3.h"

class CAlgoGsTxModV5 :
    public CAlgoBase
{
private:
    typedef CNalGsV5::Config Config;
    typedef CNalGsV5::Result Result;

    typedef IApiTxModV3::BandData BandData;
    typedef IApiTxModV3::ChannelData ChannelData;
    typedef IApiTxModV3::SectionData SectionData;

public:
    CAlgoGsTxModV5(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    ~CAlgoGsTxModV5(void);

    virtual SPRESULT PreInit();
    virtual SPRESULT Init();
    virtual SPRESULT Run();
protected:
    virtual void __SetAntCa();
private:
    SPRESULT InitData();
    static SPRESULT GetResultStatic(void*, BOOL*, uint32, uint32, uint32, uint32, uint32, uint32, uint32);
    SPRESULT GetResult(BOOL*, uint32, uint32, uint32, uint32, uint32, uint32, uint32);

private:
    CNalGsV5* m_pNal;
    CFileConfig* m_pFileConfig;

    IApiTxModV3* m_pApi;

    vector<Config>* m_parrConfig;
    vector<Result>* m_parrResult;

    vector<BandData> m_arrBandData;
    uint32 m_uMaxRetryTime;

    uint32 m_uIndex;
    vector<BandData> m_BandData;

	CNvHelperV5* m_pNv;
};
