#pragma once
#include "AlgoBase.h"
#include "NalGsV5.h"
#include "FileConfig.h"
#include "IApiRxModV3.h"

class CAlgoGsRxModV5 :
    public CAlgoBase
{
private:
    typedef CNalGsV5::Config Config;
    typedef CNalGsV5::Result Result;

    typedef IApiRxModV3::BandData BandData;
    typedef IApiRxModV3::ChannelData ChannelData;
    typedef IApiRxModV3::PointData PointData;

public:
    CAlgoGsRxModV5(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    ~CAlgoGsRxModV5(void);

    virtual SPRESULT PreInit();
    virtual void __SetAntCa();
    virtual SPRESULT Init();
    virtual SPRESULT Run();

private:
    SPRESULT InitData();
    SPRESULT AdjustCellPower(BOOL &ret);

private:
    CNalGsV5* m_pNal;
    CFileConfig* m_pFileConfig;

    IApiRxModV3* m_pApi;

    vector<Config>* m_parrConfig;
    vector<Result>* m_parrResult;

    vector<BandData> m_arrBandData;
    uint32 m_uMaxRetryTime;
    uint32 m_uRetryTime;

    int m_nMinIndex;
    int m_nMaxIndex;

    uint32 m_uIndex;

	CNvHelperV5* m_pNv;
};
