#include "StdAfx.h"
#include "CheckCaliMMIFlagUIS8910.h"
#include "ModeSwitch.h"
#include <atlconv.h>
//#include <io.h>
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CCheckCaliMMIFlagUIS8910)

static char *szNativeMMIType[] = {("Whole Phone Native MMI Flag"), ("PCBA Native MMI Flag"), ("BBAT Native MMI Flag")};

#define GSMFLAG(mask) (IS_BIT_SET(mask, TEST_FLAG_CALI_GSM_AFC_UIS8910)         \
                ||IS_BIT_SET(mask, TEST_FLAG_CALI_GSM850_AGC_UIS8910)           \
                ||IS_BIT_SET(mask, TEST_FLAG_CALI_GSM850_APC_UIS8910)           \
                ||IS_BIT_SET(mask, TEST_FLAG_CALI_EGSM_AGC_UIS8910)             \
                ||IS_BIT_SET(mask, TEST_FLAG_CALI_EGSM_APC_UIS8910)             \
                ||IS_BIT_SET(mask, TEST_FLAG_CALI_DCS_AGC_UIS8910)              \
                ||IS_BIT_SET(mask, TEST_FLAG_CALI_DCS_APC_UIS8910)              \
                ||IS_BIT_SET(mask, TEST_FLAG_CALI_PCS_AGC_UIS8910)              \
                ||IS_BIT_SET(mask, TEST_FLAG_CALI_PCS_APC_UIS8910)              \
                ||IS_BIT_SET(mask, TEST_FLAG_FINAL_GSM_UIS8910)                 \
                ||IS_BIT_SET(mask, TEST_FLAG_ANTENNA_GSM_UIS8910)               \
                ||IS_BIT_SET(mask, TEST_FLAG_FINAL2_GSM_UIS8910))


#define LTEFLAG(mask) (IS_BIT_SET(mask, CALI_FLAG_LTE_AFC_UIS8910)             \
                ||IS_BIT_SET(mask, CALI_FlAG_LTE_AGC_TDD_UIS8910)              \
                ||IS_BIT_SET(mask, CALI_FLAG_LTE_APC_TDD_UIS8910)              \
                ||IS_BIT_SET(mask, CALI_FLAG_LTE_TX_PDT_UIS8910)               \
                ||IS_BIT_SET(mask, CALI_FlAG_LTE_AGC_FDD_UIS8910)              \
                ||IS_BIT_SET(mask, CALI_FLAG_LTE_APC_FDD_UIS8910)              \
                ||IS_BIT_SET(mask, CALI_FLAG_LTE_TX_DC_UIS8910)                \
                ||IS_BIT_SET(mask, CALI_FLAG_LTE_TX_DROOP_UIS8910)             \
                ||IS_BIT_SET(mask, CALI_FLAG_LTE_RX_IRR_UIS8910)              \
                ||IS_BIT_SET(mask, FINAL_FLAG_LTE_UIS8910)                     \
                ||IS_BIT_SET(mask, ANT_FLAG_LTE_UIS8910)                       \
                ||IS_BIT_SET(mask, FINAL2_FLAG_LTE_UIS8910))

#define WIFIFLAG(mask) (IS_BIT_SET(mask, WIFI_CALI_LTE_FLAG_UIS8910)         \
                ||IS_BIT_SET(mask, WIFI_CALI_WIFI_FLAG_UIS8910)              \
                ||IS_BIT_SET(mask, WIFI_NST_FLAG_UIS8910)                    \
                ||IS_BIT_SET(mask, WIFI_ANT_FLAG_UIS8910)                    \
                ||IS_BIT_SET(mask, WIFI_NST_FLAG2_UIS8910))

///
CCheckCaliMMIFlagUIS8910::CCheckCaliMMIFlagUIS8910(void)
: m_Uetype(0),
m_bCheckWholeMMI(FALSE),
m_bCheckBBATMMI(FALSE),
m_bCheckPCBAMMI(FALSE),
m_pCalFlag(NULL),
m_bCheckUID(FALSE),
m_nGSMFlag(0),
m_nLTEflag(0),
m_nWIFIflag(0)
/*m_bCheckAPKMMI(FALSE)*/
{
    memset(m_wifi_UeInfo, 0, 20 * sizeof(int));
    m_arrTestItem.clear();
}

CCheckCaliMMIFlagUIS8910::FlagInfo  CCheckCaliMMIFlagUIS8910::m_FlagInfo[FLAG_INDEX_MAX] = {
    {GSM_CALI_AFC, 	   FLAGMODE_GSM,  L"GSM_CALI_AFC",         "GSM_CALI_AFC",     TEST_FLAG_CALI_GSM_AFC_UIS8910,     GSM_CALI_AFC},
    {GSM850_CALI_AGC,  FLAGMODE_GSM,  L"GSM850_CALI_AGC",      "GSM850_CALI_AGC",  TEST_FLAG_CALI_GSM850_AGC_UIS8910,  GSM850_CALI_AGC},
    {GSM850_CALI_APC,  FLAGMODE_GSM,  L"GSM850_CALI_APC",      "GSM850_CALI_APC",  TEST_FLAG_CALI_GSM850_APC_UIS8910,  GSM850_CALI_APC},
    {EGSM_CALI_AGC,    FLAGMODE_GSM,  L"EGSM_CALI_AGC",        "EGSM_CALI_AGC",    TEST_FLAG_CALI_EGSM_AGC_UIS8910,    EGSM_CALI_AGC},
    {EGSM_CALI_APC,    FLAGMODE_GSM,  L"EGSM_CALI_APC",        "EGSM_CALI_APC",    TEST_FLAG_CALI_EGSM_APC_UIS8910,    EGSM_CALI_APC},
    {DCS_CALI_AGC,     FLAGMODE_GSM,  L"DCS_CALI_AGC",         "DCS_CALI_AGC",     TEST_FLAG_CALI_DCS_AGC_UIS8910,     DCS_CALI_AGC},
    {DCS_CALI_APC, 	   FLAGMODE_GSM,  L"DCS_CALI_APC",         "DCS_CALI_APC",     TEST_FLAG_CALI_DCS_APC_UIS8910,     DCS_CALI_APC},
    {PCS_CALI_AGC,     FLAGMODE_GSM,  L"PCS_CALI_AGC",         "PCS_CALI_AGC",     TEST_FLAG_CALI_PCS_AGC_UIS8910,     PCS_CALI_AGC},
    {PCS_CALI_APC,     FLAGMODE_GSM,  L"PCS_CALI_APC",         "PCS_CALI_APC",     TEST_FLAG_CALI_PCS_APC_UIS8910,     PCS_CALI_APC},
    {GSM_FINAL,        FLAGMODE_GSM,  L"GSM_FINAL",            "GSM_FINAL",        TEST_FLAG_FINAL_GSM_UIS8910,        GSM_FINAL},
    {GSM_ANTENNA,      FLAGMODE_GSM,  L"GSM_ANTENNA",          "GSM_ANTENNA",      TEST_FLAG_ANTENNA_GSM_UIS8910,      GSM_ANTENNA},
    {GSM_FINAL2,       FLAGMODE_GSM,  L"GSM_FINAL2",           "GSM_FINAL2",       TEST_FLAG_FINAL2_GSM_UIS8910,       GSM_FINAL2},
    {LTE_CALI_AFC,     FLAGMODE_LTE,  L"LTE_CALI_AFC",         "LTE_CALI_AFC",     CALI_FLAG_LTE_AFC_UIS8910,          0},
    {LTE_CALI_AGC_TDD, FLAGMODE_LTE,  L"LTE_CALI_AGC_TDD",     "LTE_CALI_AGC_TDD", CALI_FlAG_LTE_AGC_TDD_UIS8910,      1},
    {LTE_CALI_APC_TDD, FLAGMODE_LTE,  L"LTE_CALI_APC_TDD",     "LTE_CALI_APC_TDD", CALI_FLAG_LTE_APC_TDD_UIS8910,      2},
    {LTE_CALI_TX_PDT,  FLAGMODE_LTE,  L"LTE_CALI_TX_PDT",      "LTE_CALI_TX_PDT",  CALI_FLAG_LTE_TX_PDT_UIS8910,       3},
    {LTE_CALI_AGC_FDD, FLAGMODE_LTE,  L"LTE_CALI_AGC_FDD",     "LTE_CALI_AGC_FDD", CALI_FlAG_LTE_AGC_FDD_UIS8910,      4},
    {LTE_CALI_APC_FDD, FLAGMODE_LTE,  L"LTE_CALI_APC_FDD",     "LTE_CALI_APC_FDD", CALI_FLAG_LTE_APC_FDD_UIS8910,      5},
    {LTE_CALI_TX_DC,   FLAGMODE_LTE,  L"LTE_CALI_TX_DC",       "LTE_CALI_TX_DC",   CALI_FLAG_LTE_TX_DC_UIS8910,        6},
    {LTE_CALI_TX_DROOP,FLAGMODE_LTE,  L"LTE_CALI_TX_DROOP",    "LTE_CALI_TX_DROOP",CALI_FLAG_LTE_TX_DROOP_UIS8910,     7},
    {LTE_FINAL,        FLAGMODE_LTE,  L"LTE_FINAL",            "LTE_FINAL",        FINAL_FLAG_LTE_UIS8910,             8},
    {LTE_ANTENNA,      FLAGMODE_LTE,  L"LTE_ANTENNA",          "LTE_ANTENNA",      ANT_FLAG_LTE_UIS8910,               9},
    {LTE_FINAL2,       FLAGMODE_LTE,  L"LTE_FINAL2",           "LTE_FINAL2",       FINAL2_FLAG_LTE_UIS8910,            11},
    {LTE_CALI_RX_IRR,  FLAGMODE_LTE,  L"LTE_CALI_RX_IRR",      "LTE_CALI_RX_IRR",  CALI_FLAG_LTE_RX_IRR_UIS8910,       12},
    {WIFI_CALI_LTE,    FLAGMODE_WIFI, L"WIFI_CALI_LTE",        "WIFI_CALI_LTE",    WIFI_CALI_LTE_FLAG_UIS8910,          0},
    {WIFI_CALI_WIFI,   FLAGMODE_WIFI, L"WIFI_CALI_WIFI",       "WIFI_CALI_WIFI",   WIFI_CALI_WIFI_FLAG_UIS8910,         1},
    {WIFI_CALI_NST,    FLAGMODE_WIFI, L"WIFI_FINAL",           "WIFI_FINAL",       WIFI_NST_FLAG_UIS8910,               8},
    {WIFI_CALI_ANT,    FLAGMODE_WIFI, L"WIFI_ANTENNA",         "WIFI_ANTENNA",     WIFI_ANT_FLAG_UIS8910,               9},
    {WIFI_CALI_NST2,   FLAGMODE_WIFI, L"WIFI_FINAL2",          "WIFI_FINAL2",      WIFI_NST_FLAG2_UIS8910,             10}
};
CCheckCaliMMIFlagUIS8910::~CCheckCaliMMIFlagUIS8910(void)
{
}



BOOL CCheckCaliMMIFlagUIS8910::LoadXMLConfig(void)
{
    // MMI Flag [5/27/2017 jian.zhong]
    MMI_TestItem wholeMMI = { WHOLE_FLAG,{ 0, {0} } };
    MMI_TestItem BBATMMI = { BBAT_FLAG,{ 2, {0} } };
    MMI_TestItem PCBAMMI = { PCBA_FLAG,{ 1, {0} } };
    m_arrTestItem.clear();
    m_Uetype = GetConfigValue(L"Option:DUTType:UEType", 0x8910);
    m_bCheckWholeMMI = GetConfigValue(L"Option:MMIFlag:WholeMMI", FALSE);
    m_bCheckBBATMMI = GetConfigValue(L"Option:BBATFlag:BBAT", FALSE);
    m_bCheckPCBAMMI = GetConfigValue(L"Option:MMIFlag:PCBAMMI", FALSE);
    if (m_bCheckWholeMMI)
    {
        m_arrTestItem.push_back(wholeMMI);
    }

    if (m_bCheckPCBAMMI)
    {
        m_arrTestItem.push_back(PCBAMMI);
    }

    if (m_bCheckBBATMMI)
    {
        m_arrTestItem.push_back(BBATMMI);
    }
    // Cal flag [5/27/2017 jian.zhong]
    m_pCalFlag = NULL;
    m_nGSMFlag = 0;
    m_nLTEflag = 0;
    m_nWIFIflag = 0;
    wstring wstrTmpFlagInfo;
    BOOL bSelected = FALSE;
    for (uint16 i = GSM_CALI_AFC; i < FLAG_INDEX_MAX; i++)
    {
        wstrTmpFlagInfo = (wstring)L"Option:CaliFlag:" + m_FlagInfo[i].NameW;
        m_pCalFlag = GetFlag(m_FlagInfo[i].flagMode);
        bSelected = (BOOL)GetConfigValue(wstrTmpFlagInfo.c_str(), 0);
        if (bSelected)
        {
            *m_pCalFlag |= m_FlagInfo[i].FlagMask;
        }
    }
    // UID [5/29/2017 jian.zhong]
    m_bCheckUID = GetConfigValue(L"Option:UID:CheckUID", FALSE);
    return TRUE;
}

SPRESULT CCheckCaliMMIFlagUIS8910::__InitAction()
{

    return SP_OK;
}

SPRESULT CCheckCaliMMIFlagUIS8910::LoadWifiUeInfo(void)
{
    PC_LTE_NV_UIS8910_DATA_T TransData;

    TransData.header.eNvType = LTE_NV_UIS8910_TYPE_GET_WIFI_INFO;
    TransData.header.Position = 0;
    TransData.header.DataSize = 20 * sizeof(int);

    CHKRESULT(SP_lteLoadNV_UIS8910(m_hDUT, &TransData));

    memcpy(m_wifi_UeInfo, &TransData.nData[0], TransData.header.DataSize);

    return SP_OK;
}

SPRESULT CCheckCaliMMIFlagUIS8910::CheckMMI()
{
    if (m_arrTestItem.size() <= 0)
    {
        LogFmtStrA(SPLOGLV_INFO, "None of APK/BBAT/WHOLE/PCBA MMI is checked , test skip");
        return SP_OK;
    }
    BOOL bOk = TRUE;
    for (uint16 i = 0; i < m_arrTestItem.size(); i++)
    {
        int nSize = sizeof(TEST_NEW_RESULT_INFO);
        TEST_NEW_RESULT_INFO FinalResult;
        CHKRESULT(SP_apReadMMI(m_hDUT, &m_arrTestItem[i].nativeMMI));
        memcpy(&FinalResult, m_arrTestItem[i].nativeMMI.uBuff + 63 * nSize, nSize);
        if(FinalResult.status != 1)
        {
            bOk = FALSE;
        }   
        NOTIFY(szNativeMMIType[m_arrTestItem[i].type], LEVEL_ITEM, 1, (FinalResult.status == 1) ? 1 : 0, 1, "", 0, "-");
    }

    if (!bOk)
    {
        LogFmtStrA(SPLOGLV_ERROR, "MMI Check Fail!");
        return SP_E_SPAT_CHECK_MMI_FLAG;
    }
    return SP_OK;
}

SPRESULT CCheckCaliMMIFlagUIS8910::CheckUID()
{
    if (!m_bCheckUID)
    {
        LogFmtStrA(SPLOGLV_INFO, "Check UID Skip");
        return SP_OK;
    }

    BYTE UID[256] = {0};
    CHKRESULT(SP_GetUID_V2(m_hDUT, UID, sizeof(UID)));

    CHAR szItemName[256] = {NULL};
    sprintf_s(szItemName, "UID : [%s]", UID);
    NOTIFY(szItemName, LEVEL_UI | LEVEL_REPORT, 1, 1, 1, NULL, -1, NULL);
    // 目前仅仅显示UID，不做其他判断，后续有需求再增加 [5/29/2017 jian.zhong]
    return SP_OK;
}

SPRESULT CCheckCaliMMIFlagUIS8910::CheckCaliFlag()
{
    uint8  rspBuf[128] = { 0 };

    if ((m_nGSMFlag == 0) && (m_nLTEflag == 0) && (m_nWIFIflag == 0))
    {
        LogFmtStrA(SPLOGLV_INFO, "None of Cali Flag is checked , test Skip");
        return SP_OK;
    }

    SPRESULT result = SP_OK;
    int nGSMFlagFromNV = 0;
    int nLTEFlagFromNV = 0;
    int nWIFIFlagFromNV = 0;

    // Gsm FLAG 
    if (GSMFLAG(*GetFlag(FLAGMODE_GSM)))
    {
        if (m_Uetype == 0x891C)
        {
            Sleep(TIMEOUT_1S);//wait for UE boot
            CHKRESULT(SP_SendATCommand(m_hDUT, "AT+UHSGL=2", TRUE, rspBuf, sizeof(rspBuf), NULL, TIMEOUT_3S));
            Sleep(TIMEOUT_3S);//wait for instrument and UE reboot
        }
        DWORD adc_reserved7 = 0;
        result = SP_gsmLoadParam(
            m_hDUT,
            true,
            CALI_ADC,
            BI_EGSM,
            11,
            4,
            (void*)&adc_reserved7,
            TIMEOUT_10S);
        if (SP_OK != result)
        {
            LogFmtStrA(SPLOGLV_ERROR, "Read GSM calibration flag fail!");
            return result;
        }
        else
        {
            //SP_DataEndianConv32(g_hDiagPhone, (unsigned char *)&adc_reserved7, 4);
            nGSMFlagFromNV = adc_reserved7;
        }
    }
    else
    {
         LogFmtStrA(SPLOGLV_INFO, "None of GSM Cali Flag is checked , test Skip");
    }

    // Lte FLAG [5/27/2017 jian.zhong]
    if((LTEFLAG(*GetFlag(FLAGMODE_LTE)))|| (LTEFLAG(*GetFlag(FLAGMODE_WIFI))))
    {
        if (m_Uetype == 0x891C)
        {
            Sleep(TIMEOUT_1S);//wait for UE boot
            CHKRESULT(SP_SendATCommand(m_hDUT, "AT+UHSGL=4", TRUE, rspBuf, sizeof(rspBuf), NULL, TIMEOUT_3S));
            Sleep(TIMEOUT_3S);//wait for instrument and UE reboot
        }
        if (LTEFLAG(*GetFlag(FLAGMODE_WIFI)))
        {
            CHKRESULT_WITH_NOTIFY(LoadWifiUeInfo(), "Wifi CheckCalibFlag Load Ue Info fail.");
            LogFmtStrA(SPLOGLV_INFO, "Wifi info:calib_solution = %d; calib_flag = 0x%x", m_wifi_UeInfo[0], m_wifi_UeInfo[1]);
        }
        //unsigned short dwFlagLTE = 0;
        PC_LTE_NV_DATA_T  LTEFlag;
        ZeroMemory(&LTEFlag, sizeof(LTEFlag));
        LTEFlag.eNvType= LTE_NV_TYPE_CALI_PARAM_FLAG;
        if (m_Uetype == 0x8850)
        {
            LTEFlag.nDataCount = 2;
        }
        else
        {
            LTEFlag.nDataCount = 1;
        }
        result = SP_lteLoadNV(m_hDUT, &LTEFlag);
        if (SP_OK != result)
        {
            LogFmtStrA(SPLOGLV_ERROR, "Read LTE calibration flag fail!");
            return result;
        }
         
        nLTEFlagFromNV = LTEFlag.nData[0];//LTE Calib flag info
        nWIFIFlagFromNV = LTEFlag.nData[1];//wifi Calib flag info
    }
    else
    {
        LogFmtStrA(SPLOGLV_INFO, "None of LTE Cali Flag is checked , test Skip");
    }

    BOOL bCheckAllPass = TRUE;
    BOOL bOK = FALSE;
    CHAR szItemName[64] = {NULL};
    int* pCalFlagFromNV = NULL;
    for ( int i = GSM_CALI_AFC; i < FLAG_INDEX_MAX; i++)
    {
        // 先判断界面是否有勾选 [5/27/2017 jian.zhong]
        m_pCalFlag = GetFlag(m_FlagInfo[i].flagMode);
        if (IS_BIT_SET(*m_pCalFlag, m_FlagInfo[i].FlagMask))
        {
            // 判断NV中的标准位是否为非0 [5/27/2017 jian.zhong]
            switch (m_FlagInfo[i].flagMode)
            {
            case    FLAGMODE_GSM:
                pCalFlagFromNV = &nGSMFlagFromNV;
                break;
            case    FLAGMODE_LTE:
                pCalFlagFromNV = &nLTEFlagFromNV;
                break;
            case    FLAGMODE_WIFI:
                pCalFlagFromNV = &nWIFIFlagFromNV;
                break;
            }
            if ((i == WIFI_CALI_LTE) || (i == WIFI_CALI_WIFI))
            {
                if (m_wifi_UeInfo[0] == 0)//0:lte calib/1:wifi calib
                {
                    if (i == WIFI_CALI_WIFI)
                    {
                        continue;
                    }
                }
                else
                {
                    if (i == WIFI_CALI_LTE)
                    {
                        continue;
                    }
                }
            }

            bOK = IS_BIT_SET(*pCalFlagFromNV, m_FlagInfo[i].FlagMask);

            sprintf_s(szItemName, "Check Cali Flag : %s", m_FlagInfo[i].NameA);
            NOTIFY(szItemName, LEVEL_UI | LEVEL_REPORT, 1, (bOK) ? 1 : 0, 1, NULL, -1, NULL);
            if (!bOK)
            {
                bCheckAllPass = FALSE;
            }
        }
    }

    if (!bCheckAllPass)
    {
        LogFmtStrA(SPLOGLV_ERROR, "Check Cali Flag Fail!");
        return SP_E_SPAT_CHECK_CAL_FLAG;
    }

    return SP_OK;
}

SPRESULT CCheckCaliMMIFlagUIS8910::__PollAction(void)
{ 
    SPRESULT res = SP_OK;

    res |= CheckMMI();
    res |= CheckCaliFlag();
    res |= CheckUID();

    return res;
}

int* CCheckCaliMMIFlagUIS8910::GetFlag(FLAG_MODE_E flagmode)
{
    switch(flagmode)
    {
    case    FLAGMODE_GSM:
        return &m_nGSMFlag;
        break;
    case    FLAGMODE_LTE:
        return &m_nLTEflag;
        break;
    case    FLAGMODE_WIFI:
        return &m_nWIFIflag;
        break;
    default:
        LogFmtStrA(SPLOGLV_ERROR, "No Flag Mode Match, please Check");
        break;
    }
    return NULL;
}
