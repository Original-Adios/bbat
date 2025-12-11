#include "stdafx.h"
#include "NrAlgoGsV4.h"
#include "NrDefine.h"

CNrAlgoGsV4::CNrAlgoGsV4(CImpBase* pImp) : CNrAlgoGs(pImp)
{
    m_Api.m_hDUT = pImp->m_hDUT;
    m_Api.m_pRFTester = pImp->m_pRFTester;

    this->m_pApi = &m_Api;
    m_dRxCellpowerStart = -20;
    m_dRxCellpowerEnd = -85;
}

CNrAlgoGsV4::~CNrAlgoGsV4(void)
{
}

SPRESULT CNrAlgoGsV4::MakeGs()
{
    m_FileConfig.Load();
    m_nRxParam = m_FileConfig.m_nRxIndex;

    int nCount = 0;
    NR_BAND_E arrSupportBand[MAX_NR_BAND];
    CHKRESULT(SP_NR_CAL_SupportBand(ILossFunc::m_pImp->m_hDUT, &nCount, arrSupportBand));

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
            READ_NV_PARAM_RLT_ANT_MAP AntMap;
            CHKRESULT(SP_ReadAntMap(ILossFunc::m_pImp->m_hDUT, RF_MODE_NR, CNrUtility::m_BandInfo[i].nNumeral, &AntMap));

            NR_CALI_RF_CH_V4_T PathInfo;
            CHKRESULT(Load_PathInfo((NR_BAND_E)i, PathInfo));

            NR_VERSION_T NrVersion;
            CHKRESULT(SP_NR_COMMOM_GetVersion(ILossFunc::m_pImp->m_hDUT, &NrVersion));

            //2RX/4RX
            for (int j = 0; j < MAX_NR_PATH_TYPE; j++)
            {
                for (int k = 0; k < PathInfo.path_cnt[j]; k++)
                {
                    NR_CALI_RF_CH_ELE_V4_T* pPathInfo = &PathInfo.pathinfo[j][k];

                    for (int m = 0; m < 4; m++)
                    {
                        PointInfo point;
                        point.nAnt = m;
                        point.Band = i;
                        point.nVoltage = m_FileConfig.m_nVoltage;

                        if (m < pPathInfo->tx_ant_cnt &&
                            pPathInfo->ant_tx[m].cal_en)
                        {
                            point.bTx = TRUE;
                            if (NrVersion.CAL_NR_ModemV4_PATH_ANT_REQ)
                            {
                                point.RfAnt = (RF_ANT_E)(pPathInfo->ant_tx[m].antA - 1);
                            }
                            else
                            {
                                point.RfAnt = (RF_ANT_E)(AntMap.Tx[m] - 1);
                            }
                            point.nChannel = ConvertToChannel(j, pPathInfo->ctrl_path_index);

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

                        if (m < pPathInfo->rx_ant_cnt &&
                            pPathInfo->ant_rx[m].cal_en)
                        {
                            point.bTx = FALSE;

                            if (NrVersion.CAL_NR_ModemV4_PATH_ANT_REQ)
                            {
                                point.RfAnt = (RF_ANT_E)(pPathInfo->ant_rx[m].antA - 1);
                            }
                            else
                            {
                                point.RfAnt = (RF_ANT_E)(AntMap.Rx[m] - 1);
                            }
                            point.nChannel = ConvertToChannel(j, pPathInfo->ctrl_path_index);

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
        }
    }

    CHKRESULT(RunBandInfo());

    return SP_OK;
}

SPRESULT CNrAlgoGsV4::Load_PathInfo(NR_BAND_E Band, NR_CALI_RF_CH_V4_T& PathInfo)
{
    ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

    CHKRESULT(SP_NR_ModemV4_ANT_CH_SUPPORT(ILossFunc::m_pImp->m_hDUT, CNrUtility::m_BandInfo[Band].BandIdent, &PathInfo));

    ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "%s", CNrUtility::m_BandInfo[Band].NameA);
    for (int j = 0; j < 2; j++)
    {
        if (j == 0)
        {
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "2RX Count: %d", PathInfo.path_cnt[j]);
        }
        else
        {
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "4RX Count: %d", PathInfo.path_cnt[j]);
        }

        for (int k = 0; k < PathInfo.path_cnt[j]; k++)
        {
            NR_CALI_RF_CH_ELE_V4_T* pEle = &PathInfo.pathinfo[j][k];
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Index%d: Path Index = %d", k, pEle->ctrl_path_index);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Index%d: Ant TX Count = %d", k, pEle->tx_ant_cnt);
            for (int l = 0; l < pEle->tx_ant_cnt; l++)
            {
                ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Index%d TX Index%d: Ant = %d", k, l, pEle->ant_tx[l].antP);
                ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Index%d TX Index%d: cal_en = %d", k, l, pEle->ant_tx[l].cal_en);
                ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Index%d TX Index%d: cali_save_index = %d", k, l, pEle->ant_tx[l].cali_save_index);
                ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Index%d TX Index%d: cali_save_en = %d", k, l, pEle->ant_tx[l].cali_save_en);
            }
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Index%d: Ant RX Count = %d", k, pEle->rx_ant_cnt);
            for (int l = 0; l < pEle->rx_ant_cnt; l++)
            {
                ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Index%d RX Index%d: Ant = %d", k, l, pEle->ant_rx[l].antP);
                ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Index%d RX Index%d: cal_en = %d", k, l, pEle->ant_rx[l].cal_en);
                ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Index%d RX Index%d: cali_save_index = %d", k, l, pEle->ant_rx[l].cali_save_index);
                ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Index%d RX Index%d: cali_save_en = %d", k, l, pEle->ant_rx[l].cali_save_en);
            }
        }
    }

    return SP_OK;
}
