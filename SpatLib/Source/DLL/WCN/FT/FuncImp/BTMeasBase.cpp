#include "StdAfx.h"
#include "BTMeasBase.h"
#include "BTApiAT.h"
#include "SimpleAop.h"
//////////////////////////////////////////////////////////////////////////
//
CBTMeasBase::CBTMeasBase(CImpBase* pImpBase, ICBTApi* pBtApi)
    : m_pBTApi(pBtApi)
    , m_pBTParamBand(NULL)
{
    bNeedRetry = FALSE;
    // m_pBTApi = new CBTApiAT(&pImpBase->m_hDUT, m_eType);
    m_pRfTesterBT = pImpBase->m_pRFTester;
    m_stRstData.BER.dAvgValue = 100.0;

    m_pImpBase = pImpBase;
    m_LossHelp.m_pSpatbase = pImpBase;

    LPCWSTR lpProjectName = pImpBase->GetConfigValue(L"Header:ProjectInfo:ProjectName", L"");
    m_Customize.InitRfSwitch(pImpBase->GetAdjParam().nTASK_ID, SP_BT, (CSpatBase*)pImpBase, lpProjectName);
}

CBTMeasBase::~CBTMeasBase(void)
{

}

SPRESULT CBTMeasBase::BTParamSet(BTMeasParamBand* pBTParamBand)
{
    auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
    if (NULL != pBTParamBand)
    {
        m_pBTParamBand = pBTParamBand;
        return SP_OK;
    }
    else
    {
        return SP_E_INVALID_PARAMETER;
    }
}

SPRESULT CBTMeasBase::BTMeasPerform()
{
    auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
    vector<BTMeasParamChan>::iterator itBTChan;
    BTMeasParamChan* pMeasParam = NULL;

    char strInfo[64] = { 0 };
    const int MAX_MEAS_COUNT = m_pImpBase->m_u32MaxFailRetryCount;
    int nRepeatCount = 0;
    SPRESULT res = SP_OK;
    SPRESULT eRslt = SP_OK;

    //Enter bt mode
    //CInstrumentLock rfLock(m_pRfTesterBT);
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterBT->SetNetMode(NM_BT), "Instrument::SetNetMode()");
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterBT->InitDev(TM_NON_SIGNAL, SUB_FT_NST, NULL), "Instrument::InitDev()");
    for (itBTChan = m_pBTParamBand->vecBTFileParamChan.begin(); itBTChan != m_pBTParamBand->vecBTFileParamChan.end(); itBTChan++)
    {
        pMeasParam = &(*itBTChan);

        if (pMeasParam->dwMask == 0)
        {
            continue;
        }

        //Show Band title
        sprintf_s(strInfo, sizeof(strInfo), "%s::CH-%02d:%s"
            , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
            , pMeasParam->nCh
            , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, pMeasParam->ePacketType)
        );

        m_pImpBase->_UiSendMsg(strInfo, LEVEL_ITEM, 1, 1, 1, "", -1, "-");
        //Tx performance meas
        DWORD dwMaskTmp = 0;
        switch (m_pBTParamBand->eMode)
        {
        case BDR:
            dwMaskTmp = pMeasParam->dwMask & (BDR_POWER | BDR_20BW | BDR_ACP | BDR_MC | BDR_ICFR | FREQ_MAXICFR | FREQ_MINICFR | BDR_CFD | FREQ_MAXCFD | FREQ_MINCFD | BT_CW);
            break;
        case EDR:
            dwMaskTmp = pMeasParam->dwMask & (BDR_POWER | EDR_ETP | EDR_DEVM | EDR_ACP | BT_CW);
            break;
        case BLE_EX:
        case BLE:
        case BLE_53:
            dwMaskTmp = pMeasParam->dwMask & (BLE_POWER | BLE_ACP | BLE_MC | BLE_CFD | FREQ_MAXICFR | FREQ_MINICFR | FREQ_MAXCFD | FREQ_MINCFD | BT_CW);
            break;
        default:
            break;
        }

        if (0 != dwMaskTmp)
        {
            //RF_PHY_S2 not do tx
            m_stRstData.Init();
            do
            {
                if (m_pImpBase->_IsUserStop())
                {
                    return SP_E_USER_ABORT;
                }
                CHKRESULT(this->MeasureUplink(pMeasParam, &m_stRstData));
                //Transmit disable
                //m_pBTApi->DUT_RFOn(BT_TX, false);
                //Judge result
                res = JudgeMeasRst(dwMaskTmp, &m_stRstData, pMeasParam->ePath, pMeasParam->ePacketType, pMeasParam->dVsgLvl);
                if (SP_OK == res)
                {
                    break;
                }
            } while (++nRepeatCount < MAX_MEAS_COUNT);
            //Show Tx performance
            CHKRESULT(ShowMeasRst(dwMaskTmp, &m_stRstData, pMeasParam->nCh, pMeasParam->dVsgLvl, pMeasParam->ePath, pMeasParam->ePacketType));
            if (m_pImpBase->m_bFailStop)
            {
                CHKRESULT(res);
                eRslt = res;
            }
            else
            {
                eRslt |= res;
            }
        }

        //RX performance meas
        dwMaskTmp = 0;
        switch (m_pBTParamBand->eMode)
        {
        case BDR:
        case EDR:
            dwMaskTmp = pMeasParam->dwMask & (BDR_BER | BLE_RSSI);
            break;
        case BLE_EX:
        case BLE:
        case BLE_53:
            dwMaskTmp = pMeasParam->dwMask & (BLE_PER | BLE_RSSI);
            break;
        default:
            break;
        }

        if (0 != dwMaskTmp)
        {
            nRepeatCount = 0;
            do
            {
                if (m_pImpBase->_IsUserStop())
                {
                    return SP_E_USER_ABORT;
                }
                m_stRstData.BER.dAvgValue = 100.0;
                m_stRstData.Rssi.dAvgValue = INVALID_NEGATIVE_DOUBLE_VALUE;
                CHKRESULT(this->TestPER(pMeasParam, &m_stRstData));
                res = JudgeMeasRst(dwMaskTmp, &m_stRstData, pMeasParam->ePath, pMeasParam->ePacketType, pMeasParam->dVsgLvl);
                if (SP_OK == res)
                {
                    break;
                }
            } while (++nRepeatCount < MAX_MEAS_COUNT);
            // RX OFF
            //m_pBTApi->DUT_RFOn(BT_RX, FALSE);
            //Show Result
            CHKRESULT(ShowMeasRst(dwMaskTmp, &m_stRstData, pMeasParam->nCh, pMeasParam->dVsgLvl, pMeasParam->ePath, pMeasParam->ePacketType));
            if (m_pImpBase->m_bFailStop)
            {
                CHKRESULT(res);
                eRslt = res;
            }
            else
            {
                eRslt |= res;
            }
        }
    }

    return eRslt;
}


SPRESULT CBTMeasBase::BTMeasLossPerform(BT_LOSS_PARAM BtLossParm)
{
    auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
    vector<BTMeasParamChan>::iterator itBTChan;
    BTMeasParamChan* pMeasParam = NULL;
    m_LossHelp.m_NetMode = NM_BT;

    char strInfo[64] = { 0 };
    const int MAX_MEAS_COUNT = m_pImpBase->m_u32MaxFailRetryCount;
    int nRepeatCount = 0;
    SPRESULT res = SP_OK;
    SPRESULT eRslt = SP_OK;

    //Enter bt mode
    //CInstrumentLock rfLock(m_pRfTesterBT);
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterBT->SetNetMode(NM_BT), "Instrument::SetNetMode()");
    CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pRfTesterBT->InitDev(TM_NON_SIGNAL, SUB_FT_NST, NULL), "Instrument::InitDev()");
    for (itBTChan = m_pBTParamBand->vecBTFileParamChan.begin(); itBTChan != m_pBTParamBand->vecBTFileParamChan.end(); itBTChan++)
    {
        pMeasParam = &(*itBTChan);

        if (pMeasParam->dwMask == 0)
        {
            continue;
        }
        //RF_PHY_S2 only test rx
        if (BLE_EX == m_pBTParamBand->eMode && RF_PHY_S2 == pMeasParam->ePacketType && !IS_BIT_SET(pMeasParam->dwMask, BLE_PER))
        {
            continue;
        }
        else
        {
            //Show Band title
            sprintf_s(strInfo, sizeof(strInfo), "%s::CH-%02d:%s"
                , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
                , pMeasParam->nCh
                , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, pMeasParam->ePacketType)
            );
        }

        m_pImpBase->_UiSendMsg(strInfo, LEVEL_ITEM, 1, 1, 1, "", -1, "-");
        //Tx performance meas
        DWORD dwMaskTmp = 0;
        switch (m_pBTParamBand->eMode)
        {
        case BDR:
            dwMaskTmp = pMeasParam->dwMask & (BDR_POWER | BDR_20BW | BDR_ACP | BDR_MC | BDR_ICFR | FREQ_MAXICFR | FREQ_MINICFR | BDR_CFD | FREQ_MAXCFD | FREQ_MINCFD);
            break;
        case EDR:
            dwMaskTmp = pMeasParam->dwMask & (BDR_POWER | EDR_ETP | EDR_DEVM | EDR_ACP);
            break;
        case BLE_EX:
        case BLE:
            dwMaskTmp = pMeasParam->dwMask & (BLE_POWER | BLE_ACP | BLE_MC | BLE_CFD | FREQ_MAXICFR | FREQ_MINICFR | FREQ_MAXCFD | FREQ_MINCFD);
            break;
        default:
            break;
        }

        if (0 != dwMaskTmp)
        {
            //RF_PHY_S2 not do tx
            m_stRstData.Init();
            double dFreq = 0;
            do
            {
                if (m_pImpBase->_IsUserStop())
                {
                    return SP_E_USER_ABORT;
                }
                //int nRealCh = CwcnUtility::WlanGetTxCh((WIFI_BANDWIDTH_ENUM)m_MeasParam.stTesterParamGroupSub.nCBWType, (WIFI_BANDWIDTH_ENUM)m_MeasParam.stTesterParamGroupSub.nSBWType, m_MeasParam.stChan.nCenChan, m_MeasParam.stChan.nPriChan);
                //m_pBTParamBand->vecBTFileParamChan;

                nRepeatCount++;
                dFreq = CwcnUtility::BT_Ch2MHz(m_pBTParamBand->eMode, pMeasParam->nCh);
                m_LossHelp.SetLoss(m_pBTParamBand->eMode, pMeasParam->nCh, dFreq, RF_IO_TX, BtLossParm.m_dPreLoss, RF_ANT_1st);
                int nRetryCount = 0;
                do
                {
                    if (nRetryCount++ > 20) {
                        //bNeedRetry = FALSE;
                        m_pImpBase->LogFmtStrA(SPLOGLV_ERROR, "BT Over Loss Adjust");
                        return -1;
                    }
                    if (m_pImpBase->_IsUserStop())
                    {
                        return SP_E_USER_ABORT;
                    }

                    CHKRESULT(this->MeasureUplink(pMeasParam, &m_stRstData));
                    double m_StandardValue = 9;
                    switch (m_pBTParamBand->eMode)
                    {
                    case BDR:
                        m_StandardValue = (m_pBTParamBand->stSpec.stBdrSpec.dTxp.low + m_pBTParamBand->stSpec.stBdrSpec.dTxp.upp) / 2;
                        break;
                    case EDR:
                        break;
                    case BLE_EX:
                        break;
                    case BLE:
                        break;
                    default:
                        break;
                    }
                    CHKRESULT(m_LossHelp.AdjustLoss(m_StandardValue, m_stRstData.Power.dAvgValue, RF_IO_TX, bNeedRetry, BtLossParm.m_Tolernece))
                } while (bNeedRetry);

                    //Transmit disable
                    //m_pBTApi->DUT_RFOn(BT_TX, false);
                    //Judge result
                    res = JudgeMeasRst(dwMaskTmp, &m_stRstData, pMeasParam->ePath, pMeasParam->ePacketType, pMeasParam->dVsgLvl);

                    if (SP_OK == res)
                    {
                        break;
                    }

            } while (++nRepeatCount < MAX_MEAS_COUNT);
            //Show Tx performance
            CHKRESULT(ShowMeasRst(dwMaskTmp, &m_stRstData, pMeasParam->nCh, pMeasParam->dVsgLvl, pMeasParam->ePath, pMeasParam->ePacketType));
            m_pImpBase->_UiSendMsg("BT Loss Tx"
                , LEVEL_ITEM | LEVEL_FT
                , -50
                , m_LossHelp.m_Loss
                , 50
                , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
                , pMeasParam->nCh
                , "dBm"
                , "Freq = %.2f"
                , dFreq
            );

            if (m_pImpBase->m_bFailStop)
            {
                CHKRESULT(res);
                eRslt = res;
            }
            else
            {
                eRslt |= res;
            }
        }
    }
    m_LossHelp.UpdateLoss(LOSS_MODE_BT);
    return eRslt;
}

SPRESULT CBTMeasBase::ShowMeasRst(DWORD  dwItemMask, SPBT_RESULT_T* pTestResult, int nChan, double dBSLevel, BT_RFPATH_ENUM ePath, BT_PACKET ePacketType)
{
    auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
    if (pTestResult == NULL)
    {
        CHKRESULT_WITH_NOTIFY_WCN_ITEM(SP_E_INVALID_PARAMETER, "BT::Invalid result!");
    }

    switch (m_pBTParamBand->eMode)
    {
    case BDR:
        CHKRESULT(_ShowMeasRstBdr(dwItemMask, pTestResult, nChan, dBSLevel, ePath, ePacketType));
        break;
    case EDR:
        CHKRESULT(_ShowMeasRstEdr(dwItemMask, pTestResult, nChan, dBSLevel, ePath, ePacketType));
        break;
    case BLE_EX:
        CHKRESULT(_ShowMeasRstBleEx(dwItemMask, pTestResult, nChan, dBSLevel, ePath, ePacketType));
        break;
    case BLE:
        CHKRESULT(_ShowMeasRstBle(dwItemMask, pTestResult, nChan, dBSLevel, ePath, ePacketType));
        break;
    default:
        break;
    }

    return SP_OK;
}

SPRESULT CBTMeasBase::_ShowMeasRstBdr(DWORD  dwItemMask, SPBT_RESULT_T* pTestResult, int nChan, double dBSLevel, BT_RFPATH_ENUM ePath, BT_PACKET ePacketType)
{
    auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
    //打印频率
    double dFreq = CwcnUtility::BT_Ch2MHz(BDR, nChan);
    if (IS_BIT_SET(dwItemMask, BDR_POWER))
    {
        double dTxpAvglow = 0.0;
        double dTxpAvgupp = 0.0;
        if (ANT_SINGLE == ePath)
        {
            dTxpAvglow = m_pBTParamBand->stSpec.stBdrSpec.dTxp.low;
            dTxpAvgupp = m_pBTParamBand->stSpec.stBdrSpec.dTxp.upp;
        }
        else
        {
            dTxpAvglow = m_pBTParamBand->stSpec.stBdrSpec.dTxpShared.low;
            dTxpAvgupp = m_pBTParamBand->stSpec.stBdrSpec.dTxpShared.upp;
        }

        m_pImpBase->_UiSendMsg("Output Power-Pavg"
            , LEVEL_ITEM | LEVEL_FT
            , dTxpAvglow
            , pTestResult->Power.dAvgValue
            , dTxpAvgupp
            , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
            , nChan
            , "dBm"
            , "Frequency:%0.2f;%s;%s"
            , dFreq
            , CwcnUtility::BT_ANT_NAME[ePath]
            , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
        );
    }

    if (IS_BIT_SET(dwItemMask, BT_CW))
    {
        double dTxpAvglow = 0.0;
        double dTxpAvgupp = 0.0;
        if (ANT_SINGLE == ePath)
        {
            dTxpAvglow = m_pBTParamBand->stSpec.stBdrSpec.dTxp.low;
            dTxpAvgupp = m_pBTParamBand->stSpec.stBdrSpec.dTxp.upp;
        }
        else
        {
            dTxpAvglow = m_pBTParamBand->stSpec.stBdrSpec.dTxpShared.low;
            dTxpAvgupp = m_pBTParamBand->stSpec.stBdrSpec.dTxpShared.upp;
        }

        m_pImpBase->_UiSendMsg("CW Output Power-Pavg"
            , LEVEL_ITEM | LEVEL_FT
            , dTxpAvglow
            , pTestResult->Power.dAvgValue
            , dTxpAvgupp
            , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
            , nChan
            , "dBm"
            , "Frequency:%0.2f;%s;%s"
            , dFreq
            , CwcnUtility::BT_ANT_NAME[ePath]
            , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
        );
    }

    if (IS_BIT_SET(dwItemMask, BDR_20BW))
    {
        m_pImpBase->_UiSendMsg("20dB-Bandwidth"
            , LEVEL_ITEM | LEVEL_FT
            , m_pBTParamBand->stSpec.stBdrSpec.dTsbd.low
            , pTestResult->BW20dB.dAvgValue
            , m_pBTParamBand->stSpec.stBdrSpec.dTsbd.upp
            , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
            , nChan
            , "KHz"
            , "Frequency:%0.2f;%s;%s"
            , dFreq
            , CwcnUtility::BT_ANT_NAME[ePath]
            , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
        );
    }

    if (IS_BIT_SET(dwItemMask, BDR_ACP))
    {
        m_pImpBase->_UiSendMsg("Adjacent Channel Power"
            , LEVEL_ITEM | LEVEL_FT
            , TRUE
            , pTestResult->ACP.indicator
            , TRUE
            , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
            , nChan
            , "Unit"
            , "Frequency:%0.2f;%s;%s"
            , dFreq
            , CwcnUtility::BT_ANT_NAME[ePath]
            , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
        );
    }

    if (IS_BIT_SET(dwItemMask, BDR_ICFR))
    {
        m_pImpBase->_UiSendMsg("Initial Carrier Frequency Tolerance"
            , LEVEL_ITEM | LEVEL_FT
            , m_pBTParamBand->stSpec.stBdrSpec.dIcft.low
            , pTestResult->InitalCarrierFreq.dAvgValue
            , m_pBTParamBand->stSpec.stBdrSpec.dIcft.upp
            , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
            , nChan
            , "-"
            , "Frequency:%0.2f;%s;%s"
            , dFreq
            , CwcnUtility::BT_ANT_NAME[ePath]
            , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
        );
    }
    //MAX ICFR
    if (IS_BIT_SET(dwItemMask, FREQ_MAXICFR))
    {
        m_pImpBase->_UiSendMsg("BDR MAX Initial Carrier Frequency Tolerance"
            , LEVEL_ITEM | LEVEL_FT
            , m_pBTParamBand->stSpec.stBdrSpec.dIcftMax.low
            , pTestResult->InitalCarrierFreq.dMaxValue
            , m_pBTParamBand->stSpec.stBdrSpec.dIcftMax.upp
            , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
            , nChan
            , "-"
            , "Frequency:%0.2f;%s;%s"
            , dFreq
            , CwcnUtility::BT_ANT_NAME[ePath]
            , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
        );
    }
    //MIN ICFR
    if (IS_BIT_SET(dwItemMask, FREQ_MINICFR))
    {
        m_pImpBase->_UiSendMsg("BDR MIN Initial Carrier Frequency Tolerance"
            , LEVEL_ITEM | LEVEL_FT
            , m_pBTParamBand->stSpec.stBdrSpec.dIcftMin.low
            , pTestResult->InitalCarrierFreq.dMinValue
            , m_pBTParamBand->stSpec.stBdrSpec.dIcftMin.upp
            , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
            , nChan
            , "-"
            , "Frequency:%0.2f;%s;%s"
            , dFreq
            , CwcnUtility::BT_ANT_NAME[ePath]
            , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
        );
    }

    if (IS_BIT_SET(dwItemMask, BDR_CFD))
    {
        m_pImpBase->_UiSendMsg("Carrier Freq Drift"
            , LEVEL_ITEM | LEVEL_FT
            , m_pBTParamBand->stSpec.stBdrSpec.dCfd.low
            , pTestResult->CarrierFreqDrift.dAvgValue
            , m_pBTParamBand->stSpec.stBdrSpec.dCfd.upp
            , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
            , nChan
            , "-"
            , "Frequency:%0.2f;%s;%s"
            , dFreq
            , CwcnUtility::BT_ANT_NAME[ePath]
            , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
        );
    }
    //MAX CFD
    if (IS_BIT_SET(dwItemMask, FREQ_MAXCFD))
    {
        m_pImpBase->_UiSendMsg("BDR MAX Carrier Freq Drift"
            , LEVEL_ITEM | LEVEL_FT
            , m_pBTParamBand->stSpec.stBdrSpec.dCfdMax.low
            , pTestResult->CarrierFreqDrift.dMaxValue
            , m_pBTParamBand->stSpec.stBdrSpec.dCfdMax.upp
            , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
            , nChan
            , "-"
            , "Frequency:%0.2f;%s;%s"
            , dFreq
            , CwcnUtility::BT_ANT_NAME[ePath]
            , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
        );
    }
    //MIN CFD
    if (IS_BIT_SET(dwItemMask, FREQ_MINCFD))
    {
        m_pImpBase->_UiSendMsg("BDR MIN Carrier Freq Drift"
            , LEVEL_ITEM | LEVEL_FT
            , m_pBTParamBand->stSpec.stBdrSpec.dCfdMin.low
            , pTestResult->CarrierFreqDrift.dMinValue
            , m_pBTParamBand->stSpec.stBdrSpec.dCfdMin.upp
            , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
            , nChan
            , "-"
            , "Frequency:%0.2f;%s;%s"
            , dFreq
            , CwcnUtility::BT_ANT_NAME[ePath]
            , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
        );
    }

    if (IS_BIT_SET(dwItemMask, BDR_MC))
    {
        m_pImpBase->_UiSendMsg("f1avg"
            , LEVEL_ITEM | LEVEL_FT
            , m_pBTParamBand->stSpec.stBdrSpec.dModF1avg.low
            , pTestResult->Modulation.f1avg
            , m_pBTParamBand->stSpec.stBdrSpec.dModF1avg.upp
            , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
            , nChan
            , "-"
            , "Frequency:%0.2f;%s;%s"
            , dFreq
            , CwcnUtility::BT_ANT_NAME[ePath]
            , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
        );

        m_pImpBase->_UiSendMsg("f2max"
            , LEVEL_ITEM | LEVEL_FT
            , m_pBTParamBand->stSpec.stBdrSpec.dModF2max.low
            , pTestResult->Modulation.f2max
            , m_pBTParamBand->stSpec.stBdrSpec.dModF2max.upp
            , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
            , nChan
            , "-"
            , "Frequency:%0.2f;%s;%s"
            , dFreq
            , CwcnUtility::BT_ANT_NAME[ePath]
            , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
        );

        m_pImpBase->_UiSendMsg("f2avg"
            , LEVEL_ITEM | LEVEL_FT
            , NOLOWLMT
            , pTestResult->Modulation.f2avg
            , NOUPPLMT
            , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
            , nChan
            , "-"
            , "Frequency:%0.2f;%s;%s"
            , dFreq
            , CwcnUtility::BT_ANT_NAME[ePath]
            , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
        );

        m_pImpBase->_UiSendMsg("f2avg / f1avg"
            , LEVEL_ITEM | LEVEL_FT
            , m_pBTParamBand->stSpec.stBdrSpec.dModRatio
            , pTestResult->Modulation.f2f1avg_rate
            , 999.0
            , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
            , nChan
            , "-"
            , "Frequency:%0.2f;%s;%s"
            , dFreq
            , CwcnUtility::BT_ANT_NAME[ePath]
            , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
        );
    }

    if (IS_BIT_SET(dwItemMask, BDR_BER))
    {
        m_pImpBase->_UiSendMsg("SensitivityTest"
            , LEVEL_ITEM | LEVEL_FT
            , 0.0
            , pTestResult->BER.dAvgValue
            , m_pBTParamBand->stSpec.stBdrSpec.dBer
            , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
            , nChan
            , "-"
            , "Frequency:%0.2f;%s;%s;BSLevel:%.2f"
            , dFreq
            , CwcnUtility::BT_ANT_NAME[ePath]
            , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
            , dBSLevel
        );
    }

    if (IS_BIT_SET(dwItemMask, BLE_RSSI))
    {
        m_pImpBase->_UiSendMsg("RSSITest"
            , LEVEL_ITEM | LEVEL_FT
            , dBSLevel + m_pBTParamBand->stSpec.stBdrSpec.dRssi.low
            , pTestResult->Rssi.dAvgValue
            , dBSLevel + m_pBTParamBand->stSpec.stBdrSpec.dRssi.upp
            , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
            , nChan
            , "-"
            , "Frequency:%0.2f;%s;%s;BSLevel:%.2f"
            , dFreq
            , CwcnUtility::BT_ANT_NAME[ePath]
            , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
            , dBSLevel
        );
    }

    return SP_OK;
}

SPRESULT CBTMeasBase::_ShowMeasRstEdr(DWORD  dwItemMask, SPBT_RESULT_T* pTestResult, int nChan, double dBSLevel, BT_RFPATH_ENUM ePath, BT_PACKET ePacketType)
{
    auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
    //打印频率
    double dFreq = CwcnUtility::BT_Ch2MHz(EDR, nChan);
    if (IS_BIT_SET(dwItemMask, BDR_POWER))
    {
        double dTxpAvglow = 0.0;
        double dTxpAvgupp = 0.0;
        if (ANT_SINGLE == ePath)
        {
            dTxpAvglow = m_pBTParamBand->stSpec.stEdrSpec.dTxp.low;
            dTxpAvgupp = m_pBTParamBand->stSpec.stEdrSpec.dTxp.upp;
        }
        else
        {
            dTxpAvglow = m_pBTParamBand->stSpec.stEdrSpec.dTxpShared.low;
            dTxpAvgupp = m_pBTParamBand->stSpec.stEdrSpec.dTxpShared.upp;
        }

        m_pImpBase->_UiSendMsg("Output Power-Pavg"
            , LEVEL_ITEM | LEVEL_FT
            , dTxpAvglow
            , pTestResult->Power.dAvgValue
            , dTxpAvgupp
            , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
            , nChan
            , "dBm"
            , "Frequency:%0.2f;%s;%s"
            , dFreq
            , CwcnUtility::BT_ANT_NAME[ePath]
            , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
        );
    }

    if (IS_BIT_SET(dwItemMask, BT_CW))
    {
        double dTxpAvglow = 0.0;
        double dTxpAvgupp = 0.0;
        if (ANT_SINGLE == ePath)
        {
            dTxpAvglow = m_pBTParamBand->stSpec.stEdrSpec.dTxp.low;
            dTxpAvgupp = m_pBTParamBand->stSpec.stEdrSpec.dTxp.upp;
        }
        else
        {
            dTxpAvglow = m_pBTParamBand->stSpec.stEdrSpec.dTxpShared.low;
            dTxpAvgupp = m_pBTParamBand->stSpec.stEdrSpec.dTxpShared.upp;
        }

        m_pImpBase->_UiSendMsg("CW Output Power-Pavg"
            , LEVEL_ITEM | LEVEL_FT
            , dTxpAvglow
            , pTestResult->Power.dAvgValue
            , dTxpAvgupp
            , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
            , nChan
            , "dBm"
            , "Frequency:%0.2f;%s;%s"
            , dFreq
            , CwcnUtility::BT_ANT_NAME[ePath]
            , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
        );
    }

    if (IS_BIT_SET(dwItemMask, EDR_ETP))
    {
        m_pImpBase->_UiSendMsg("Relative Tranmsmit Power"
            , LEVEL_ITEM | LEVEL_FT
            , m_pBTParamBand->stSpec.stEdrSpec.dRTxp.low
            , pTestResult->RelativePower.dAvgValue
            , m_pBTParamBand->stSpec.stEdrSpec.dRTxp.upp
            , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
            , nChan
            , "dB"
            , "Frequency:%0.2f;%s;%s"
            , dFreq
            , CwcnUtility::BT_ANT_NAME[ePath]
            , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
        );
    }

    if (IS_BIT_SET(dwItemMask, EDR_DEVM))
    {
        m_pImpBase->_UiSendMsg("wi"
            , LEVEL_ITEM | LEVEL_FT
            , m_pBTParamBand->stSpec.stEdrSpec.dWi.low
            , pTestResult->DEVM.wi
            , m_pBTParamBand->stSpec.stEdrSpec.dWi.upp
            , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
            , nChan
            , "Unit"
            , "Frequency:%0.2f;%s;%s"
            , dFreq
            , CwcnUtility::BT_ANT_NAME[ePath]
            , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
        );

        m_pImpBase->_UiSendMsg("w0"
            , LEVEL_ITEM | LEVEL_FT
            , m_pBTParamBand->stSpec.stEdrSpec.dW0.low
            , pTestResult->DEVM.w0
            , m_pBTParamBand->stSpec.stEdrSpec.dW0.upp
            , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
            , nChan
            , "Unit"
            , "Frequency:%0.2f;%s;%s"
            , dFreq
            , CwcnUtility::BT_ANT_NAME[ePath]
            , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
        );

        m_pImpBase->_UiSendMsg("wi+w0"
            , LEVEL_ITEM | LEVEL_FT
            , m_pBTParamBand->stSpec.stEdrSpec.dWi.low
            , pTestResult->DEVM.wi0
            , m_pBTParamBand->stSpec.stEdrSpec.dWi.upp
            , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
            , nChan
            , "Unit"
            , "Frequency:%0.2f;%s;%s"
            , dFreq
            , CwcnUtility::BT_ANT_NAME[ePath]
            , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
        );

        double dRmsEvmLow = m_pBTParamBand->stSpec.stEdrSpec.dRmsEvm_2DHX.low;
        double dRmsEvmUpp = m_pBTParamBand->stSpec.stEdrSpec.dRmsEvm_2DHX.upp;
        double dPeakEvmLow = m_pBTParamBand->stSpec.stEdrSpec.dPeakEvm_2DHX.low;
        double dPeakEvmUpp = m_pBTParamBand->stSpec.stEdrSpec.dPeakEvm_2DHX.upp;
        double dP99EvmLow = m_pBTParamBand->stSpec.stEdrSpec.dP99Evm_2DHX.low;
        double dP99EvmUpp = m_pBTParamBand->stSpec.stEdrSpec.dP99Evm_2DHX.upp;
        if (ePacketType >= EDR_3DH1)
        {
            dRmsEvmLow = m_pBTParamBand->stSpec.stEdrSpec.dRmsEvm_3DHX.low;
            dRmsEvmUpp = m_pBTParamBand->stSpec.stEdrSpec.dRmsEvm_3DHX.upp;
            dPeakEvmLow = m_pBTParamBand->stSpec.stEdrSpec.dPeakEvm_3DHX.low;
            dPeakEvmUpp = m_pBTParamBand->stSpec.stEdrSpec.dPeakEvm_3DHX.upp;
            dP99EvmLow = m_pBTParamBand->stSpec.stEdrSpec.dP99Evm_3DHX.low;
            dP99EvmUpp = m_pBTParamBand->stSpec.stEdrSpec.dP99Evm_3DHX.upp;
        }
        m_pImpBase->_UiSendMsg("RMS Evm"
            , LEVEL_ITEM | LEVEL_FT
            , dRmsEvmLow
            , pTestResult->DEVM.avgEVM
            , dRmsEvmUpp
            , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
            , nChan
            , "Unit"
            , "Frequency:%0.2f;%s;%s"
            , dFreq
            , CwcnUtility::BT_ANT_NAME[ePath]
            , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
        );

        m_pImpBase->_UiSendMsg("Peak Evm"
            , LEVEL_ITEM | LEVEL_FT
            , dPeakEvmLow
            , pTestResult->DEVM.maxEVM
            , dPeakEvmUpp
            , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
            , nChan
            , "Unit"
            , "Frequency:%0.2f;%s;%s"
            , dFreq
            , CwcnUtility::BT_ANT_NAME[ePath]
            , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
        );
        //%99 EVM
        m_pImpBase->_UiSendMsg("P99% Evm"
            , LEVEL_ITEM | LEVEL_FT
            , dP99EvmLow
            , pTestResult->DEVM.p99EVM
            , dP99EvmUpp
            , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
            , nChan
            , "Unit"
            , "Frequency:%0.2f;%s;%s"
            , dFreq
            , CwcnUtility::BT_ANT_NAME[ePath]
            , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
        );
        //
    }

    if (IS_BIT_SET(dwItemMask, EDR_ACP))
    {
        m_pImpBase->_UiSendMsg("In-Band Spurious Emissions"
            , LEVEL_ITEM | LEVEL_FT
            , TRUE
            , pTestResult->ACP.indicator
            , TRUE
            , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
            , nChan
            , "-"
            , "Frequency:%0.2f;%s;%s"
            , dFreq
            , CwcnUtility::BT_ANT_NAME[ePath]
            , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
        );
    }

    if (IS_BIT_SET(dwItemMask, BDR_BER))
    {
        m_pImpBase->_UiSendMsg("SensitivityTest"
            , LEVEL_ITEM | LEVEL_FT
            , 0.0
            , pTestResult->BER.dAvgValue
            , m_pBTParamBand->stSpec.stEdrSpec.dBer
            , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
            , nChan
            , "-"
            , "Frequency:%0.2f;%s;%s;BSLevel:%.2f"
            , dFreq
            , CwcnUtility::BT_ANT_NAME[ePath]
            , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
            , dBSLevel
        );
    }

    if (IS_BIT_SET(dwItemMask, BLE_RSSI))
    {
        m_pImpBase->_UiSendMsg("RSSITest"
            , LEVEL_ITEM | LEVEL_FT
            , dBSLevel + m_pBTParamBand->stSpec.stEdrSpec.dRssi.low
            , pTestResult->Rssi.dAvgValue
            , dBSLevel + m_pBTParamBand->stSpec.stEdrSpec.dRssi.upp
            , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
            , nChan
            , "-"
            , "Frequency:%0.2f;%s;%s;BSLevel:%.2f"
            , dFreq
            , CwcnUtility::BT_ANT_NAME[ePath]
            , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
            , dBSLevel
        );
    }

    return SP_OK;
}

SPRESULT CBTMeasBase::_ShowMeasRstBle(DWORD  dwItemMask, SPBT_RESULT_T* pTestResult, int nChan, double dBSLevel, BT_RFPATH_ENUM ePath, BT_PACKET /*ePacketType*/)
{
    auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
    //打印频率
    double dFreq = CwcnUtility::BT_Ch2MHz(BLE, nChan);
    if (IS_BIT_SET(dwItemMask, BLE_POWER))
    {
        double dTxpAvglow = 0.0;
        double dTxpAvgupp = 0.0;
        if (ANT_SINGLE == ePath)
        {
            dTxpAvglow = m_pBTParamBand->stSpec.stBleSpec.dTxpAvg.low;
            dTxpAvgupp = m_pBTParamBand->stSpec.stBleSpec.dTxpAvg.upp;
        }
        else
        {
            dTxpAvglow = m_pBTParamBand->stSpec.stBleSpec.dTxpAvgShared.low;
            dTxpAvgupp = m_pBTParamBand->stSpec.stBleSpec.dTxpAvgShared.upp;
        }

        m_pImpBase->_UiSendMsg("Output Power-Pavg"
            , LEVEL_ITEM | LEVEL_FT
            , dTxpAvglow
            , pTestResult->Power.dAvgValue
            , dTxpAvgupp
            , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
            , nChan
            , "dBm"
            , "Frequency:%0.2f;%s"
            , dFreq
            , CwcnUtility::BT_ANT_NAME[ePath]
        );


        m_pImpBase->_UiSendMsg("Output Power-Ppk-Pavg"
            , LEVEL_ITEM | LEVEL_FT
            , m_pBTParamBand->stSpec.stBleSpec.dTxpPeak.low
            , pTestResult->Power.dMaxValue - pTestResult->Power.dAvgValue
            , m_pBTParamBand->stSpec.stBleSpec.dTxpPeak.upp
            , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
            , nChan
            , "dBm"
            , "Frequency:%0.2f;%s"
            , dFreq
            , CwcnUtility::BT_ANT_NAME[ePath]
        );
    }

	if (IS_BIT_SET(dwItemMask, BT_CW))
	{
		double dTxpAvglow = 0.0;
		double dTxpAvgupp = 0.0;
		if (ANT_SINGLE == ePath)
		{
			dTxpAvglow = m_pBTParamBand->stSpec.stBleSpec.dTxpAvg.low;
			dTxpAvgupp = m_pBTParamBand->stSpec.stBleSpec.dTxpAvg.upp;
		}
		else
		{
			dTxpAvglow = m_pBTParamBand->stSpec.stBleSpec.dTxpAvgShared.low;
			dTxpAvgupp = m_pBTParamBand->stSpec.stBleSpec.dTxpAvgShared.upp;
		}

		m_pImpBase->_UiSendMsg("CW Output Power-Pavg"
			, LEVEL_ITEM | LEVEL_FT
			, dTxpAvglow
			, pTestResult->Power.dAvgValue
			, dTxpAvgupp
			, CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
			, nChan
			, "dBm"
			, "Frequency:%0.2f;%s"
			, dFreq
			, CwcnUtility::BT_ANT_NAME[ePath]
		);
	}

        if (IS_BIT_SET(dwItemMask, BLE_ACP))
        {
            m_pImpBase->_UiSendMsg("In-Band Spurious Emissons"
                , LEVEL_ITEM | LEVEL_FT
                , TRUE
                , pTestResult->ACP.indicator
                , TRUE
                , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
                , nChan
                , "Unit"
                , "Frequency:%0.2f;%s"
                , dFreq
                , CwcnUtility::BT_ANT_NAME[ePath]
            );
        }

        if (IS_BIT_SET(dwItemMask, BLE_MC))
        {
            m_pImpBase->_UiSendMsg("f1avg"
                , LEVEL_ITEM | LEVEL_FT
                , m_pBTParamBand->stSpec.stBleSpec.dF1Avg.low
                , pTestResult->Modulation.f1avg
                , m_pBTParamBand->stSpec.stBleSpec.dF1Avg.upp
                , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
                , nChan
                , "-"
                , "Frequency:%0.2f;%s"
                , dFreq
                , CwcnUtility::BT_ANT_NAME[ePath]
            );

            m_pImpBase->_UiSendMsg("f2max"
                , LEVEL_ITEM | LEVEL_FT
                , m_pBTParamBand->stSpec.stBleSpec.dF2Max
                , pTestResult->Modulation.f2max
                , 999.0
                , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
                , nChan
                , "-"
                , "Frequency:%0.2f;%s"
                , dFreq
                , CwcnUtility::BT_ANT_NAME[ePath]
            );

            m_pImpBase->_UiSendMsg("f2avg"
                , LEVEL_ITEM | LEVEL_FT
                , NOLOWLMT
                , pTestResult->Modulation.f2avg
                , NOUPPLMT
                , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
                , nChan
                , "-"
                , "Frequency:%0.2f;%s"
                , dFreq
                , CwcnUtility::BT_ANT_NAME[ePath]
            );

            m_pImpBase->_UiSendMsg("f2avg / f1avg"
                , LEVEL_ITEM | LEVEL_FT
                , m_pBTParamBand->stSpec.stBleSpec.dRatio
                , pTestResult->Modulation.f2f1avg_rate
                , 999.0
                , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
                , nChan
                , "-"
                , "Frequency:%0.2f;%s"
                , dFreq
                , CwcnUtility::BT_ANT_NAME[ePath]
            );
        }

        if (IS_BIT_SET(dwItemMask, BLE_CFD))
        {
            m_pImpBase->_UiSendMsg("Fn"
                , LEVEL_ITEM | LEVEL_FT
                , m_pBTParamBand->stSpec.stBleSpec.dFnMax[RF_PHY_1M].low
                , pTestResult->CarrierFreqDrift_BLE.fn
                , m_pBTParamBand->stSpec.stBleSpec.dFnMax[RF_PHY_1M].upp
                , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
                , nChan
                , "-"
                , "Frequency:%0.2f;%s"
                , dFreq
                , CwcnUtility::BT_ANT_NAME[ePath]
            );

            m_pImpBase->_UiSendMsg("F0-Fn"
                , LEVEL_ITEM | LEVEL_FT
                , m_pBTParamBand->stSpec.stBleSpec.dF0FnMax[RF_PHY_1M].low
                , pTestResult->CarrierFreqDrift_BLE.f0fn
                , m_pBTParamBand->stSpec.stBleSpec.dF0FnMax[RF_PHY_1M].upp
                , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
                , nChan
                , "-"
                , "Frequency:%0.2f;%s"
                , dFreq
                , CwcnUtility::BT_ANT_NAME[ePath]
            );

            m_pImpBase->_UiSendMsg("F1-F0"
                , LEVEL_ITEM | LEVEL_FT
                , m_pBTParamBand->stSpec.stBleSpec.dF1F0[RF_PHY_1M].low
                , pTestResult->CarrierFreqDrift_BLE.f1f0
                , m_pBTParamBand->stSpec.stBleSpec.dF1F0[RF_PHY_1M].upp
                , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
                , nChan
                , "-"
                , "Frequency:%0.2f;%s"
                , dFreq
                , CwcnUtility::BT_ANT_NAME[ePath]
            );

            m_pImpBase->_UiSendMsg("Fn-F(n-5)"
                , LEVEL_ITEM | LEVEL_FT
                , m_pBTParamBand->stSpec.stBleSpec.dFnFn5[RF_PHY_1M].low
                , pTestResult->CarrierFreqDrift_BLE.fnfn5
                , m_pBTParamBand->stSpec.stBleSpec.dFnFn5[RF_PHY_1M].upp
                , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
                , nChan
                , "-"
                , "Frequency:%0.2f;%s"
                , dFreq
                , CwcnUtility::BT_ANT_NAME[ePath]
            );
        }

        if (IS_BIT_SET(dwItemMask, BLE_PER))
        {
            m_pImpBase->_UiSendMsg("Sensitivity - PER"
                , LEVEL_ITEM | LEVEL_FT
                , 0.0
                , pTestResult->BER.dAvgValue
                , m_pBTParamBand->stSpec.stBleSpec.dBer
                , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
                , nChan
                , "-"
                , "Frequency:%0.2f;%s;BSLevel:%.2f"
                , dFreq
                , CwcnUtility::BT_ANT_NAME[ePath]
                , dBSLevel
            );
        }

        if (IS_BIT_SET(dwItemMask, BLE_RSSI))
        {
            m_pImpBase->_UiSendMsg("RSSITest"
                , LEVEL_ITEM | LEVEL_FT
                , dBSLevel + m_pBTParamBand->stSpec.stBleSpec.dRssi.low
                , pTestResult->Rssi.dAvgValue
                , dBSLevel + m_pBTParamBand->stSpec.stBleSpec.dRssi.upp
                , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
                , nChan
                , "-"
                , "Frequency:%0.2f;%s;BSLevel:%.2f"
                , dFreq
                , CwcnUtility::BT_ANT_NAME[ePath]
                , dBSLevel
            );
        }

        return SP_OK;
    }

    SPRESULT CBTMeasBase::_ShowMeasRstBleEx(DWORD  dwItemMask, SPBT_RESULT_T * pTestResult, int nChan, double dBSLevel, BT_RFPATH_ENUM ePath, BT_PACKET ePacketType)
    {
        auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
        //打印频率
        double dFreq = CwcnUtility::BT_Ch2MHz(BLE_EX, nChan);
        if (IS_BIT_SET(dwItemMask, BLE_POWER))
        {
            double dTxpAvglow = 0.0;
            double dTxpAvgupp = 0.0;
            if (ANT_SINGLE == ePath)
            {
                dTxpAvglow = m_pBTParamBand->stSpec.stBleExSpec.dTxpAvg.low;
                dTxpAvgupp = m_pBTParamBand->stSpec.stBleExSpec.dTxpAvg.upp;
            }
            else
            {
                dTxpAvglow = m_pBTParamBand->stSpec.stBleExSpec.dTxpAvgShared.low;
                dTxpAvgupp = m_pBTParamBand->stSpec.stBleExSpec.dTxpAvgShared.upp;
            }

            m_pImpBase->_UiSendMsg("Output Power-Pavg"
                , LEVEL_ITEM | LEVEL_FT
                , dTxpAvglow
                , pTestResult->Power.dAvgValue
                , dTxpAvgupp
                , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
                , nChan
                , "dBm"
                , "Frequency:%0.2f;%s;%s"
                , dFreq
                , CwcnUtility::BT_ANT_NAME[ePath]
                , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
            );


            m_pImpBase->_UiSendMsg("Output Power-Ppk-Pavg"
                , LEVEL_ITEM | LEVEL_FT
                , m_pBTParamBand->stSpec.stBleExSpec.dTxpPeak.low
                , pTestResult->Power.dMaxValue - pTestResult->Power.dAvgValue
                , m_pBTParamBand->stSpec.stBleExSpec.dTxpPeak.upp
                , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
                , nChan
                , "dBm"
                , "Frequency:%0.2f;%s;%s"
                , dFreq
                , CwcnUtility::BT_ANT_NAME[ePath]
                , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
            );
        }

        if (IS_BIT_SET(dwItemMask, BT_CW))
        {
            double dTxpAvglow = 0.0;
            double dTxpAvgupp = 0.0;
            if (ANT_SINGLE == ePath)
            {
                dTxpAvglow = m_pBTParamBand->stSpec.stBleExSpec.dTxpAvg.low;
                dTxpAvgupp = m_pBTParamBand->stSpec.stBleExSpec.dTxpAvg.upp;
            }
            else
            {
                dTxpAvglow = m_pBTParamBand->stSpec.stBleExSpec.dTxpAvgShared.low;
                dTxpAvgupp = m_pBTParamBand->stSpec.stBleExSpec.dTxpAvgShared.upp;
            }

            m_pImpBase->_UiSendMsg("CW Output Power-Pavg"
                , LEVEL_ITEM | LEVEL_FT
                , dTxpAvglow
                , pTestResult->Power.dAvgValue
                , dTxpAvgupp
                , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
                , nChan
                , "dBm"
                , "Frequency:%0.2f;%s;%s"
                , dFreq
                , CwcnUtility::BT_ANT_NAME[ePath]
                , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
            );
        }

        if (IS_BIT_SET(dwItemMask, BLE_ACP) && RF_PHY_S2 != ePacketType)
        {
            m_pImpBase->_UiSendMsg("In-Band Spurious Emissons"
                , LEVEL_ITEM | LEVEL_FT
                , TRUE
                , pTestResult->ACP.indicator
                , TRUE
                , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
                , nChan
                , "Unit"
                , "Frequency:%0.2f;%s;%s"
                , dFreq
                , CwcnUtility::BT_ANT_NAME[ePath]
                , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
            );
        }

        if (IS_BIT_SET(dwItemMask, BLE_MC))
        {
            double dF1AvgLowLimit = m_pBTParamBand->stSpec.stBleExSpec.dF1Avg.low;
            double dF1AvgUppLimit = m_pBTParamBand->stSpec.stBleExSpec.dF1Avg.upp;
            if (ePacketType == RF_PHY_2M)
            {
                dF1AvgLowLimit = dF1AvgLowLimit * 2.0;
                dF1AvgUppLimit = dF1AvgUppLimit * 2.0;
            }
            if (RF_PHY_1M == ePacketType || RF_PHY_S8 == ePacketType || RF_PHY_2M == ePacketType)
            {
                m_pImpBase->_UiSendMsg("f1avg"
                    , LEVEL_ITEM | LEVEL_FT
                    , dF1AvgLowLimit
                    , pTestResult->Modulation.f1avg
                    , dF1AvgUppLimit
                    , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
                    , nChan
                    , "-"
                    , "Frequency:%0.2f;%s;%s"
                    , dFreq
                    , CwcnUtility::BT_ANT_NAME[ePath]
                    , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
                );
            }

            if (RF_PHY_1M == ePacketType || RF_PHY_2M == ePacketType)
            {
                m_pImpBase->_UiSendMsg("f2avg"
                    , LEVEL_ITEM | LEVEL_FT
                    , NOLOWLMT
                    , pTestResult->Modulation.f2avg
                    , NOUPPLMT
                    , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
                    , nChan
                    , "-"
                    , "Frequency:%0.2f;%s;%s"
                    , dFreq
                    , CwcnUtility::BT_ANT_NAME[ePath]
                    , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
                );

                m_pImpBase->_UiSendMsg("f2avg / f1avg"
                    , LEVEL_ITEM | LEVEL_FT
                    , m_pBTParamBand->stSpec.stBleExSpec.dRatio
                    , pTestResult->Modulation.f2f1avg_rate
                    , 999.0
                    , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
                    , nChan
                    , "-"
                    , "Frequency:%0.2f;%s;%s"
                    , dFreq
                    , CwcnUtility::BT_ANT_NAME[ePath]
                    , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
                );
                double dF2MaxLowLimit = m_pBTParamBand->stSpec.stBleExSpec.dF2Max;
                if (ePacketType == RF_PHY_2M)
                {
                    dF2MaxLowLimit = dF2MaxLowLimit * 2.0;
                }

                m_pImpBase->_UiSendMsg("f2max"
                    , LEVEL_ITEM | LEVEL_FT
                    , dF2MaxLowLimit
                    , pTestResult->Modulation.f2max
                    , 999.0
                    , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
                    , nChan
                    , "-"
                    , "Frequency:%0.2f;%s;%s"
                    , dFreq
                    , CwcnUtility::BT_ANT_NAME[ePath]
                    , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
                );

            }
        }

        if (IS_BIT_SET(dwItemMask, BLE_CFD) && RF_PHY_S2 != ePacketType)
        {
            m_pImpBase->_UiSendMsg("Fn"
                , LEVEL_ITEM | LEVEL_FT
                , m_pBTParamBand->stSpec.stBleExSpec.dFnMax[ePacketType].low
                , pTestResult->CarrierFreqDrift_BLE.fn
                , m_pBTParamBand->stSpec.stBleExSpec.dFnMax[ePacketType].upp
                , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
                , nChan
                , "-"
                , "Frequency:%0.2f;%s;%s"
                , dFreq
                , CwcnUtility::BT_ANT_NAME[ePath]
                , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
            );

            m_pImpBase->_UiSendMsg("F0-Fn"
                , LEVEL_ITEM | LEVEL_FT
                , m_pBTParamBand->stSpec.stBleExSpec.dF0FnMax[ePacketType].low
                , pTestResult->CarrierFreqDrift_BLE.f0fn
                , m_pBTParamBand->stSpec.stBleExSpec.dF0FnMax[ePacketType].upp
                , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
                , nChan
                , "-"
                , "Frequency:%0.2f;%s;%s"
                , dFreq
                , CwcnUtility::BT_ANT_NAME[ePath]
                , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
            );
            if (ePacketType == RF_PHY_S8)
            {
                m_pImpBase->_UiSendMsg("F0-F3"
                    , LEVEL_ITEM | LEVEL_FT
                    , m_pBTParamBand->stSpec.stBleExSpec.dF1F0[ePacketType].low
                    , pTestResult->CarrierFreqDrift_BLE.f1f0
                    , m_pBTParamBand->stSpec.stBleExSpec.dF1F0[ePacketType].upp
                    , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
                    , nChan
                    , "-"
                    , "Frequency:%0.2f;%s;%s"
                    , dFreq
                    , CwcnUtility::BT_ANT_NAME[ePath]
                    , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
                );

                m_pImpBase->_UiSendMsg("Fn-F(n-3)"
                    , LEVEL_ITEM | LEVEL_FT
                    , m_pBTParamBand->stSpec.stBleExSpec.dFnFn5[ePacketType].low
                    , pTestResult->CarrierFreqDrift_BLE.fnfn5
                    , m_pBTParamBand->stSpec.stBleExSpec.dFnFn5[ePacketType].upp
                    , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
                    , nChan
                    , "-"
                    , "Frequency:%0.2f;%s;%s"
                    , dFreq
                    , CwcnUtility::BT_ANT_NAME[ePath]
                    , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
                );
            }
            else
            {
                m_pImpBase->_UiSendMsg("F1-F0"
                    , LEVEL_ITEM | LEVEL_FT
                    , m_pBTParamBand->stSpec.stBleExSpec.dF1F0[ePacketType].low
                    , pTestResult->CarrierFreqDrift_BLE.f1f0
                    , m_pBTParamBand->stSpec.stBleExSpec.dF1F0[ePacketType].upp
                    , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
                    , nChan
                    , "-"
                    , "Frequency:%0.2f;%s;%s"
                    , dFreq
                    , CwcnUtility::BT_ANT_NAME[ePath]
                    , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
                );

                m_pImpBase->_UiSendMsg("Fn-F(n-5)"
                    , LEVEL_ITEM | LEVEL_FT
                    , m_pBTParamBand->stSpec.stBleExSpec.dFnFn5[ePacketType].low
                    , pTestResult->CarrierFreqDrift_BLE.fnfn5
                    , m_pBTParamBand->stSpec.stBleExSpec.dFnFn5[ePacketType].upp
                    , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
                    , nChan
                    , "-"
                    , "Frequency:%0.2f;%s;%s"
                    , dFreq
                    , CwcnUtility::BT_ANT_NAME[ePath]
                    , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
                );
            }
        }

        //MAX ICFR
        if (IS_BIT_SET(dwItemMask, FREQ_MAXICFR))
        {
            m_pImpBase->_UiSendMsg("MAX BLE Carrier Frequency Accuracy"
                , LEVEL_ITEM | LEVEL_FT
                , m_pBTParamBand->stSpec.stBleExSpec.dIcftMax[ePacketType].low
                , pTestResult->CarrierFreqDrift_BLE.dIcftMax
                , m_pBTParamBand->stSpec.stBleExSpec.dIcftMax[ePacketType].upp
                , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
                , nChan
                , "-"
                , "Frequency:%0.2f;%s;%s"
                , dFreq
                , CwcnUtility::BT_ANT_NAME[ePath]
                , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
            );
        }
        //MIN ICFR
        if (IS_BIT_SET(dwItemMask, FREQ_MINICFR))
        {
            m_pImpBase->_UiSendMsg("MIN BLE Carrier Frequency Accuracy"
                , LEVEL_ITEM | LEVEL_FT
                , m_pBTParamBand->stSpec.stBleExSpec.dIcftMin[ePacketType].low
                , pTestResult->CarrierFreqDrift_BLE.dIcftMin
                , m_pBTParamBand->stSpec.stBleExSpec.dIcftMin[ePacketType].upp
                , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
                , nChan
                , "-"
                , "Frequency:%0.2f;%s;%s"
                , dFreq
                , CwcnUtility::BT_ANT_NAME[ePath]
                , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
            );
        }

        //MAX CFD
        if (IS_BIT_SET(dwItemMask, FREQ_MAXCFD))
        {
            m_pImpBase->_UiSendMsg("MAX BLE Carrier Freq Drift"
                , LEVEL_ITEM | LEVEL_FT
                , m_pBTParamBand->stSpec.stBleExSpec.dCfdMax[ePacketType].low
                , pTestResult->CarrierFreqDrift_BLE.dCfdMax
                , m_pBTParamBand->stSpec.stBleExSpec.dCfdMax[ePacketType].upp
                , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
                , nChan
                , "-"
                , "Frequency:%0.2f;%s;%s"
                , dFreq
                , CwcnUtility::BT_ANT_NAME[ePath]
                , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
            );
        }
        //MIN CFD
        if (IS_BIT_SET(dwItemMask, FREQ_MINCFD))
        {
            m_pImpBase->_UiSendMsg("MIN BLE Carrier Freq Drift"
                , LEVEL_ITEM | LEVEL_FT
                , m_pBTParamBand->stSpec.stBleExSpec.dCfdMin[ePacketType].low
                , pTestResult->CarrierFreqDrift_BLE.dCfdMin
                , m_pBTParamBand->stSpec.stBleExSpec.dCfdMin[ePacketType].upp
                , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
                , nChan
                , "-"
                , "Frequency:%0.2f;%s;%s"
                , dFreq
                , CwcnUtility::BT_ANT_NAME[ePath]
                , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
            );
        }

        if (IS_BIT_SET(dwItemMask, BLE_PER))
        {
            m_pImpBase->_UiSendMsg("Sensitivity - PER"
                , LEVEL_ITEM | LEVEL_FT
                , 0.0
                , pTestResult->BER.dAvgValue
                , m_pBTParamBand->stSpec.stBleExSpec.dBer
                , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
                , nChan
                , "-"
                , "Frequency:%0.2f;%s;%s;BSLevel:%.2f"
                , dFreq
                , CwcnUtility::BT_ANT_NAME[ePath]
                , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
                , dBSLevel
            );
        }

        if (IS_BIT_SET(dwItemMask, BLE_RSSI))
        {
            m_pImpBase->_UiSendMsg("RSSITest"
                , LEVEL_ITEM | LEVEL_FT
                , dBSLevel + m_pBTParamBand->stSpec.stBleExSpec.dRssi.low
                , pTestResult->Rssi.dAvgValue
                , dBSLevel + m_pBTParamBand->stSpec.stBleExSpec.dRssi.upp
                , CwcnUtility::BT_BAND_NAME[m_pBTParamBand->eMode]
                , nChan
                , "-"
                , "Frequency:%0.2f;%s;%s;BSLevel:%.2f"
                , dFreq
                , CwcnUtility::BT_ANT_NAME[ePath]
                , CwcnUtility::BTGetPacketString(m_pBTParamBand->eMode, ePacketType)
                , dBSLevel
            );
        }

        return SP_OK;
    }

    SPRESULT CBTMeasBase::JudgeMeasRst(DWORD  dwItemMask, SPBT_RESULT_T * pTestResult, BT_RFPATH_ENUM ePath, BT_PACKET ePacketType, double dBSLevel)
    {
        auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
        if (pTestResult == NULL)
        {
            CHKRESULT_WITH_NOTIFY_WCN_ITEM(SP_E_INVALID_PARAMETER, "BT: Invalid result!");
        }

        switch (m_pBTParamBand->eMode)
        {
        case BDR:
            CHKRESULT(_JudgeBdrRslt(dwItemMask, pTestResult, ePath, dBSLevel));
            break;
        case EDR:
            CHKRESULT(_JudgeEdrRslt(dwItemMask, pTestResult, ePath, ePacketType, dBSLevel));
            break;
        case BLE:
            CHKRESULT(_JudgeBleRslt(dwItemMask, pTestResult, ePath, dBSLevel));
            break;
        case BLE_EX:
            CHKRESULT(_JudgeBleExRslt(dwItemMask, pTestResult, ePath, ePacketType, dBSLevel));
            break;
        default:
            break;
        }

        return SP_OK;
    }

    SPRESULT CBTMeasBase::TxSpecialPatternMeas(DWORD dwMask, SPWI_BT_PARAM_TESTER * pstRfTester, SPBT_RESULT_T * pTestResult)
    {
        auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
        if (NULL == pstRfTester)
        {
            CHKRESULT_WITH_NOTIFY_WCN_ITEM(SP_E_SPAT_INVALID_PARAMETER, "BT::Invalid parameter!");
        }
        //Set pattern
        CHKRESULT(m_pBTApi->DUT_SetPattern(BT_TX, pstRfTester->ePattern));
        //Transmit enable
        CHKRESULT_WITH_NOTIFY_BT_POWER_OFF(m_pBTApi->DUT_RFOn(BT_TX, true), "DUT_RFOn(TRUE)");
        //	Sleep(500);
            //Setup tester for meas
        m_pImpBase->SetRepairItem($REPAIR_ITEM_INSTRUMENT);
        SPRESULT res = m_pRfTesterBT->InitTest(dwMask, pstRfTester);
        if (SP_OK != res)
        {
            m_pImpBase->NOTIFY("InitTest(TX)", LEVEL_ITEM, 1, 0, 1);
            m_pBTApi->DUT_RFOn(BT_TX, false);
            return res;
        }
        //Fetch meas results
        res = m_pRfTesterBT->FetchResult(dwMask, pTestResult);
        if (SP_OK != res)
        {
            m_pImpBase->NOTIFY("FetchResult(TX)", LEVEL_ITEM, 1, 0, 1);

            m_pBTApi->DUT_RFOn(BT_TX, false);

            return res;
        }
        //Transmit disable
        CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_RFOn(BT_TX, false), "DUT_RFOn(BT_TX,false)");
        return SP_OK;
    }


    SPRESULT CBTMeasBase::TxCWMeas(DWORD dwMask, SPWI_BT_PARAM_TESTER * pstRfTester, SPBT_RESULT_T * pTestResult)
    {
        auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
        if (NULL == pstRfTester)
        {
            CHKRESULT_WITH_NOTIFY_WCN_ITEM(SP_E_SPAT_INVALID_PARAMETER, "BT::Invalid parameter!");
        }
        //Set pattern
        CHKRESULT(m_pBTApi->DUT_SetPattern(BT_TX, pstRfTester->ePattern));
        //Set PowerLevel
        CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_SetPowerLevel(9), "DUT_SetPowerLevel");
        //Transmit enable
        CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_CWOn(BT_TX, true), "DUT_CWOn(TRUE)");
        //	Sleep(500);
            //Setup tester for meas
        m_pImpBase->SetRepairItem($REPAIR_ITEM_INSTRUMENT);
        SPRESULT res = m_pRfTesterBT->InitTest(dwMask, pstRfTester);
        if (SP_OK != res)
        {
            m_pImpBase->NOTIFY("InitTest(TX)", LEVEL_ITEM, 1, 0, 1);
			m_pBTApi->DUT_CWOn(BT_TX, false);
            return res;
        }
        //Fetch meas results
        res = m_pRfTesterBT->FetchResult(dwMask, pTestResult);
        if (SP_OK != res)
        {
            m_pImpBase->NOTIFY("FetchResult(TX)", LEVEL_ITEM, 1, 0, 1);
            m_pBTApi->DUT_RFOn(BT_TX, false);
            return res;
        }
        //Transmit disable
        CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_CWOn(BT_TX, false), "DUT_CWOn(BT_TX,false)");
        return SP_OK;
    }

    SPRESULT CBTMeasBase::BDTxSpecialPatternMeas(DWORD dwMask, SPWI_BT_PARAM_TESTER * pstRfTester, SPBT_RESULT_T * pTestResult)
    {
        auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
        if (NULL == pstRfTester)
        {
            CHKRESULT_WITH_NOTIFY_WCN_ITEM(SP_E_SPAT_INVALID_PARAMETER, "BT::Invalid parameter!");
        }

        //Set pattern
        CHKRESULT(m_pBTApi->DUT_SetPattern(BT_TX, pstRfTester->ePattern));
        //Set power level mode.
        CHKRESULT(m_pBTApi->BDDUT_SetTxPwrLevel(9));
        //Set tx power value(dBm).
        CHKRESULT(m_pBTApi->BDDUT_SetTxPwrValue(5));
        //Set tx power table index.
        CHKRESULT(m_pBTApi->BDDUT_SetTxPwrTabIdx(0));
        //Transmit enable
        CHKRESULT_WITH_NOTIFY_BT_POWER_OFF(m_pBTApi->DUT_RFOn(BT_TX, true), "DUT_RFOn(TRUE)");
        Sleep(500);
        //Setup tester for meas
        m_pImpBase->SetRepairItem($REPAIR_ITEM_INSTRUMENT);
        SPRESULT res = m_pRfTesterBT->InitTest(dwMask, pstRfTester);
        if (SP_OK != res)
        {
            m_pImpBase->NOTIFY("InitTest(TX)", LEVEL_ITEM, 1, 0, 1);
            //if (!m_pImpBase->m_bFailStop)
            {
                m_pBTApi->DUT_RFOn(BT_TX, false);
            }

            return res;
        }
        //Fetch meas results
        res = m_pRfTesterBT->FetchResult(dwMask, pTestResult);
        if (SP_OK != res)
        {
            m_pImpBase->NOTIFY("FetchResult(TX)", LEVEL_ITEM, 1, 0, 1);
            //if (!m_pImpBase->m_bFailStop)
            {
                m_pBTApi->DUT_RFOn(BT_TX, false);
            }

            return res;
        }
        //Transmit disable
        CHKRESULT_WITH_NOTIFY_WCN_ITEM(m_pBTApi->DUT_RFOn(BT_TX, false), "DUT_RFOn(BT_TX,false)");
        return SP_OK;
    }

    SPRESULT CBTMeasBase::_JudgeBdrRslt(DWORD  dwItemMask, SPBT_RESULT_T * pTestResult, BT_RFPATH_ENUM ePath, double dBSLevel)
    {
        auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
        SPRESULT eCmpRlst = SP_OK;
        SPRESULT eItemRlst = SP_OK;

        if (NULL == pTestResult)
        {
            return SP_E_SPAT_INVALID_PARAMETER;
        }

        if (IS_BIT_SET(dwItemMask, BDR_POWER))
        {
            double dTxpAvglow = 0.0;
            double dTxpAvgupp = 0.0;
            if (ANT_SINGLE == ePath)
            {
                dTxpAvglow = m_pBTParamBand->stSpec.stBdrSpec.dTxp.low;
                dTxpAvgupp = m_pBTParamBand->stSpec.stBdrSpec.dTxp.upp;
            }
            else
            {
                dTxpAvglow = m_pBTParamBand->stSpec.stBdrSpec.dTxpShared.low;
                dTxpAvgupp = m_pBTParamBand->stSpec.stBdrSpec.dTxpShared.upp;
            }

            m_pImpBase->SetRepairItem($REPAIR_ITEM_TXP);
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BDR TXP meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->Power.dAvgValue,
                dTxpAvglow,
                dTxpAvgupp);

            RSLT_RANGE_CHECK(pTestResult->Power.dAvgValue
                , dTxpAvglow
                , dTxpAvgupp
                , eItemRlst
                , SP_E_WCN_BT_TXP_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
        }

        if (IS_BIT_SET(dwItemMask, BT_CW))
        {
            double dTxpAvglow = 0.0;
            double dTxpAvgupp = 0.0;
            if (ANT_SINGLE == ePath)
            {
                dTxpAvglow = m_pBTParamBand->stSpec.stBdrSpec.dTxp.low;
                dTxpAvgupp = m_pBTParamBand->stSpec.stBdrSpec.dTxp.upp;
            }
            else
            {
                dTxpAvglow = m_pBTParamBand->stSpec.stBdrSpec.dTxpShared.low;
                dTxpAvgupp = m_pBTParamBand->stSpec.stBdrSpec.dTxpShared.upp;
            }

            m_pImpBase->SetRepairItem($REPAIR_ITEM_TXP);
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BDR CW TXP meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->Power.dAvgValue,
                dTxpAvglow,
                dTxpAvgupp);

            RSLT_RANGE_CHECK(pTestResult->Power.dAvgValue
                , dTxpAvglow
                , dTxpAvgupp
                , eItemRlst
                , SP_E_WCN_BT_TXP_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
        }

        if (IS_BIT_SET(dwItemMask, BDR_20BW))
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_OBW);
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BDR BDR_20BW meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->BW20dB.dAvgValue
                , m_pBTParamBand->stSpec.stBdrSpec.dTsbd.low
                , m_pBTParamBand->stSpec.stBdrSpec.dTsbd.upp);

            RSLT_RANGE_CHECK(pTestResult->BW20dB.dAvgValue
                , m_pBTParamBand->stSpec.stBdrSpec.dTsbd.low
                , m_pBTParamBand->stSpec.stBdrSpec.dTsbd.upp
                , eItemRlst, SP_E_WCN_BT_OBW_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
        }

        if (IS_BIT_SET(dwItemMask, BDR_ACP))
        {
            INT nTotalACPFailCount = 0;
            double dMaxAcp = -999.0;

            m_pImpBase->SetRepairItem($REPAIR_ITEM_ACLR);
            for (int nAcpIndx = 0; nAcpIndx < 11; nAcpIndx++)
            {
                m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BDR BDR_ACP meas = %.3f, Low = %.3f, high = %.3f",
                    pTestResult->ACP.acp[nAcpIndx]
                    , m_pBTParamBand->stSpec.stBdrSpec.dTsacp[nAcpIndx].low
                    , m_pBTParamBand->stSpec.stBdrSpec.dTsacp[nAcpIndx].upp);

                if (((nAcpIndx < 4) || (nAcpIndx > 6)) && (pTestResult->ACP.acp[nAcpIndx] > dMaxAcp))
                {
                    dMaxAcp = pTestResult->ACP.acp[nAcpIndx];
                }

                RSLT_RANGE_CHECK(pTestResult->ACP.acp[nAcpIndx]
                    , m_pBTParamBand->stSpec.stBdrSpec.dTsacp[nAcpIndx].low
                    , m_pBTParamBand->stSpec.stBdrSpec.dTsacp[nAcpIndx].upp
                    , eItemRlst, SP_E_WCN_BT_ACP_FAIL);

                if (SP_OK != eItemRlst)
                {
                    nTotalACPFailCount++;
                }
            }
            if (nTotalACPFailCount <= 3 && (dMaxAcp < ACP_ABS_LIMITION))
            {
                eItemRlst = SP_OK;
                pTestResult->ACP.indicator = TRUE;
            }
            else
            {
                eItemRlst = SP_E_WCN_BT_ACP_FAIL;
                pTestResult->ACP.indicator = FALSE;
            }
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
        }

        if (IS_BIT_SET(dwItemMask, BDR_ICFR))
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_ICFT);
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BDR BDR_ICFT meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->InitalCarrierFreq.dAvgValue
                , m_pBTParamBand->stSpec.stBdrSpec.dIcft.low
                , m_pBTParamBand->stSpec.stBdrSpec.dIcft.upp);

            RSLT_RANGE_CHECK(pTestResult->InitalCarrierFreq.dAvgValue
                , m_pBTParamBand->stSpec.stBdrSpec.dIcft.low
                , m_pBTParamBand->stSpec.stBdrSpec.dIcft.upp
                , eItemRlst, SP_E_WCN_BT_ICFR_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
        }

        //MAX ICFT
        if (IS_BIT_SET(dwItemMask, FREQ_MAXICFR))
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_ICFT);
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: MAX BDR_ICFT meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->InitalCarrierFreq.dMaxValue
                , m_pBTParamBand->stSpec.stBdrSpec.dIcftMax.low
                , m_pBTParamBand->stSpec.stBdrSpec.dIcftMax.upp);

            RSLT_RANGE_CHECK(pTestResult->InitalCarrierFreq.dMaxValue
                , m_pBTParamBand->stSpec.stBdrSpec.dIcftMax.low
                , m_pBTParamBand->stSpec.stBdrSpec.dIcftMax.upp
                , eItemRlst, SP_E_WCN_BT_ICFR_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
        }
        //MIN ICFT
        if (IS_BIT_SET(dwItemMask, FREQ_MINICFR))
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_ICFT);
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: MIN BDR_ICFT meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->InitalCarrierFreq.dMinValue
                , m_pBTParamBand->stSpec.stBdrSpec.dIcftMin.low
                , m_pBTParamBand->stSpec.stBdrSpec.dIcftMin.upp);

            RSLT_RANGE_CHECK(pTestResult->InitalCarrierFreq.dMinValue
                , m_pBTParamBand->stSpec.stBdrSpec.dIcftMin.low
                , m_pBTParamBand->stSpec.stBdrSpec.dIcftMin.upp
                , eItemRlst, SP_E_WCN_BT_ICFR_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
        }

        if (IS_BIT_SET(dwItemMask, BDR_CFD))
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_CFD);
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BDR BDR_CFD meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->CarrierFreqDrift.dAvgValue
                , m_pBTParamBand->stSpec.stBdrSpec.dCfd.low
                , m_pBTParamBand->stSpec.stBdrSpec.dCfd.upp);

            RSLT_RANGE_CHECK(pTestResult->CarrierFreqDrift.dAvgValue
                , m_pBTParamBand->stSpec.stBdrSpec.dCfd.low
                , m_pBTParamBand->stSpec.stBdrSpec.dCfd.upp
                , eItemRlst, SP_E_WCN_BT_CFD_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
        }

        //MAX CFD
        if (IS_BIT_SET(dwItemMask, FREQ_MAXCFD))
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_CFD);
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: MAX BDR_CFD meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->CarrierFreqDrift.dMaxValue
                , m_pBTParamBand->stSpec.stBdrSpec.dCfdMax.low
                , m_pBTParamBand->stSpec.stBdrSpec.dCfdMax.upp);

            RSLT_RANGE_CHECK(pTestResult->CarrierFreqDrift.dMaxValue
                , m_pBTParamBand->stSpec.stBdrSpec.dCfdMax.low
                , m_pBTParamBand->stSpec.stBdrSpec.dCfdMax.upp
                , eItemRlst, SP_E_WCN_BT_CFD_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
        }
        //MIN CFD
        if (IS_BIT_SET(dwItemMask, FREQ_MINCFD))
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_CFD);
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: MIN BDR_CFD meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->CarrierFreqDrift.dMinValue
                , m_pBTParamBand->stSpec.stBdrSpec.dCfdMin.low
                , m_pBTParamBand->stSpec.stBdrSpec.dCfdMin.upp);

            RSLT_RANGE_CHECK(pTestResult->CarrierFreqDrift.dMinValue
                , m_pBTParamBand->stSpec.stBdrSpec.dCfdMin.low
                , m_pBTParamBand->stSpec.stBdrSpec.dCfdMin.upp
                , eItemRlst, SP_E_WCN_BT_CFD_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
        }

        if (IS_BIT_SET(dwItemMask, BDR_MC))
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_MC);
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BDR BDR_MC:CFD meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->Modulation.f1avg
                , m_pBTParamBand->stSpec.stBdrSpec.dModF1avg.low
                , m_pBTParamBand->stSpec.stBdrSpec.dModF1avg.upp);

            RSLT_RANGE_CHECK(pTestResult->Modulation.f1avg
                , m_pBTParamBand->stSpec.stBdrSpec.dModF1avg.low
                , m_pBTParamBand->stSpec.stBdrSpec.dModF1avg.upp
                , eItemRlst, SP_E_WCN_BT_MC_F1AVG_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }

            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BDR BDR_MC:ModFlag meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->Modulation.f2max
                , m_pBTParamBand->stSpec.stBdrSpec.dModF2max.low
                , m_pBTParamBand->stSpec.stBdrSpec.dModF2max.upp);

            RSLT_RANGE_CHECK(pTestResult->Modulation.f2max
                , m_pBTParamBand->stSpec.stBdrSpec.dModF2max.low
                , m_pBTParamBand->stSpec.stBdrSpec.dModF2max.upp
                , eItemRlst, SP_E_WCN_BT_MC_F2MAX_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }

            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BDR BDR_MC:ModFlag:max meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->Modulation.f2f1avg_rate
                , m_pBTParamBand->stSpec.stBdrSpec.dModRatio
                , 999.0);

            RSLT_RANGE_CHECK(pTestResult->Modulation.f2f1avg_rate
                , m_pBTParamBand->stSpec.stBdrSpec.dModRatio
                , 999.0
                , eItemRlst, SP_E_WCN_BT_MC_F2TOF1_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
        }
        if (IS_BIT_SET(dwItemMask, BDR_BER))
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_BER);
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BDR BER meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->BER.dAvgValue,
                0.0,
                m_pBTParamBand->stSpec.stBdrSpec.dBer);

            RSLT_RANGE_CHECK(pTestResult->BER.dAvgValue,
                0.0,
                m_pBTParamBand->stSpec.stBdrSpec.dBer
                , eItemRlst, SP_E_WCN_BT_PER_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }

        }

        if (IS_BIT_SET(dwItemMask, BLE_RSSI))
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_BER);
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BDR RSSI meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->Rssi.dAvgValue,
                dBSLevel + m_pBTParamBand->stSpec.stBdrSpec.dRssi.low,
                dBSLevel + m_pBTParamBand->stSpec.stBdrSpec.dRssi.upp);

            RSLT_RANGE_CHECK(pTestResult->Rssi.dAvgValue,
                dBSLevel + m_pBTParamBand->stSpec.stBdrSpec.dRssi.low,
                dBSLevel + m_pBTParamBand->stSpec.stBdrSpec.dRssi.upp,
                eItemRlst, SP_E_WCN_BT_RSSI_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
        }

        return eCmpRlst;
    }

    SPRESULT CBTMeasBase::_JudgeEdrRslt(DWORD  dwItemMask, SPBT_RESULT_T * pTestResult, BT_RFPATH_ENUM ePath, BT_PACKET ePacketType, double dBSLevel)
    {
        auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
        if (NULL == pTestResult)
        {
            return SP_E_SPAT_INVALID_PARAMETER;
        }

        SPRESULT eCmpRlst = SP_OK;
        SPRESULT eItemRlst = SP_OK;

        if (IS_BIT_SET(dwItemMask, BDR_POWER))
        {
            double dTxpAvglow = 0.0;
            double dTxpAvgupp = 0.0;
            if (ANT_SINGLE == ePath)
            {
                dTxpAvglow = m_pBTParamBand->stSpec.stEdrSpec.dTxp.low;
                dTxpAvgupp = m_pBTParamBand->stSpec.stEdrSpec.dTxp.upp;
            }
            else
            {
                dTxpAvglow = m_pBTParamBand->stSpec.stEdrSpec.dTxpShared.low;
                dTxpAvgupp = m_pBTParamBand->stSpec.stEdrSpec.dTxpShared.upp;
            }
            m_pImpBase->SetRepairItem($REPAIR_ITEM_TXP);
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: EDR POWER meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->Power.dAvgValue,
                dTxpAvglow,
                dTxpAvgupp);

            RSLT_RANGE_CHECK(pTestResult->Power.dAvgValue
                , dTxpAvglow
                , dTxpAvgupp
                , eItemRlst
                , SP_E_WCN_BT_TXP_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
        }

        if (IS_BIT_SET(dwItemMask, BT_CW))
        {
            double dTxpAvglow = 0.0;
            double dTxpAvgupp = 0.0;
            if (ANT_SINGLE == ePath)
            {
                dTxpAvglow = m_pBTParamBand->stSpec.stEdrSpec.dTxp.low;
                dTxpAvgupp = m_pBTParamBand->stSpec.stEdrSpec.dTxp.upp;
            }
            else
            {
                dTxpAvglow = m_pBTParamBand->stSpec.stEdrSpec.dTxpShared.low;
                dTxpAvgupp = m_pBTParamBand->stSpec.stEdrSpec.dTxpShared.upp;
            }
            m_pImpBase->SetRepairItem($REPAIR_ITEM_TXP);
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: EDR CW POWER meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->Power.dAvgValue,
                dTxpAvglow,
                dTxpAvgupp);

            RSLT_RANGE_CHECK(pTestResult->Power.dAvgValue
                , dTxpAvglow
                , dTxpAvgupp
                , eItemRlst
                , SP_E_WCN_BT_TXP_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
        }

        if (IS_BIT_SET(dwItemMask, EDR_ETP))
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_RTP);
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: EDR POWER meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->RelativePower.dAvgValue
                , m_pBTParamBand->stSpec.stEdrSpec.dRTxp.low
                , m_pBTParamBand->stSpec.stEdrSpec.dRTxp.upp);

            RSLT_RANGE_CHECK(pTestResult->RelativePower.dAvgValue
                , m_pBTParamBand->stSpec.stEdrSpec.dRTxp.low
                , m_pBTParamBand->stSpec.stEdrSpec.dRTxp.upp
                , eItemRlst, SP_E_WCN_BT_RTP_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
        }

        if (IS_BIT_SET(dwItemMask, EDR_ACP))
        {
            INT nTotalACPFailCount = 0; // Bug 810885
            double dMaxAcp = -999.0;

            m_pImpBase->SetRepairItem($REPAIR_ITEM_ACLR);
            for (int nAcpIndx = 0; nAcpIndx < 11; nAcpIndx++)
            {
                m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: EDR ACP meas = %.3f, Low = %.3f, high = %.3f",
                    pTestResult->ACP.acp[nAcpIndx]
                    , m_pBTParamBand->stSpec.stEdrSpec.dIbse[nAcpIndx].low
                    , m_pBTParamBand->stSpec.stEdrSpec.dIbse[nAcpIndx].upp);

                if ((nAcpIndx != 5) && (pTestResult->ACP.acp[nAcpIndx] > dMaxAcp))
                {
                    dMaxAcp = pTestResult->ACP.acp[nAcpIndx];
                }

                RSLT_RANGE_CHECK(pTestResult->ACP.acp[nAcpIndx]
                    , m_pBTParamBand->stSpec.stEdrSpec.dIbse[nAcpIndx].low
                    , m_pBTParamBand->stSpec.stEdrSpec.dIbse[nAcpIndx].upp
                    , eItemRlst, SP_E_WCN_BT_ACP_FAIL);

                if (SP_OK != eItemRlst)
                {
                    nTotalACPFailCount++;
                }
            }
            if (nTotalACPFailCount <= 3 && (dMaxAcp < ACP_ABS_LIMITION))
            {
                eItemRlst = SP_OK;
                pTestResult->ACP.indicator = TRUE;
            }
            else
            {
                eItemRlst = SP_E_WCN_BT_ACP_FAIL;
                pTestResult->ACP.indicator = FALSE;
            }
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
        }

        if (IS_BIT_SET(dwItemMask, EDR_DEVM))
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_EVM);

            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: EDR EVM:W0 meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->DEVM.w0
                , m_pBTParamBand->stSpec.stEdrSpec.dW0.low
                , m_pBTParamBand->stSpec.stEdrSpec.dW0.upp);

            RSLT_RANGE_CHECK(pTestResult->DEVM.w0
                , m_pBTParamBand->stSpec.stEdrSpec.dW0.low
                , m_pBTParamBand->stSpec.stEdrSpec.dW0.upp
                , eItemRlst, SP_E_WCN_BT_EVM_W0_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }

            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: EDR EVM:Wi meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->DEVM.wi
                , m_pBTParamBand->stSpec.stEdrSpec.dWi.low
                , m_pBTParamBand->stSpec.stEdrSpec.dWi.upp
            );
            RSLT_RANGE_CHECK(pTestResult->DEVM.wi
                , m_pBTParamBand->stSpec.stEdrSpec.dWi.low
                , m_pBTParamBand->stSpec.stEdrSpec.dWi.upp
                , eItemRlst, SP_E_WCN_BT_EVM_WI_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }

            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: EDR EVM:Wi0 meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->DEVM.wi0
                , m_pBTParamBand->stSpec.stEdrSpec.dWi.low
                , m_pBTParamBand->stSpec.stEdrSpec.dWi.upp
            );
            RSLT_RANGE_CHECK(pTestResult->DEVM.wi0
                , m_pBTParamBand->stSpec.stEdrSpec.dWi.low
                , m_pBTParamBand->stSpec.stEdrSpec.dWi.upp
                , eItemRlst, SP_E_WCN_BT_EVM_WI0_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }

            double dRmsEvmLow = m_pBTParamBand->stSpec.stEdrSpec.dRmsEvm_2DHX.low;
            double dRmsEvmUpp = m_pBTParamBand->stSpec.stEdrSpec.dRmsEvm_2DHX.upp;
            double dPeakEvmLow = m_pBTParamBand->stSpec.stEdrSpec.dPeakEvm_2DHX.low;
            double dPeakEvmUpp = m_pBTParamBand->stSpec.stEdrSpec.dPeakEvm_2DHX.upp;
            double dP99EvmLow = m_pBTParamBand->stSpec.stEdrSpec.dP99Evm_2DHX.low;
            double dP99EvmUpp = m_pBTParamBand->stSpec.stEdrSpec.dP99Evm_2DHX.upp;
            if (ePacketType >= EDR_3DH1)
            {
                dRmsEvmLow = m_pBTParamBand->stSpec.stEdrSpec.dRmsEvm_3DHX.low;
                dRmsEvmUpp = m_pBTParamBand->stSpec.stEdrSpec.dRmsEvm_3DHX.upp;
                dPeakEvmLow = m_pBTParamBand->stSpec.stEdrSpec.dPeakEvm_3DHX.low;
                dPeakEvmUpp = m_pBTParamBand->stSpec.stEdrSpec.dPeakEvm_3DHX.upp;
                dP99EvmLow = m_pBTParamBand->stSpec.stEdrSpec.dP99Evm_3DHX.low;
                dP99EvmUpp = m_pBTParamBand->stSpec.stEdrSpec.dP99Evm_3DHX.upp;
            }

            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: EDR EVM:avgEVM meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->DEVM.avgEVM
                , dRmsEvmLow
                , dRmsEvmUpp
            );
            RSLT_RANGE_CHECK(pTestResult->DEVM.avgEVM
                , dRmsEvmLow
                , dRmsEvmUpp
                , eItemRlst, SP_E_WCN_BT_EVM_AVG_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }

            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: EDR EVM:maxEVM meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->DEVM.maxEVM
                , dPeakEvmLow
                , dPeakEvmUpp
            );
            RSLT_RANGE_CHECK(pTestResult->DEVM.maxEVM
                , dPeakEvmLow
                , dPeakEvmUpp
                , eItemRlst, SP_E_WCN_BT_EVM_MAX_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
            //99%
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: EDR EVM:p99EVM meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->DEVM.p99EVM
                , dP99EvmLow
                , dP99EvmUpp
            );
            RSLT_RANGE_CHECK(pTestResult->DEVM.p99EVM
                , dP99EvmLow
                , dP99EvmUpp
                , eItemRlst, SP_E_WCN_BT_EVM_MAX_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
            //
        }

        if (IS_BIT_SET(dwItemMask, BDR_BER))
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_BER);
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: EDR BER meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->BER.dAvgValue,
                0.0,
                m_pBTParamBand->stSpec.stEdrSpec.dBer);

            RSLT_RANGE_CHECK(pTestResult->BER.dAvgValue,
                0.0,
                m_pBTParamBand->stSpec.stEdrSpec.dBer
                , eItemRlst, SP_E_WCN_BT_PER_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
        }

        if (IS_BIT_SET(dwItemMask, BLE_RSSI))
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_BER);
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: EDR RSSI meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->Rssi.dAvgValue,
                dBSLevel + m_pBTParamBand->stSpec.stEdrSpec.dRssi.low,
                dBSLevel + m_pBTParamBand->stSpec.stEdrSpec.dRssi.upp);

            RSLT_RANGE_CHECK(pTestResult->Rssi.dAvgValue,
                dBSLevel + m_pBTParamBand->stSpec.stEdrSpec.dRssi.low,
                dBSLevel + m_pBTParamBand->stSpec.stEdrSpec.dRssi.upp,
                eItemRlst, SP_E_WCN_BT_RSSI_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
        }

        return eCmpRlst;
    }

    SPRESULT CBTMeasBase::_JudgeBleRslt(DWORD  dwItemMask, SPBT_RESULT_T * pTestResult, BT_RFPATH_ENUM ePath, double dBSLevel)
    {
        auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
        if (NULL == pTestResult)
        {
            return SP_E_SPAT_INVALID_PARAMETER;
        }

        SPRESULT eCmpRlst = SP_OK;
        SPRESULT eItemRlst = SP_OK;

        if (IS_BIT_SET(dwItemMask, BLE_POWER))
        {
            double dTxpAvglow = 0.0;
            double dTxpAvgupp = 0.0;
            if (ANT_SINGLE == ePath)
            {
                dTxpAvglow = m_pBTParamBand->stSpec.stBleSpec.dTxpAvg.low;
                dTxpAvgupp = m_pBTParamBand->stSpec.stBleSpec.dTxpAvg.upp;
            }
            else
            {
                dTxpAvglow = m_pBTParamBand->stSpec.stBleSpec.dTxpAvgShared.low;
                dTxpAvgupp = m_pBTParamBand->stSpec.stBleSpec.dTxpAvgShared.upp;
            }
            m_pImpBase->SetRepairItem($REPAIR_ITEM_TXP);
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE Power meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->Power.dAvgValue
                , dTxpAvglow
                , dTxpAvgupp
            );
            RSLT_RANGE_CHECK(pTestResult->Power.dAvgValue
                , dTxpAvglow
                , dTxpAvgupp
                , eItemRlst,
                SP_E_WCN_BT_TXP_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }

            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE Power:Relative meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->Power.dMaxValue - pTestResult->Power.dAvgValue
                , m_pBTParamBand->stSpec.stBleSpec.dTxpPeak.low
                , m_pBTParamBand->stSpec.stBleSpec.dTxpPeak.upp
            );
            RSLT_RANGE_CHECK(pTestResult->Power.dMaxValue - pTestResult->Power.dAvgValue
                , m_pBTParamBand->stSpec.stBleSpec.dTxpPeak.low
                , m_pBTParamBand->stSpec.stBleSpec.dTxpPeak.upp
                , eItemRlst, SP_E_WCN_BT_RTP_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
        }

        if (IS_BIT_SET(dwItemMask, BT_CW))
        {
            double dTxpAvglow = 0.0;
            double dTxpAvgupp = 0.0;
            if (ANT_SINGLE == ePath)
            {
                dTxpAvglow = m_pBTParamBand->stSpec.stBleSpec.dTxpAvg.low;
                dTxpAvgupp = m_pBTParamBand->stSpec.stBleSpec.dTxpAvg.upp;
            }
            else
            {
                dTxpAvglow = m_pBTParamBand->stSpec.stBleSpec.dTxpAvgShared.low;
                dTxpAvgupp = m_pBTParamBand->stSpec.stBleSpec.dTxpAvgShared.upp;
            }
            m_pImpBase->SetRepairItem($REPAIR_ITEM_TXP);
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE CW Power meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->Power.dAvgValue
                , dTxpAvglow
                , dTxpAvgupp
            );
            RSLT_RANGE_CHECK(pTestResult->Power.dAvgValue
                , dTxpAvglow
                , dTxpAvgupp
                , eItemRlst,
                SP_E_WCN_BT_TXP_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
        }

        if (IS_BIT_SET(dwItemMask, BLE_ACP))
        {
            SPRESULT eAcpRslt = SP_OK;
            m_pImpBase->SetRepairItem($REPAIR_ITEM_ACLR);
            for (int nAcpIndx = 0; nAcpIndx < 11; nAcpIndx++)
            {
                m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE ACP meas = %.3f, Low = %.3f, high = %.3f",
                    pTestResult->ACP.acp[nAcpIndx]
                    , m_pBTParamBand->stSpec.stBleSpec.dIbse[nAcpIndx].low
                    , m_pBTParamBand->stSpec.stBleSpec.dIbse[nAcpIndx].upp
                );
                RSLT_RANGE_CHECK(pTestResult->ACP.acp[nAcpIndx]
                    , m_pBTParamBand->stSpec.stBleSpec.dIbse[nAcpIndx].low
                    , m_pBTParamBand->stSpec.stBleSpec.dIbse[nAcpIndx].upp
                    , eItemRlst, SP_E_WCN_BT_ACP_FAIL);

                eAcpRslt |= eItemRlst;
            }
            pTestResult->ACP.indicator = (eAcpRslt == SP_OK ? TRUE : FALSE);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eAcpRslt;
            }
        }

        if (IS_BIT_SET(dwItemMask, BLE_MC))
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_MC);
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE MC:F1 meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->Modulation.f1avg
                , m_pBTParamBand->stSpec.stBleSpec.dF1Avg.low
                , m_pBTParamBand->stSpec.stBleSpec.dF1Avg.upp
            );
            RSLT_RANGE_CHECK(pTestResult->Modulation.f1avg
                , m_pBTParamBand->stSpec.stBleSpec.dF1Avg.low
                , m_pBTParamBand->stSpec.stBleSpec.dF1Avg.upp
                , eItemRlst, SP_E_WCN_BT_ACP_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }

            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE MC:F1 meas = %.3f, Low = %.3f",
                pTestResult->Modulation.f2max
                , m_pBTParamBand->stSpec.stBleSpec.dF2Max
            );
            RSLT_RANGE_CHECK(pTestResult->Modulation.f2max
                , m_pBTParamBand->stSpec.stBleSpec.dF2Max
                , 999.0
                , eItemRlst, SP_E_WCN_BT_MC_F2MAX_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }

            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE MC:F2/F1 Rate meas = %.3f, low limit = %.3f",
                pTestResult->Modulation.f2f1avg_rate
                , m_pBTParamBand->stSpec.stBleSpec.dRatio
            );
            RSLT_RANGE_CHECK(pTestResult->Modulation.f2f1avg_rate
                , m_pBTParamBand->stSpec.stBleSpec.dRatio
                , 999.0
                , eItemRlst, SP_E_WCN_BT_MC_F2TOF1_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
        }

        if (IS_BIT_SET(dwItemMask, BLE_CFD))
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_CFD);
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE CFD:f1avg meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->CarrierFreqDrift_BLE.fn
                , m_pBTParamBand->stSpec.stBleSpec.dFnMax[RF_PHY_1M].low
                , m_pBTParamBand->stSpec.stBleSpec.dFnMax[RF_PHY_1M].upp
            );
            RSLT_RANGE_CHECK(pTestResult->CarrierFreqDrift_BLE.fn
                , m_pBTParamBand->stSpec.stBleSpec.dFnMax[RF_PHY_1M].low
                , m_pBTParamBand->stSpec.stBleSpec.dFnMax[RF_PHY_1M].upp
                , eItemRlst, SP_E_WCN_BT_CFOD_FN_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }

            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE CFD:fn meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->CarrierFreqDrift_BLE.f0fn
                , -99
                , m_pBTParamBand->stSpec.stBleSpec.dF0FnMax
            );
            RSLT_RANGE_CHECK(pTestResult->CarrierFreqDrift_BLE.f0fn
                , m_pBTParamBand->stSpec.stBleSpec.dF0FnMax[RF_PHY_1M].low
                , m_pBTParamBand->stSpec.stBleSpec.dF0FnMax[RF_PHY_1M].upp
                , eItemRlst, SP_E_WCN_BT_CFOD_F0FN_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }

            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE CFD:f1f0 meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->CarrierFreqDrift_BLE.f1f0
                , m_pBTParamBand->stSpec.stBleSpec.dF1F0[RF_PHY_1M].low
                , m_pBTParamBand->stSpec.stBleSpec.dF1F0[RF_PHY_1M].upp
            );
            RSLT_RANGE_CHECK(pTestResult->CarrierFreqDrift_BLE.f1f0
                , m_pBTParamBand->stSpec.stBleSpec.dF1F0[RF_PHY_1M].low
                , m_pBTParamBand->stSpec.stBleSpec.dF1F0[RF_PHY_1M].upp
                , eItemRlst, SP_E_WCN_BT_CFOD_F1F0_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }

            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE CFD:fnfn5 meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->CarrierFreqDrift_BLE.fnfn5
                , m_pBTParamBand->stSpec.stBleSpec.dFnFn5[RF_PHY_1M].low
                , m_pBTParamBand->stSpec.stBleSpec.dFnFn5[RF_PHY_1M].upp
            );
            RSLT_RANGE_CHECK(pTestResult->CarrierFreqDrift_BLE.fnfn5
                , m_pBTParamBand->stSpec.stBleSpec.dFnFn5[RF_PHY_1M].low
                , m_pBTParamBand->stSpec.stBleSpec.dFnFn5[RF_PHY_1M].upp
                , eItemRlst, SP_E_WCN_BT_CFOD_FNFN5_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
        }

        if (IS_BIT_SET(dwItemMask, BLE_PER))
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_BER);
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE PER meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->BER.dAvgValue,
                0.0,
                m_pBTParamBand->stSpec.stBleSpec.dBer);

            RSLT_RANGE_CHECK(pTestResult->BER.dAvgValue,
                0.0,
                m_pBTParamBand->stSpec.stBleSpec.dBer
                , eItemRlst, SP_E_WCN_BT_PER_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
        }

        if (IS_BIT_SET(dwItemMask, BLE_RSSI))
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_BER);
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE RSSI meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->Rssi.dAvgValue,
                dBSLevel + m_pBTParamBand->stSpec.stBleSpec.dRssi.low,
                dBSLevel + m_pBTParamBand->stSpec.stBleSpec.dRssi.upp);

            RSLT_RANGE_CHECK(pTestResult->Rssi.dAvgValue,
                dBSLevel + m_pBTParamBand->stSpec.stBleSpec.dRssi.low,
                dBSLevel + m_pBTParamBand->stSpec.stBleSpec.dRssi.upp,
                eItemRlst, SP_E_WCN_BT_RSSI_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
        }

        return eCmpRlst;
    }

    SPRESULT CBTMeasBase::_JudgeBleExRslt(DWORD  dwItemMask, SPBT_RESULT_T * pTestResult, BT_RFPATH_ENUM ePath, BT_PACKET ePacketType, double dBSLevel)
    {
        auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
        if (NULL == pTestResult)
        {
            return SP_E_SPAT_INVALID_PARAMETER;
        }

        SPRESULT eCmpRlst = SP_OK;
        SPRESULT eItemRlst = SP_OK;

        if (IS_BIT_SET(dwItemMask, BLE_POWER))
        {
            double dTxpAvglow = 0.0;
            double dTxpAvgupp = 0.0;
            if (ANT_SINGLE == ePath)
            {
                dTxpAvglow = m_pBTParamBand->stSpec.stBleExSpec.dTxpAvg.low;
                dTxpAvgupp = m_pBTParamBand->stSpec.stBleExSpec.dTxpAvg.upp;
            }
            else
            {
                dTxpAvglow = m_pBTParamBand->stSpec.stBleExSpec.dTxpAvgShared.low;
                dTxpAvgupp = m_pBTParamBand->stSpec.stBleExSpec.dTxpAvgShared.upp;
            }

            m_pImpBase->SetRepairItem($REPAIR_ITEM_TXP);
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE Avg Power meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->Power.dAvgValue
                , dTxpAvglow
                , dTxpAvgupp
            );
            RSLT_RANGE_CHECK(pTestResult->Power.dAvgValue
                , dTxpAvglow
                , dTxpAvgupp
                , eItemRlst, SP_E_WCN_BT_TXP_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }


            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE Peak Power: meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->Power.dMaxValue - pTestResult->Power.dAvgValue
                , m_pBTParamBand->stSpec.stBleExSpec.dTxpPeak.low
                , m_pBTParamBand->stSpec.stBleExSpec.dTxpPeak.upp
            );
            RSLT_RANGE_CHECK(pTestResult->Power.dMaxValue - pTestResult->Power.dAvgValue
                , m_pBTParamBand->stSpec.stBleExSpec.dTxpPeak.low
                , m_pBTParamBand->stSpec.stBleExSpec.dTxpPeak.upp
                , eItemRlst, SP_E_WCN_BT_RTP_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
        }

        if (IS_BIT_SET(dwItemMask, BT_CW))
        {
            double dTxpAvglow = 0.0;
            double dTxpAvgupp = 0.0;
            if (ANT_SINGLE == ePath)
            {
                dTxpAvglow = m_pBTParamBand->stSpec.stBleExSpec.dTxpAvg.low;
                dTxpAvgupp = m_pBTParamBand->stSpec.stBleExSpec.dTxpAvg.upp;
            }
            else
            {
                dTxpAvglow = m_pBTParamBand->stSpec.stBleExSpec.dTxpAvgShared.low;
                dTxpAvgupp = m_pBTParamBand->stSpec.stBleExSpec.dTxpAvgShared.upp;
            }

            m_pImpBase->SetRepairItem($REPAIR_ITEM_TXP);
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE CW Avg Power meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->Power.dAvgValue
                , dTxpAvglow
                , dTxpAvgupp
            );
            RSLT_RANGE_CHECK(pTestResult->Power.dAvgValue
                , dTxpAvglow
                , dTxpAvgupp
                , eItemRlst, SP_E_WCN_BT_TXP_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
        }

        if (IS_BIT_SET(dwItemMask, BLE_ACP) && RF_PHY_S2 != ePacketType)
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_ACLR);
            SPRESULT eAcpRslt = SP_OK;
            for (int nAcpIndx = 0; nAcpIndx < 11; nAcpIndx++)
            {
                m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE ACP meas = %.3f, Low = %.3f, high = %.3f",
                    pTestResult->ACP.acp[nAcpIndx]
                    , m_pBTParamBand->stSpec.stBleExSpec.dIbse[nAcpIndx].low
                    , m_pBTParamBand->stSpec.stBleExSpec.dIbse[nAcpIndx].upp
                );
                RSLT_RANGE_CHECK(pTestResult->ACP.acp[nAcpIndx]
                    , m_pBTParamBand->stSpec.stBleExSpec.dIbse[nAcpIndx].low
                    , m_pBTParamBand->stSpec.stBleExSpec.dIbse[nAcpIndx].upp
                    , eItemRlst, SP_E_WCN_BT_ACP_FAIL);
                eAcpRslt |= eItemRlst;
            }
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eAcpRslt;
            }
            pTestResult->ACP.indicator = (eAcpRslt == SP_OK ? TRUE : FALSE);
        }

        if (IS_BIT_SET(dwItemMask, BLE_MC) && RF_PHY_S2 != ePacketType)
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_MC);
            double dF1AvgLowLimit = m_pBTParamBand->stSpec.stBleExSpec.dF1Avg.low;
            double dF1AvgUppLimit = m_pBTParamBand->stSpec.stBleExSpec.dF1Avg.upp;
            if (ePacketType == RF_PHY_2M)
            {
                dF1AvgLowLimit = dF1AvgLowLimit * 2.0;
                dF1AvgUppLimit = dF1AvgUppLimit * 2.0;
            }
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE MC:F1 avg meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->Modulation.f1avg
                , dF1AvgLowLimit
                , dF1AvgUppLimit
            );
            RSLT_RANGE_CHECK(pTestResult->Modulation.f1avg
                , dF1AvgLowLimit
                , dF1AvgUppLimit
                , eItemRlst, SP_E_WCN_BT_MC_F1AVG_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }


            if (ePacketType == RF_PHY_2M || ePacketType == RF_PHY_1M)
            {
                m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE MC:F2/F1 Rate meas = %.3f, low limit = %.3f",
                    pTestResult->Modulation.f2f1avg_rate
                    , m_pBTParamBand->stSpec.stBleExSpec.dRatio
                );
                RSLT_RANGE_CHECK(pTestResult->Modulation.f2f1avg_rate
                    , m_pBTParamBand->stSpec.stBleExSpec.dRatio
                    , 999.0
                    , eItemRlst, SP_E_WCN_BT_MC_F2TOF1_FAIL);
                if (SP_OK == eCmpRlst)
                {
                    eCmpRlst = eItemRlst;
                }
                double dF2MaxLowLimit = m_pBTParamBand->stSpec.stBleExSpec.dF2Max;
                if (ePacketType == RF_PHY_2M)
                {
                    dF2MaxLowLimit = dF2MaxLowLimit * 2.0;
                }
                m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE MC:F2 max meas = %.3f, Low = %.3f, high = %.3f",
                    pTestResult->Modulation.f2max
                    , dF2MaxLowLimit
                    , 999.0
                );
                RSLT_RANGE_CHECK(pTestResult->Modulation.f2max
                    , dF2MaxLowLimit
                    , 999.0
                    , eItemRlst, SP_E_WCN_BT_MC_F2MAX_FAIL);
                if (SP_OK == eCmpRlst)
                {
                    eCmpRlst = eItemRlst;
                }
            }
        }

        if (IS_BIT_SET(dwItemMask, BLE_CFD) && RF_PHY_S2 != ePacketType)
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_CFD);
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE CFD:Fn Max meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->CarrierFreqDrift_BLE.fn
                , m_pBTParamBand->stSpec.stBleExSpec.dFnMax[ePacketType].low
                , m_pBTParamBand->stSpec.stBleExSpec.dFnMax[ePacketType].upp
            );
            RSLT_RANGE_CHECK(pTestResult->CarrierFreqDrift_BLE.fn
                , m_pBTParamBand->stSpec.stBleExSpec.dFnMax[ePacketType].low
                , m_pBTParamBand->stSpec.stBleExSpec.dFnMax[ePacketType].upp
                , eItemRlst, SP_E_WCN_BT_CFOD_FN_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }

            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE CFD:F0-Fn meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->CarrierFreqDrift_BLE.f0fn
                , m_pBTParamBand->stSpec.stBleExSpec.dF0FnMax[ePacketType].low
                , m_pBTParamBand->stSpec.stBleExSpec.dF0FnMax[ePacketType].upp
            );
            RSLT_RANGE_CHECK(pTestResult->CarrierFreqDrift_BLE.f0fn
                , m_pBTParamBand->stSpec.stBleExSpec.dF0FnMax[ePacketType].low
                , m_pBTParamBand->stSpec.stBleExSpec.dF0FnMax[ePacketType].upp
                , eItemRlst, SP_E_WCN_BT_CFOD_F0FN_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }

            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE CFD:f3f0 meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->CarrierFreqDrift_BLE.f1f0
                , m_pBTParamBand->stSpec.stBleExSpec.dF1F0[ePacketType].low
                , m_pBTParamBand->stSpec.stBleExSpec.dF1F0[ePacketType].upp
            );
            RSLT_RANGE_CHECK(pTestResult->CarrierFreqDrift_BLE.f1f0
                , m_pBTParamBand->stSpec.stBleExSpec.dF1F0[ePacketType].low
                , m_pBTParamBand->stSpec.stBleExSpec.dF1F0[ePacketType].upp
                , eItemRlst, SP_E_WCN_BT_CFOD_F1F0_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }

            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLE CFD:|Fn-Fn-3|Max meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->CarrierFreqDrift_BLE.fnfn5
                , m_pBTParamBand->stSpec.stBleExSpec.dFnFn5[ePacketType].low
                , m_pBTParamBand->stSpec.stBleExSpec.dFnFn5[ePacketType].upp
            );
            RSLT_RANGE_CHECK(pTestResult->CarrierFreqDrift_BLE.fnfn5
                , m_pBTParamBand->stSpec.stBleExSpec.dFnFn5[ePacketType].low
                , m_pBTParamBand->stSpec.stBleExSpec.dFnFn5[ePacketType].upp
                , eItemRlst, SP_E_WCN_BT_CFOD_FNFN5_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
        }

        //MAX ICFT
        if (IS_BIT_SET(dwItemMask, FREQ_MAXICFR))
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_ICFT);
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: MAX BLE_ICFT meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->CarrierFreqDrift_BLE.dIcftMax
                , m_pBTParamBand->stSpec.stBleExSpec.dIcftMax[ePacketType].low
                , m_pBTParamBand->stSpec.stBleExSpec.dIcftMax[ePacketType].upp);

            RSLT_RANGE_CHECK(pTestResult->CarrierFreqDrift_BLE.dIcftMax
                , m_pBTParamBand->stSpec.stBleExSpec.dIcftMax[ePacketType].low
                , m_pBTParamBand->stSpec.stBleExSpec.dIcftMax[ePacketType].upp
                , eItemRlst, SP_E_WCN_BT_ICFR_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
        }
        //MIN ICFT
        if (IS_BIT_SET(dwItemMask, FREQ_MINICFR))
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_ICFT);
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: MIN BLE_ICFT meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->CarrierFreqDrift_BLE.dIcftMin
                , m_pBTParamBand->stSpec.stBleExSpec.dIcftMin[ePacketType].low
                , m_pBTParamBand->stSpec.stBleExSpec.dIcftMin[ePacketType].upp);

            RSLT_RANGE_CHECK(pTestResult->CarrierFreqDrift_BLE.dIcftMin
                , m_pBTParamBand->stSpec.stBleExSpec.dIcftMin[ePacketType].low
                , m_pBTParamBand->stSpec.stBleExSpec.dIcftMin[ePacketType].upp
                , eItemRlst, SP_E_WCN_BT_ICFR_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
        }

        //MAX CFD
        if (IS_BIT_SET(dwItemMask, FREQ_MAXCFD))
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_CFD);
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: MAX BLE_CFD meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->CarrierFreqDrift_BLE.dCfdMax
                , m_pBTParamBand->stSpec.stBleExSpec.dCfdMax[ePacketType].low
                , m_pBTParamBand->stSpec.stBleExSpec.dCfdMax[ePacketType].upp);

            RSLT_RANGE_CHECK(pTestResult->CarrierFreqDrift_BLE.dCfdMax
                , m_pBTParamBand->stSpec.stBleExSpec.dCfdMax[ePacketType].low
                , m_pBTParamBand->stSpec.stBleExSpec.dCfdMax[ePacketType].upp
                , eItemRlst, SP_E_WCN_BT_CFD_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
        }
        //MIN CFD
        if (IS_BIT_SET(dwItemMask, FREQ_MINCFD))
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_CFD);
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: MIN BLE_CFD meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->CarrierFreqDrift_BLE.dCfdMin
                , m_pBTParamBand->stSpec.stBleExSpec.dCfdMin[ePacketType].low
                , m_pBTParamBand->stSpec.stBleExSpec.dCfdMin[ePacketType].upp);

            RSLT_RANGE_CHECK(pTestResult->CarrierFreqDrift_BLE.dCfdMin
                , m_pBTParamBand->stSpec.stBleExSpec.dCfdMin[ePacketType].low
                , m_pBTParamBand->stSpec.stBleExSpec.dCfdMin[ePacketType].upp
                , eItemRlst, SP_E_WCN_BT_CFD_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
        }

        if (IS_BIT_SET(dwItemMask, BLE_PER))
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_BER);
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLEEX PER meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->BER.dAvgValue,
                0.0,
                m_pBTParamBand->stSpec.stBleExSpec.dBer);

            RSLT_RANGE_CHECK(pTestResult->BER.dAvgValue,
                0.0,
                m_pBTParamBand->stSpec.stBleExSpec.dBer
                , eItemRlst, SP_E_WCN_BT_PER_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
        }

        if (IS_BIT_SET(dwItemMask, BLE_RSSI))
        {
            m_pImpBase->SetRepairItem($REPAIR_ITEM_BER);
            m_pImpBase->LogFmtStrA(SPLOGLV_VERBOSE, "[BT]: BLEEX RSSI meas = %.3f, Low = %.3f, high = %.3f",
                pTestResult->Rssi.dAvgValue,
                dBSLevel + m_pBTParamBand->stSpec.stBleExSpec.dRssi.low,
                dBSLevel + m_pBTParamBand->stSpec.stBleExSpec.dRssi.upp);

            RSLT_RANGE_CHECK(pTestResult->Rssi.dAvgValue,
                dBSLevel + m_pBTParamBand->stSpec.stBleExSpec.dRssi.low,
                dBSLevel + m_pBTParamBand->stSpec.stBleExSpec.dRssi.upp,
                eItemRlst, SP_E_WCN_BT_RSSI_FAIL);
            if (SP_OK == eCmpRlst)
            {
                eCmpRlst = eItemRlst;
            }
        }

        return eCmpRlst;
    }

    void CBTMeasBase::ConfigTesterParam(BTMeasParamChan * pParam)
    {
        auto _function = SimpleAop(this->m_pImpBase, __FUNCTION__);
        m_stTester.dRefLvl = pParam->dRefLvl;
        m_stTester.dVsgLvl = pParam->dVsgLvl;
        m_stTester.ePacketType = pParam->ePacketType;
        m_stTester.ePattern = pParam->ePattern;
        m_stTester.eRfPort = CwcnUtility::BTGetAnt(pParam->ePath);
        m_stTester.nAvgCount = pParam->nAvgCount;
        m_stTester.nCh = pParam->nCh;
        m_stTester.eProto = m_pBTParamBand->eMode;
        //continues rx wave
        m_stTester.nTotalPackets = 0;
        m_stTester.nPacketLen = pParam->nPacketLen;
    }
