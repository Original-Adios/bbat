#pragma once
#include "AlgoBase.h"
#include "NalGsV4.h"
#include "FileConfig.h"
#include "IApiRxModV3.h"

class CAlgoGsRxModV4 :
    public CAlgoBase
{
private:
    typedef CNalGsV4::Config Config;
    typedef CNalGsV4::Result Result;

    typedef IApiRxModV3::BandData BandData;
    typedef IApiRxModV3::ChannelData ChannelData;
    typedef IApiRxModV3::PointData PointData;

public:
    CAlgoGsRxModV4(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    ~CAlgoGsRxModV4(void);

    virtual SPRESULT PreInit();
    virtual void __SetAntCa();
    virtual SPRESULT Init();
    virtual SPRESULT Run();

private:
    SPRESULT InitData();
    SPRESULT AdjustCellPower(BOOL &ret,int pathtype);

private:
    CNalGsV4* m_pNal;
    CFileConfig* m_pFileConfig;

    IApiRxModV3* m_pApi;

    vector<Config>* m_parrConfig;
    vector<Result>* m_parrResult;

    vector<BandData> m_arrBandData;
    vector<int> m_arrBandAntIndex;
    uint32 m_uMaxRetryTime;
    uint32 m_uRetryTime;

    int m_nMinIndex;
    int m_nMaxIndex;

	CNvHelperV4* m_pNv;
};
