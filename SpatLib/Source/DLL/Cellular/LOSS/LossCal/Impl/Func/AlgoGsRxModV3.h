#pragma once
#include "AlgoBase.h"
#include "NalGs.h"
#include "FileConfig.h"
#include "IApiRxModV3.h"

class CAlgoGsRxModV3 :
    public CAlgoBase
{
private:
    typedef CNalGs::Config Config;
    typedef CNalGs::Result Result;

    typedef IApiRxModV3::BandData BandData;
    typedef IApiRxModV3::ChannelData ChannelData;
    typedef IApiRxModV3::PointData PointData;

public:
    CAlgoGsRxModV3(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    ~CAlgoGsRxModV3(void);

    virtual SPRESULT PreInit();
    virtual void __SetAntCa();
    virtual SPRESULT Init();
    virtual SPRESULT Run();

private:
    SPRESULT InitData();
    SPRESULT AdjustCellPower(BOOL &ret);

private:
    CNalGs* m_pNal;
    CFileConfig* m_pFileConfig;

    IApiRxModV3* m_pApi;

    vector<Config>* m_parrConfig;
    vector<Result>* m_parrResult;

    vector<BandData> m_arrBandData;
    uint32 m_uMaxRetryTime;
    uint32 m_uRetryTime;

    int m_nMinIndex;
    int m_nMaxIndex;

	CNvHelper* m_pNv;
};
