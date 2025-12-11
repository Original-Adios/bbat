#pragma once
#include "AlgoBase.h"
#include "NalGs.h"
#include "FileConfig.h"
#include "IApiTxModV3.h"

class CAlgoGsTxModV3 :
    public CAlgoBase
{
private:
    typedef CNalGs::Config Config;
    typedef CNalGs::Result Result;

    typedef IApiTxModV3::BandData BandData;
    typedef IApiTxModV3::ChannelData ChannelData;
    typedef IApiTxModV3::SectionData SectionData;

public:
    CAlgoGsTxModV3(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    ~CAlgoGsTxModV3(void);

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
    CNalGs* m_pNal;
    CFileConfig* m_pFileConfig;

    IApiTxModV3* m_pApi;

    vector<Config>* m_parrConfig;
    vector<Result>* m_parrResult;

    vector<BandData> m_arrBandData;
    uint32 m_uMaxRetryTime;

	CNvHelper* m_pNv;
};
