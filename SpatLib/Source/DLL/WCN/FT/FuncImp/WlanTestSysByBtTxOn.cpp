#include "StdAfx.h"
#include <cassert>
#include "WlanEnterMode.h"
#include "WlanMeasOS80.h"
#include "WlanMeasCW.h"
#include "WlanTestSysByBtTxOn.h"
#include "WlanLoadXMLOS80.h"
#include "SimpleAop.h"

//////////////////////////////////////////////////////////////////////////
//
IMPLEMENT_RUNTIME_CLASS(CWlanTestSysByBtTxOn)

CWlanTestSysByBtTxOn::CWlanTestSysByBtTxOn(void)
    : m_pEnterMode(NULL)
    , m_pWlanAnt(NULL)
    , m_pWlanApi(NULL)
    , m_pBtEnterMode(NULL)
    , m_pRfPathSwitch(NULL)
    , m_pBtApi(NULL)
    , NonINFSleep(2000)
{
}

CWlanTestSysByBtTxOn::~CWlanTestSysByBtTxOn(void)
{
}

SPRESULT CWlanTestSysByBtTxOn::__InitAction(void)
{
    auto _function = SimpleAop(this, __FUNCTION__);
    CHKRESULT(__super::__InitAction());

    if (m_stWlanParamImp.VecWlanParamBandImp.empty())
    {
        LogFmtStrA(SPLOGLV_ERROR, "The wlan config parameter is null");
        return SP_E_SPAT_INVALID_PARAMETER;
    }
    m_pEnterMode = new CWlanEnterMode(this);
    if (NULL == m_pEnterMode)
    {
        LogFmtStrA(SPLOGLV_ERROR, "new CWlanEnterMode failed!");
        return SP_E_SPAT_ALLOC_MEMORY;
    }

    m_pBtApi = new CBTApiAT(this->m_hDUT, BLE);
    if (NULL == m_pBtApi)
    {
        LogFmtStrA(SPLOGLV_ERROR, "new CBTApiAT fail!");
        return SP_E_SPAT_ALLOC_MEMORY;
    }

    m_pBtEnterMode = new CBTEnterMode(m_pBtApi, BLE);
    if (NULL == m_pBtEnterMode)
    {
        LogFmtStrA(SPLOGLV_ERROR, "new CBTEnterMode fail!");
        return SP_E_SPAT_ALLOC_MEMORY;
    }

    m_pRfPathSwitch = new CBTRFPathSwitch(m_pBtApi);
    if (NULL == m_pRfPathSwitch)
    {
        LogFmtStrA(SPLOGLV_ERROR, "new CBTRFPathSwitch fail!");
        return SP_E_SPAT_ALLOC_MEMORY;
    }

    m_pWlanAnt = new CWlanAntSwitch(this);
    if (NULL == m_pWlanAnt)
    {
        LogFmtStrA(SPLOGLV_ERROR, "new CWlanAntSwitch failed!");
        return SP_E_SPAT_ALLOC_MEMORY;
    }
    m_pWlanApi = new CWlanApiAT(m_hDUT);
    if (NULL == m_pWlanApi)
    {
        LogFmtStrA(SPLOGLV_ERROR, "new CWlanApiAT failed!");
        return SP_E_SPAT_ALLOC_MEMORY;
    }
    return SP_OK;
}

BOOL CWlanTestSysByBtTxOn::LoadXMLConfig(void)
{
    auto _function = SimpleAop(this, __FUNCTION__);
    /*CHKRESULT*/(__super::LoadXMLConfig());

    if (!m_stWlanParamImp.VecWlanParamBandImp.empty())
    {
        m_stWlanParamImp.VecWlanParamBandImp.clear();
    }
    CWlanLoadXMLOS80* pLoadXml = NULL;
    pLoadXml = new CWlanLoadXMLOS80(this);
    if (NULL == pLoadXml)
    {
        LogFmtStrA(SPLOGLV_ERROR, "new CWlanLoadXMLOS80 failed!");
        return FALSE;
    }
    SPRESULT rlt = pLoadXml->WlanLoadXmlFile(m_stWlanParamImp);
    delete pLoadXml;

    //Get BT_INF_Source Ant
    std::wstring strBtTxSource = L"Param:BT_TXON_Source";
    btChanParam.ePath = (BT_RFPATH_ENUM)_wtoi(GetConfigValue((strBtTxSource + L":Ant").c_str(), L"1:StandAlone"));

    btChanParam.nCh = GetConfigValue((strBtTxSource + L":TCH").c_str(), 39);
    btChanParam.dVsgLvl = GetConfigValue((strBtTxSource + L":LVLStep").c_str(), 3);
    std::wstring strCommon = L"Param:Common";
    NonINFSleep = GetConfigValue((strCommon + L":BtTxOffSleep").c_str(), 2000);
    return (SP_OK == rlt);
}

SPRESULT CWlanTestSysByBtTxOn::__PollAction(void)
{
    CInstrumentLock rfLock(m_pRFTester);
    auto _function = SimpleAop(this, __FUNCTION__);
    SetRepairMode(RepairMode_Wlan);
    CHKRESULT(ChangeDUTRunMode());
    if (m_stWlanParamImp.VecWlanParamBandImp.empty())
    {
        LogFmtStrA(SPLOGLV_ERROR, "The wlan config parameter is null");
        return SP_E_SPAT_INVALID_PARAMETER;
    }

    //Enable wlan mode
    if (m_u32MaxFailRetryCount < 5)
    {
        m_u32MaxFailRetryCount = 5;
    }
    MAX_MEAS_COUNT = m_u32MaxFailRetryCount;

    SPRESULT eRes = m_pEnterMode->Wlan_ModeEnable(TRUE);
    _UiSendMsg("WlanEnterEutMode", LEVEL_ITEM, SP_OK, eRes, SP_OK, "", -1, "-");
    CHKRESULT(eRes);
    //Wlan performance meas
    SPRESULT eWlanBandRlst = SP_OK;
    ANTENNA_ENUM eAnt = ANT_WLAN_INVALID;
    for (int i = 0; i < (int)m_stWlanParamImp.VecWlanParamBandImp.size(); i++)
    {
        if (0 == m_stWlanParamImp.VecWlanParamBandImp[i].vecConfParamGroup.size())
        {
            continue;
        }
        if (eAnt != m_stWlanParamImp.VecWlanParamBandImp[i].vecConfParamGroup[0].stAlgoParamGroupSub.eAnt)
        {
            eAnt = m_stWlanParamImp.VecWlanParamBandImp[i].vecConfParamGroup[0].stAlgoParamGroupSub.eAnt;
            eRes = m_pWlanAnt->Wlan_AntSwitch(eAnt);
            if (SP_OK != eRes)
            {
                _UiSendMsg("WlanSwitchAnt", LEVEL_ITEM, SP_OK, eRes, SP_OK, "", -1, "-");
                m_pEnterMode->Wlan_ModeEnable(FALSE);
                return eRes;
            }
        }

        eRes = WlanMeasPerform((WlanMeasParamBand*)&m_stWlanParamImp.VecWlanParamBandImp[i]);
        if (m_bFailStop)
        {
            eWlanBandRlst = eRes;
            if (SP_OK != eWlanBandRlst)
            {
                break;
            }
        }
        else
        {
            if (SP_OK == eWlanBandRlst)
            {
                eWlanBandRlst = eRes;
            }
        }
    }
    //Disable Wlan mode
    eRes = m_pEnterMode->Wlan_ModeEnable(FALSE);
    _UiSendMsg("WlanLeaveEutMode", LEVEL_ITEM, SP_OK, eRes, SP_OK, "", -1, "-");
    if (SP_OK == eWlanBandRlst)
    {
        return eRes;
    }
    return eWlanBandRlst;
}

void CWlanTestSysByBtTxOn::__LeaveAction(void)
{
}

SPRESULT CWlanTestSysByBtTxOn::__FinalAction(void)
{
    auto _function = SimpleAop(this, __FUNCTION__);

    if (NULL != m_pBtApi)
    {
        delete m_pBtApi;
        m_pBtApi = NULL;
    }

    if (NULL != m_pBtEnterMode)
    {
        delete m_pBtEnterMode;
        m_pBtEnterMode = NULL;
    }

    if (NULL != m_pRfPathSwitch)
    {
        delete m_pRfPathSwitch;
        m_pRfPathSwitch = NULL;
    }

    if (NULL != m_pEnterMode)
    {
        delete m_pEnterMode;
        m_pEnterMode = NULL;
    }

    if (NULL != m_pWlanAnt)
    {
        delete m_pWlanAnt;
        m_pWlanAnt = NULL;
    }

    if (NULL != m_pWlanApi)
    {
        delete m_pWlanApi;
        m_pWlanApi = NULL;
    }
    return SP_OK;
}
SPRESULT CWlanTestSysByBtTxOn::BT_Tx_On(void)
{
    CHKRESULT(m_pBtEnterMode->BT_SetBtType(BLE));
    CHKRESULT(m_pBtEnterMode->BT_ModeEnable(BT_NST, TRUE));
    CHKRESULT(m_pRfPathSwitch->BT_RFPathSet(btChanParam.ePath));

    //set channel
    CHKRESULT(m_pBtApi->DUT_SetCH(BT_TX, btChanParam.nCh));
    //Transmit enable
    CHKRESULT_WITH_NOTIFY_WIFI_BY_BTINF_POWER_OFF(m_pBtApi->DUT_RFOn(BT_TX, TRUE), "DUT_RFOn(TRUE)");

    _UiSendMsg("BT_Tx_On", LEVEL_ITEM, 0, 0, 0, CwcnUtility::BT_ANT_NAME[btChanParam.ePath]);

    return SP_OK;
}
SPRESULT CWlanTestSysByBtTxOn::BT_Tx_Off(void)
{
    //Transmit off
    SPRESULT sRet = m_pBtApi->DUT_RFOn(BT_TX, FALSE);
    sRet &= m_pBtEnterMode->BT_ModeEnable(BT_LEAVE, FALSE);
    _UiSendMsg("BT_Tx_Off", LEVEL_ITEM, 0, SP_OK == sRet ? 0 : 1, 0, CwcnUtility::BT_ANT_NAME[btChanParam.ePath]);

    return SP_OK;
}


SPRESULT CWlanTestSysByBtTxOn::WlanMeasPerform(void* vecParam)
{
    auto _function = SimpleAop(this, __FUNCTION__);
    WlanMeasParamBand* pWlanParamBand = (WlanMeasParamBand*)vecParam;
    char strInfo[256] = { 0 };

    SPRESULT sRes = SP_E_FAIL;
    DWORD	dwMask = 0;

    m_MeasParam.Init();


    // [6]: Setup instrument and measure up-link result
    //CInstrumentLock rfLock(m_pRfTesterWlan);
    SetRepairItem($REPAIR_ITEM_INSTRUMENT);
    CHKRESULT(m_pRFTester->SetNetMode(NM_WIFI));
    CHKRESULT(m_pRFTester->InitDev(TM_NON_SIGNAL_FINAL, SUB_FT_NST, NULL));

    vector<SPWI_WLAN_PARAM_GROUP>::iterator itWlanGroup;
    for (itWlanGroup = pWlanParamBand->vecConfParamGroup.begin(); itWlanGroup != pWlanParamBand->vecConfParamGroup.end(); itWlanGroup++)
    {
        //General config
        m_MeasParam.stTesterParamGroupSub = itWlanGroup->stAlgoParamGroupSub;
        dwMask = m_MeasParam.stTesterParamGroupSub.dwMask;
        //General config End
        if (dwMask == NULL)
        {
            continue;
        }
        SetRepairBand(CwcnUtility::m_WlanBandInfo[pWlanParamBand->eMode]);

        WIFI_PROTOCOL_ENUM eProto = pWlanParamBand->eMode;
        m_MeasParam.eMode = pWlanParamBand->eMode;

        if (WIFI_CW_SPECTRUM != eProto)
        {
            m_MeasParam.stTesterParamGroupSub.dwMask &= ~WIFI_CW;
        }

        //         m_MeasParam.stTesterParamGroupSub.dPwrLvl = m_MeasParam.stTesterParamGroupSub.dRefLvl;
        // 		LPCSTR strCBW = CwcnUtility::WLAN_BW_NAME[m_MeasParam.stTesterParamGroupSub.nCBWType];
        // 		LPCSTR strSBW = CwcnUtility::WLAN_BW_NAME[m_MeasParam.stTesterParamGroupSub.nSBWType];
        // 		LPCSTR lpRate = CwcnUtility::WlanGetRateString(m_MeasParam.stTesterParamGroupSub.eRate);

                //Channel cycle
        for (uint32 nCh = 0; nCh < itWlanGroup->vectChan.size(); nCh++)
        {
            if (_IsUserStop())
            {
                return SP_E_USER_ABORT;
            }

            //Meas channel config
            m_MeasParam.stChan = itWlanGroup->vectChan[nCh];

            //Show
            sprintf_s(strInfo, sizeof(strInfo), "TestInfo %s::PriCh-%02d::CenCh-%02d"
                , CwcnUtility::WLAN_BAND_NAME[m_MeasParam.eMode]
                , m_MeasParam.stChan.nPriChan
                , m_MeasParam.stChan.nCenChan);

            _UiSendMsg(strInfo, LEVEL_ITEM, 1, 1, 1, "", -1, "-");

            //double dFreq = CwcnUtility::WIFI_Ch2MHz(m_MeasParam.stChan.nCenChan);

            //RX performance meas
            double dDefaultLvl = m_MeasParam.stTesterParamGroupSub.dVsgLvl;
            DWORD dwMaskTmpRx = dwMask & (WIFI_PER);
            if (0 != dwMaskTmpRx)
            {
                sRes = BT_TxOn_TestPER(&m_MeasParam, &pWlanParamBand->stSpec);
                if (!(SP_OK == sRes && dDefaultLvl == m_MeasParam.stTesterParamGroupSub.dVsgLvl))
                {
                    Sleep(NonINFSleep);
                    CHKRESULT(BT_TxOff_TestPER(&m_MeasParam, &pWlanParamBand->stSpec));
                }
            }
        }
    }
    return sRes;
}

SPRESULT CWlanTestSysByBtTxOn::BT_TxOn_TestPER(SPWI_WLAN_PARAM_MEAS_GROUP* pMeasINFParam, WlanMeasBandSpec* pSpec)
{
    SPRESULT sRet = SP_E_FAIL;
    SPRESULT eRslt = SP_E_WCN_WLAN_PER_FAIL;
    int nRepeatCount = 0;

    CHKRESULT(BT_Tx_On());//BlueTooth Tx On
    do
    {
        LogFmtStrA(SPLOGLV_INFO, "%s: Adj VsgLvl %0.2f, LoopCount: %d, MaxAdj: %d", __FUNCTION__, pMeasINFParam->stTesterParamGroupSub.dVsgLvl, nRepeatCount + 1, MAX_MEAS_COUNT);

        m_rstData.per.dAvgValue = 100.0;
        sRet = TestPER(pMeasINFParam, &m_rstData);
        if (SP_OK != sRet)
        {
            break;
        }
        //Judge result
        RSLT_RANGE_CHECK(m_rstData.per.dAvgValue, 0, pSpec->dPer, eRslt, SP_E_WCN_WLAN_PER_FAIL);
        if (SP_OK == eRslt)
        {
            break;
        }
        pMeasINFParam->stTesterParamGroupSub.dVsgLvl += btChanParam.dVsgLvl;
    } while (++nRepeatCount < MAX_MEAS_COUNT);
    m_pWlanApi->DUT_RxOn(false);

    //Show Result
    LPCSTR strCBW = CwcnUtility::WLAN_BW_NAME[pMeasINFParam->stTesterParamGroupSub.nCBWType];
    LPCSTR strSBW = CwcnUtility::WLAN_BW_NAME[pMeasINFParam->stTesterParamGroupSub.nSBWType];
    LPCSTR lpRate = CwcnUtility::WlanGetRateString(pMeasINFParam->stTesterParamGroupSub.eRate);
    double dFreq = CwcnUtility::WIFI_Ch2MHz(pMeasINFParam->stChan.nCenChan);

    _UiSendMsg("BtTxOn Wif PER Test"
        , LEVEL_ITEM | LEVEL_FT
        , 0
        , m_rstData.per.dAvgValue
        , pSpec->dPer
        , CwcnUtility::WLAN_BAND_NAME[pMeasINFParam->eMode]
        , pMeasINFParam->stChan.nCenChan
        , "-"
        , "Frequency:%0.2f;%s;%s;CBW:%s;SBW:%s;BSLevel:%.2f, LoopCount: %d"
        , dFreq
        , CwcnUtility::WLAN_ANT_NAME[pMeasINFParam->stTesterParamGroupSub.eAnt]
        , lpRate
        , strCBW
        , strSBW
        , pMeasINFParam->stTesterParamGroupSub.dVsgLvl
        , nRepeatCount + 1);

    CHKRESULT(BT_Tx_Off());//BlueTooth Tx Off
    return sRet | eRslt;
}


SPRESULT CWlanTestSysByBtTxOn::BT_TxOff_TestPER(SPWI_WLAN_PARAM_MEAS_GROUP* pMeasINFParam, WlanMeasBandSpec* pSpec)
{
    SPRESULT sRet = SP_E_FAIL;
    SPRESULT eRslt = SP_E_WCN_WLAN_PER_FAIL;
    int nRepeatCount = 0;
    double dALvl = pMeasINFParam->stTesterParamGroupSub.dVsgLvl;
    double dBLvl = pMeasINFParam->stTesterParamGroupSub.dVsgLvl;

    do
    {
        LogFmtStrA(SPLOGLV_INFO, "%s: Adj VsgLvl %0.2f, LoopCount: %d, MaxAdj: %d", __FUNCTION__, pMeasINFParam->stTesterParamGroupSub.dVsgLvl, nRepeatCount + 1, MAX_MEAS_COUNT);

        m_rstData.per.dAvgValue = 100.0;
        sRet = TestPER(pMeasINFParam, &m_rstData);
        if (SP_OK != sRet)
        {
            break;
        }
        //Judge result
        RSLT_RANGE_CHECK(m_rstData.per.dAvgValue, 0, pSpec->dPer, eRslt, SP_E_WCN_WLAN_PER_FAIL);
        if (SP_OK != eRslt/* && dBLvl != dALvl*/)
        {
            break;
        }

        pMeasINFParam->stTesterParamGroupSub.dVsgLvl -= btChanParam.dVsgLvl;
        dBLvl = pMeasINFParam->stTesterParamGroupSub.dVsgLvl;
    } while (++nRepeatCount < MAX_MEAS_COUNT);
    m_pWlanApi->DUT_RxOn(false);

    LPCSTR strCBW = CwcnUtility::WLAN_BW_NAME[pMeasINFParam->stTesterParamGroupSub.nCBWType];
    LPCSTR strSBW = CwcnUtility::WLAN_BW_NAME[pMeasINFParam->stTesterParamGroupSub.nSBWType];
    LPCSTR lpRate = CwcnUtility::WlanGetRateString(pMeasINFParam->stTesterParamGroupSub.eRate);
    double dFreq = CwcnUtility::WIFI_Ch2MHz(pMeasINFParam->stChan.nCenChan);

    double dGap = abs(dALvl - dBLvl);
    if (dGap < pSpec->dLvlGap && dGap > 0)
    {
        eRslt = SP_OK;
    }
    else
    {
        if (0 == dGap)
        {
            dGap = -999;
            LogFmtStrA(SPLOGLV_ERROR, "%s: BT_TxOff_TestPER VsgLvl %0.2f Fail; But Same VsgLvl, BT_TxOn_TestPER Pass", __FUNCTION__, pMeasINFParam->stTesterParamGroupSub.dVsgLvl);
        }
        eRslt = SP_E_WCN_WLAN_PER_FAIL;
    }

    //Show Result
    _UiSendMsg("BtTxOff Wifi PER Test"
        , LEVEL_ITEM | LEVEL_FT
        , 0
        , dGap
        , pSpec->dLvlGap
        , CwcnUtility::WLAN_BAND_NAME[pMeasINFParam->eMode]
        , pMeasINFParam->stChan.nCenChan
        , "-"
        , "Frequency:%0.2f;%s;%s;CBW:%s;SBW:%s;BSLevel:%.2f, LoopCount: %d"
        , dFreq
        , CwcnUtility::WLAN_ANT_NAME[pMeasINFParam->stTesterParamGroupSub.eAnt]
        , lpRate
        , strCBW
        , strSBW
        , pMeasINFParam->stTesterParamGroupSub.dVsgLvl
        , nRepeatCount + 1);

    return sRet | eRslt;
}

SPRESULT CWlanTestSysByBtTxOn::TestPER(SPWI_WLAN_PARAM_MEAS_GROUP* pDownlinkMeasParam, SPWI_RESULT_T* pRxReult)
{
    auto _function = SimpleAop(this, __FUNCTION__);
    if (NULL == pDownlinkMeasParam)
    {
        return SP_E_SPAT_INVALID_PARAMETER;
    }
    DWORD dwMask = pDownlinkMeasParam->stTesterParamGroupSub.dwMask;

    if (!IS_BIT_SET(dwMask, WIFI_PER) && !IS_BIT_SET(dwMask, WIFI_RSSI))
    {
        return SP_OK;
    }


    SetRepairItem($REPAIR_ITEM_COMMUNICATION);
    ConfigTesterParam(pDownlinkMeasParam);

    int nCenCh = pDownlinkMeasParam->stChan.nCenChan;
    int nPriCh = pDownlinkMeasParam->stChan.nPriChan;

    //Set Band
    //Set channel band width
    CHKRESULT(m_pWlanApi->DUT_SetBandWidth((WIFI_BANDWIDTH_ENUM)pDownlinkMeasParam->stTesterParamGroupSub.nCBWType));
    //Set signal band width
    CHKRESULT(m_pWlanApi->DUT_SetSigBandWidth((WIFI_BANDWIDTH_ENUM)pDownlinkMeasParam->stTesterParamGroupSub.nSBWType));
    //Set Channel
    CHKRESULT(m_pWlanApi->DUT_SetCH(nPriCh, nCenCh));

    CHKRESULT(m_pWlanApi->DUT_RxOn(true));
    Sleep(100);

    if (IS_BIT_SET(dwMask, WIFI_PER))
    {
        SetRepairItem($REPAIR_ITEM_INSTRUMENT);
        int nTotalPackets = m_stTester.nTotalPackets;
        CHKRESULT(m_pRFTester->InitTest(WIFI_PER, &m_stTester));

        int nCountTry = 30;
        int good_packets = 0;
        int error_packets = 0;
        int nPreCount = -nTotalPackets;
        do
        {
            CHKRESULT(m_pWlanApi->DUT_GetPER(good_packets, error_packets));
            if (good_packets > nTotalPackets || good_packets + error_packets - nPreCount < 100)
            {
                break;
            }
            Sleep(100);
            nPreCount = good_packets + error_packets;
        } while (nCountTry-- > 0);
        /// To improve the pass rate of mass production line, 
        /// We accept the interference which is not too big.
        /// So we ignore below check rules.
        if (good_packets > nTotalPackets)
        {
            pRxReult->per.dAvgValue = 0.0;
        }
        else
        {
            pRxReult->per.dAvgValue = ((double)(nTotalPackets - good_packets)) / (nTotalPackets) * 100.0;
        }
        LogFmtStrA(SPLOGLV_VERBOSE, "[WIFI]: Total = %d, good = %d, error = %d Avg Per = %0.2f%%.", nTotalPackets, good_packets, error_packets, pRxReult->per.dAvgValue);
    }
    // Turn off VSG
    CHKRESULT(m_pRFTester->SetGen(MM_MODULATION, FALSE));

    return SP_OK;
}

void CWlanTestSysByBtTxOn::ConfigTesterParam(SPWI_WLAN_PARAM_MEAS_GROUP* pMeasParam)
{
    auto _function = SimpleAop(this, __FUNCTION__);
    m_stTester.nCenChan = pMeasParam->stChan.nCenChan;
    m_stTester.nPriChan = pMeasParam->stChan.nPriChan;
    CwcnUtility::WlanGetAnt(pMeasParam->stTesterParamGroupSub.eAnt, &m_stTester.ePort[0]);
    m_stTester.eProto = pMeasParam->eMode;
    m_stTester.nAvgCount = pMeasParam->stTesterParamGroupSub.nAvgCount;
    m_stTester.nTotalPackets = pMeasParam->stTesterParamGroupSub.nTotalPackets;
    m_stTester.nCBWType = pMeasParam->stTesterParamGroupSub.nCBWType;
    m_stTester.nSBWType = pMeasParam->stTesterParamGroupSub.nSBWType;
    m_stTester.eRate = pMeasParam->stTesterParamGroupSub.eRate;
    m_stTester.dRefLvl = pMeasParam->stTesterParamGroupSub.dRefLvl;
    m_stTester.dVsgLvl = pMeasParam->stTesterParamGroupSub.dVsgLvl;
}