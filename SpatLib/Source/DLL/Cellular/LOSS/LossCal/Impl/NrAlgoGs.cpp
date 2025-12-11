#include "StdAfx.h"
#include "NrAlgoGs.h"

CNrAlgoGs::CNrAlgoGs(CImpBase *pImp) : CCommonAlgoGsBase(pImp), ILossFunc(pImp)
{
    m_Api.m_hDUT = pImp->m_hDUT;
    m_Api.m_pRFTester = pImp->m_pRFTester;
    m_FileConfig.m_pSpatBase = pImp;

    m_nTxParamStart = 0;
    m_nTxParamEnd = 700;
    m_dTxParamSlope = -0.25;
    m_dTxTarget = 0;

    m_dRxCellpowerStart = -25;
    m_dRxCellpowerEnd = -75;
    m_nRxParam = 45;
    m_dRxTarget = -5;

    m_dTolerance = 1;
    m_bAdjust = TRUE;

    this->m_pApi = &m_Api;
}

CNrAlgoGs::~CNrAlgoGs(void)
{
}

SPRESULT CNrAlgoGs::MakeGs()
{
    m_FileConfig.Load();

    int nCount = 0;
    NR_BAND_E arrSupportBand[MAX_NR_BAND];
#ifndef Internal_Debug
    CHKRESULT(SP_NR_CAL_SupportBand(ILossFunc::m_pImp->m_hDUT, &nCount, arrSupportBand));
#else
    nCount = MAX_NR_BAND;
    for (int i = 0; i < MAX_NR_BAND; i++)
    {
        arrSupportBand[i] = CNrUtility::m_BandInfo[i].nNumeral;
    }
#endif

    for (int i = 0; i < MAX_NR_BAND; i++)
    {
        if (m_FileConfig.m_arrBand[i])
        {
            for (int j = 0; j < nCount; j++)
            {
                if (arrSupportBand[j] == CNrUtility::m_BandInfo[i].Band)
                {
                    goto found;
                }
            }
            return SP_E_INVALID_PARAMETER;
        found:
            NR_ANT_CH_SUPPORT_T TX;
            NR_ANT_CH_SUPPORT_T RX;
#ifndef Internal_Debug
            CHKRESULT(SP_NR_ANT_CH_SUPPORT(ILossFunc::m_pImp->m_hDUT, CNrUtility::m_BandInfo[i].BandIdent, &TX, &RX));

            //TX.ANT[0] = 0;
            //TX.ANT[1] = 0;
            //RX.ANT[0] = 0;
            //RX.ANT[1] = 0;
#else
            TX.ANT[0] = 1;
            TX.ANT[1] = 1;
            TX.Channel[0] = 1;

            RX.ANT[0] = 1;
            RX.ANT[1] = 1;
            RX.ANT[2] = 1;
            RX.ANT[3] = 1;
            RX.Channel[0] = 1;
#endif

            READ_NV_PARAM_RLT_ANT_MAP AntMap;
#ifndef Internal_Debug
            CHKRESULT(SP_ReadAntMap(ILossFunc::m_pImp->m_hDUT, RF_MODE_NR, CNrUtility::m_BandInfo[i].nNumeral, &AntMap));
#endif

            for (int k = 0; k < 4; k++)
            {
                PointInfo point;
                point.nAnt = k;
                point.Band = i;

                if (TX.ANT[k])
                {
                    point.bTx = TRUE;
                    point.RfAnt = (RF_ANT_E)(AntMap.Tx[k] - 1);

                    point.uArfcn = CNrUtility::GetUlValidArfcn((NR_BAND_E)i, 0, NR_BW_5MHz);
                    point.dFreq = CNrUtility::m_BandInfo[i].UlFreq.dBegin;
                    m_arrBandInfo.push_back(point);

                    point.uArfcn = CNrUtility::GetUlValidArfcn((NR_BAND_E)i, CNrUtility::GetUlFreqRange((NR_BAND_E)i) / 2.0, NR_BW_5MHz);
                    point.dFreq = CNrUtility::GetFreq(point.uArfcn);
                    m_arrBandInfo.push_back(point);

                    point.uArfcn = CNrUtility::GetUlValidArfcn((NR_BAND_E)i, CNrUtility::GetUlFreqRange((NR_BAND_E)i), NR_BW_5MHz);
                    point.dFreq = CNrUtility::m_BandInfo[i].UlFreq.dEnd;
                    m_arrBandInfo.push_back(point);
                }
                if (RX.ANT[k])
                {
                    point.bTx = FALSE;
                    point.RfAnt = (RF_ANT_E)(AntMap.Rx[k] - 1);

                    point.uArfcn = CNrUtility::GetDlValidArfcn((NR_BAND_E)i, 0, NR_BW_5MHz);
                    point.dFreq = CNrUtility::m_BandInfo[i].DlFreq.dBegin;
                    m_arrBandInfo.push_back(point);

                    point.uArfcn = CNrUtility::GetDlValidArfcn((NR_BAND_E)i, CNrUtility::GetDlFreqRange((NR_BAND_E)i) / 2.0, NR_BW_5MHz);
                    point.dFreq = CNrUtility::GetFreq(point.uArfcn);
                    m_arrBandInfo.push_back(point);

                    point.uArfcn = CNrUtility::GetDlValidArfcn((NR_BAND_E)i, CNrUtility::GetDlFreqRange((NR_BAND_E)i), NR_BW_5MHz);
                    point.dFreq = CNrUtility::m_BandInfo[i].DlFreq.dEnd;
                    m_arrBandInfo.push_back(point);
                }
            }
        }
    }

    CHKRESULT(RunBandInfo());

    return SP_OK;
}

SPRESULT CNrAlgoGs::Run()
{
    this->m_pLoss = &ILossFunc::m_pImp->m_lossVal.nrLoss;
    CHKRESULT(MakeGs());

    return SP_OK;
}

SPRESULT CNrAlgoGs::Init()
{
    return SP_OK;
}

SPRESULT CNrAlgoGs::Release()
{
    return SP_OK;
}

void CNrAlgoGs::Serialization(std::vector<uint8> *parrData)
{
    uint32 uSize = m_arrGsData.size() * sizeof(GsData);
    parrData->resize(uSize);
    memcpy(&(*parrData)[0], &m_arrGsData[0], uSize);
}
