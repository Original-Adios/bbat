#pragma once
#include "FuncBase.h"
#include "NalGs_UIS8910.h"
#include "FileConfig_UIS8910.h"
#include "IAlgo.h"
#include "IApiRx.h"

class CAlgoGsRx :
    public CFuncBase,
    public IAlgo
{
private:
    typedef CNalGs_UIS8910::Config Config;
    typedef CNalGs_UIS8910::Result Result;

    typedef IApiRx::BandData BandData;
    typedef IApiRx::ChannelData ChannelData;
    typedef IApiRx::PointData PointData;

public:
    CAlgoGsRx(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    ~CAlgoGsRx(void);

    virtual SPRESULT PreInit();
    virtual void SetAntCa( RF_ANT_E Ant, LTE_CA_E Ca );
    virtual SPRESULT Init();
    virtual SPRESULT Run();

private:
    SPRESULT InitData();
    SPRESULT AdjustCellPower(BOOL &ret);

private:
    CNalGs_UIS8910* m_pNal;
    CFileConfig_UIS8910* m_pFileConfig;

    RF_ANT_E m_Ant;
    LTE_CA_E m_Ca;

    IApiRx* m_pApi;

    vector<Config>* m_parrConfig;
    vector<Result>* m_parrResult;

    vector<BandData> m_arrBandData;
    uint32 m_uMaxRetryTime;
    uint32 m_uRetryTime;

    int m_nMinIndex;
    int m_nMaxIndex;
};
