#pragma once
#include "global_def.h"
#include "Global/uni_error_def.h"
#include <vector>
#include "CommonApiBase.h"
#include "CommonDefine.h"
#include "ImpBase.h"

using namespace std;

class CCommonAlgoClcBase
{
public:
    typedef CCommonApiBase::PointData PointData;

private:
    struct RefData
    {
        double dGsData;
        double dLoss;

        RefData()
        {
            memset(this, 0, sizeof RefData);
        }
    };

public:
    CCommonAlgoClcBase(CImpBase *pImp);
    virtual ~CCommonAlgoClcBase(void);

    SPRESULT ClcLoss();

public:
    CCommonApiBase *m_pApi;
    vector<GsData> m_arrGsData;
    std::vector<RF_CABLE_LOSS_POINT_EX> m_arrLoss;
    RF_CABLE_LOSS_UNIT_EX* m_pLoss;

protected:
    virtual SPRESULT CustomizeLossData();
    virtual LPCSTR GetBandName(int nBand) = 0;

protected:
    vector<PointData> m_arrPoint[MAX_RF_ANT];

private:
    SPRESULT GetGsData();
    BOOL CheckLoss(PointData *pLossData);
    BOOL CheckLossTx(PointData *pLossData);
    BOOL CheckLossRx(PointData *pLossData);
    BOOL GenLossTable();

    SPRESULT SetLoss(double dLoss);
private:
    BOOL m_bLossCheck;
    double m_dPreLoss;
    double m_dSpecLower;
    double m_dSpecUpper;

    int m_nMaxRetryTime;
    int m_nRetryTime;

    CImpBase *m_pImp;
};
