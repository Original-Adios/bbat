#include "StdAfx.h"
#include "DummyGSM.h"

/// 
IMPLEMENT_RUNTIME_CLASS(CDummyGSM)
//////////////////////////////////////////////////////////////////////////
CDummyGSM::CDummyGSM(void)
{
}

CDummyGSM::~CDummyGSM(void)
{
}

SPRESULT CDummyGSM::__PollAction(void)
{
    LPCSTR BAND[BI_GSM_MAX_BAND] = {"EGSM900", "DCS1800", "PCS190", "GSM850"};
    INT CHANNEL[BI_GSM_MAX_BAND][3] = {
        {1, 77, 975}, {512, 679, 885}, {517, 667, 810}, {128, 190, 250}
    };
    INT PCL[BI_GSM_MAX_BAND][2] = {
        {5, 19}, {0, 15}, {1, 13}, {6, 18}
    };
    double TXP_LIMIT[BI_GSM_MAX_BAND][2] = {
        {5.0, 33.0}, {0.0, 31.0}, {0.0, 31.0}, {5.0, 33.0}
    };
    double TXP_VALUE[BI_GSM_MAX_BAND] = {31.5, 28.5, 29.0, 32.5};
    double FER_LIMIT[BI_GSM_MAX_BAND][2] = {
        {-90.0, 90.0}, {-180, 180.0}, {-180, 180.0}, {-90.0, 90.0}
    };
    double FER_VALUE[BI_GSM_MAX_BAND] = {50.2, 100.5, 103.2, 10.9};
    for (INT i=0; i<BI_GSM_MAX_BAND; i++)
    {
        for (INT j=0; j<3; j++)
        {
            for (INT k=0; k<2; k++)
            {
                NOTIFY("TXP", LEVEL_ITEM|LEVEL_FT, TXP_LIMIT[i][0], TXP_VALUE[i], TXP_LIMIT[i][1], BAND[i], CHANNEL[i][j], "dBm", "PCL = %d", PCL[i][k]);
                NOTIFY("FER", LEVEL_ITEM|LEVEL_FT, FER_LIMIT[i][0], FER_VALUE[i], FER_LIMIT[i][1], BAND[i], CHANNEL[i][j], "Hz",  "PCL = %d", PCL[i][k]);
            }
        }
    }
    return SP_OK;
}
