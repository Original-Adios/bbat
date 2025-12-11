#pragma once
#include "AlgoBase.h"
#include "NalGs.h"
#include "FileConfig.h"
#include "IApiTxV2.h"

class CAlgoGsTx :
    public CAlgoBase
{
private:
    typedef CNalGs::Config Config;
    typedef CNalGs::Result Result;

    typedef IApiTxV2::BandData BandData;
    typedef IApiTxV2::ChannelData ChannelData;
    typedef IApiTxV2::SectionData SectionData;

public:
    CAlgoGsTx(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    ~CAlgoGsTx(void);

    virtual SPRESULT PreInit();
    virtual void __SetAntCa();
    virtual SPRESULT Init();
    virtual SPRESULT Run();

private:
    SPRESULT InitData();
    static SPRESULT GetResultStatic(void*, BOOL*, uint32, uint32, uint32, uint32, uint32, uint32, uint32);
    SPRESULT GetResult(BOOL*, uint32, uint32, uint32, uint32, uint32, uint32, uint32);

private:
    CNalGs* m_pNal;
    CFileConfig* m_pFileConfig;

    LTE_CA_E m_Ca;

    IApiTxV2* m_pApi;

    vector<Config>* m_parrConfig;
    vector<Result>* m_parrResult;

    vector<BandData> m_arrBandData;
    uint32 m_uMaxRetryTime;
};
