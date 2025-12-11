#include "stdafx.h"
#include "NrAlgoGsV5.h"
#include "NrDefine.h"

CNrAlgoGsV5::CNrAlgoGsV5(CImpBase* pImp) : CNrAlgoGs(pImp)
{
    m_Api.m_hDUT = pImp->m_hDUT;
    m_Api.m_pRFTester = pImp->m_pRFTester;

    this->m_pApi = &m_Api;
    m_dRxCellpowerStart = -20;
    m_dRxCellpowerEnd = -85;
}

CNrAlgoGsV5::~CNrAlgoGsV5(void)
{
}

SPRESULT CNrAlgoGsV5::MakeGs()
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
            NR_CALI_RF_V5_PATH_INFO_T PathInfo;
            CHKRESULT(Load_PathInfo((NR_BAND_E)i, PathInfo));

            NR_VERSION_T NrVersion;
            CHKRESULT(SP_NR_COMMOM_GetVersion(ILossFunc::m_pImp->m_hDUT, &NrVersion));

            //2RX/4RX
            for (int j = 0; j < MAX_PATHGROUP_CNT; j++)
            {
                NR_CALI_RF_V5_PATH_GROUP_T* pPathInfo = &PathInfo.PathGroup[j];

                for (int m = 0; m < 4; m++)
                {
                    PointInfo point;
                    point.nAnt = m;
                    point.Band = i;
                    point.nVoltage = m_FileConfig.m_nVoltage;

                    if (m < pPathInfo->tx_path_cnt &&
                        pPathInfo->tx_pathinfo[m].Tx_Cali_En.ApcRefEn)
                    {
                        point.bTx = TRUE;
                        point.RfAnt = (RF_ANT_E)(pPathInfo->tx_pathinfo[m].antP - 1);
                        point.nChannel = pPathInfo->tx_pathinfo[m].path_id;
                        point.PathGroup = pPathInfo->GroupID;

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

                    if (m < pPathInfo->rx_path_cnt &&
                        pPathInfo->rx_pathinfo[m].Rx_Cali_En.AgcRefEn)
                    {
                        point.bTx = FALSE;

                        point.RfAnt = (RF_ANT_E)(pPathInfo->rx_pathinfo[m].antP - 1);
                        point.nChannel = pPathInfo->rx_pathinfo[m].path_id;
                        point.PathGroup = pPathInfo->GroupID;

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

    CHKRESULT(RunBandInfo());

    return SP_OK;
}

SPRESULT CNrAlgoGsV5::Load_PathInfo(NR_BAND_E Band, NR_CALI_RF_V5_PATH_INFO_T& PathInfo)
{
    ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

    CHKRESULT(SP_NR_ModemV5_ANT_CH_SUPPORT(ILossFunc::m_pImp->m_hDUT, CNrUtility::m_BandInfo[Band].BandIdent, &PathInfo));

    ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "%s", CNrUtility::m_BandInfo[Band].NameA);

    for (int GroupIndex = 0; GroupIndex < MAX_PATHGROUP_CNT; GroupIndex++)
    {
        if (PathInfo.PathGroup[GroupIndex].tx_path_cnt != 0 || PathInfo.PathGroup[GroupIndex].rx_path_cnt != 0)
        {
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "PathGroupIndex = %d, GroupID = %d", GroupIndex, PathInfo.PathGroup[GroupIndex].GroupID);
        }

        ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Tx Path Count = %d", PathInfo.PathGroup[GroupIndex].tx_path_cnt);
        for (int i = 0; i < PathInfo.PathGroup[GroupIndex].tx_path_cnt; i++)
        {
            NR_CALI_RF_V5_PATH_ELE_T* pEle = &PathInfo.PathGroup[GroupIndex].tx_pathinfo[i];

            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Tx Path%d: PathIndex = %d", i, pEle->path_id);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Tx Path%d: bSplitFlag = %d", i, pEle->bSplitFlag);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Tx Path%d: antL = %d", i, pEle->antL);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Tx Path%d: antP = %d", i, pEle->antP);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Tx Path%d: Srs_1T_Switch = %d, %d, %d, %d", i, pEle->Srs_1T_Switch[0], pEle->Srs_1T_Switch[1], pEle->Srs_1T_Switch[2], pEle->Srs_1T_Switch[3]);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Tx Path%d: Srs_2T_Switch = %d, %d", i, pEle->Srs_2T_Switch[0], pEle->Srs_2T_Switch[1]);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Tx Path%d: Srs_1T_AntP = %d, %d, %d, %d", i, pEle->Srs_1T_AntP[0], pEle->Srs_1T_AntP[1], pEle->Srs_1T_AntP[2], pEle->Srs_1T_AntP[3]);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Tx Path%d: Srs_2T_AntP = %d, %d", i, pEle->Srs_2T_AntP[0], pEle->Srs_2T_AntP[1]);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Tx Path%d: PowerClass = %d", i, pEle->PowerClass);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Tx Path%d: PMax = %d", i, pEle->PMax);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Tx Path%d: Modulation = %d", i, pEle->Modulation);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Tx Path%d: ModulationNumber = %d", i, pEle->ModulationNumber);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Tx Path%d: SubBandNumber = %d", i, pEle->SubBandNumber);

            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Tx Path%d: Cal En: SmartAntEn = %d", i, pEle->Tx_Cali_En.SmartAntEn);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Tx Path%d: Cal En: AsdivEn = %d", i, pEle->Tx_Cali_En.AsdivEn);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Tx Path%d: Cal En: PaDroopEn = %d", i, pEle->Tx_Cali_En.PaDroopEn);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Tx Path%d: Cal En: ApcRefEn = %d", i, pEle->Tx_Cali_En.ApcRefEn);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Tx Path%d: Cal En: ApcCompEn = %d", i, pEle->Tx_Cali_En.ApcCompEn);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Tx Path%d: Cal En: PdetEn = %d", i, pEle->Tx_Cali_En.PdetEn);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Tx Path%d: Cal En: TxIrrEn = %d", i, pEle->Tx_Cali_En.TxIrrEn);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Tx Path%d: Cal En: TxDcoc = %d", i, pEle->Tx_Cali_En.TxDcoc);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Tx Path%d: Cal En: TxDpdEn = %d", i, pEle->Tx_Cali_En.TxDpdEn);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Tx Path%d: Cal En: TxSrsEn = %d", i, pEle->Tx_Cali_En.TxSrsEn);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Tx Path%d: Cal En: FbrxTxIrrEn = %d", i, pEle->Tx_Cali_En.FbrxTxIrrEn);
        }

        ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Rx Path Count = %d", PathInfo.PathGroup[GroupIndex].rx_path_cnt);
        for (int i = 0; i < PathInfo.PathGroup[GroupIndex].rx_path_cnt; i++)
        {
            NR_CALI_RF_V5_PATH_ELE_T* pEle = &PathInfo.PathGroup[GroupIndex].rx_pathinfo[i];

            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Rx Path%d: PathIndex = %d", i, pEle->path_id);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Rx Path%d: bSplitFlag = %d", i, pEle->bSplitFlag);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Rx Path%d: antL = %d", i, pEle->antL);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Rx Path%d: antP = %d", i, pEle->antP);

            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Rx Path%d: Cal En: SmartAntEn = %d", i, pEle->Rx_Cali_En.SmartAntEn);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Rx Path%d: Cal En: AsdivEn = %d", i, pEle->Rx_Cali_En.AsdivEn);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Rx Path%d: Cal En: AgcRefEn = %d", i, pEle->Rx_Cali_En.AgcRefEn);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Rx Path%d: Cal En: AgcCompEn = %d", i, pEle->Rx_Cali_En.AgcCompEn);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Rx Path%d: Cal En: IIp2En = %d", i, pEle->Rx_Cali_En.IIp2En);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Rx Path%d: Cal En: RxIrr = %d", i, pEle->Rx_Cali_En.RxIrr);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Rx Path%d: Cal En: LnaPhaseJumpEn = %d", i, pEle->Rx_Cali_En.LnaPhaseJumpEn);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Rx Path%d: Cal En: RxDcocEn = %d", i, pEle->Rx_Cali_En.RxDcocEn);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Rx Path%d: Cal En: ExLnaType = %d", i, pEle->Rx_Cali_En.ExLnaType);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Rx Path%d: Cal En: ExLnaCompType = %d", i, pEle->Rx_Cali_En.ExLnaCompType);
            ILossFunc::m_pImp->LogFmtStrA(SPLOGLV_INFO, "Rx Path%d: Cal En: FbrxGainWordEn = %d", i, pEle->Rx_Cali_En.FbrxGainWordEn);
        }
    }

    return SP_OK;
}
