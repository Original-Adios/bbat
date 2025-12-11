#pragma once
#include "AlgoBase.h"
#include "NalGs.h"
#include "FileConfig.h"
#include "IApiRxV3.h"

class CAlgoGsRx :
    public CAlgoBase
{
private:
    typedef CNalGs::Config Config;
    typedef CNalGs::Result Result;

    typedef IApiRxV3::BandData BandData;
    typedef IApiRxV3::ChannelData ChannelData;
    typedef IApiRxV3::PointData PointData;

public:
    CAlgoGsRx(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    ~CAlgoGsRx(void);

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

    LTE_CA_E m_Ca;

    IApiRxV3* m_pApi;

    vector<Config>* m_parrConfig;
    vector<Result>* m_parrResult;

    vector<BandData> m_arrBandData;
    uint32 m_uMaxRetryTime;
    uint32 m_uRetryTime;

    int m_nMinIndex;
    int m_nMaxIndex;
};
