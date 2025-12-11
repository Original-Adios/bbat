#include "StdAfx.h"
#include "CheckCaliMMIFlagUIS8811.h"
#include "ModeSwitch.h"
#include <atlconv.h>
//#include <io.h>
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CCheckCaliMMIFlagUIS8811)

static char *szNativeMMIType[] = {("Whole Phone Native MMI Flag"), ("PCBA Native MMI Flag"), ("BBAT Native MMI Flag")};

///
CCheckCaliMMIFlagUIS8811::CCheckCaliMMIFlagUIS8811(void)
: m_bCheckWholeMMI(FALSE),
m_bCheckBBATMMI(FALSE),
m_bCheckPCBAMMI(FALSE),
m_pCalFlag(NULL),
m_bCheckUID(FALSE),
m_nNBIOTflag(0)
/*m_bCheckAPKMMI(FALSE)*/
{
    m_arrTestItem.clear();
}

CCheckCaliMMIFlagUIS8811::FlagInfo  CCheckCaliMMIFlagUIS8811::m_FlagInfo[FLAG_INDEX_MAX] = {
    {NBIOT_CALIB_AFC, 	   FLAGMODE_NBIOT,  L"NBIOT_CALIB_AFC",      "NBIOT_CALIB_AFC",       TEST_FLAG_CALI_NBIOT_AFC_UIS8811,        NBIOT_CALIB_AFC},
    {NBIOT_CALIB_AGC,      FLAGMODE_NBIOT,  L"NBIOT_CALIB_AGC",      "NBIOT_CALIB_AGC",       TEST_FLAG_CALI_NBIOT_AGC_UIS8811,        NBIOT_CALIB_AGC},
    {NBIOT_CALIB_APC,      FLAGMODE_NBIOT,  L"NBIOT_CALIB_APC",      "NBIOT_CALIB_APC",       TEST_FLAG_CALI_NBIOT_APC_UIS8811,        NBIOT_CALIB_APC},
    {NBIOT_FT,             FLAGMODE_NBIOT,  L"NBIOT_FT",             "NBIOT_FT",              TEST_FLAG_CALI_NBIOT_FT_UIS8811,         NBIOT_FT},
    {NBIOT_CALIB_RXFAT,    FLAGMODE_NBIOT,  L"NBIOT_CALIB_RXFAT",    "NBIOT_CALIB_RXFAT",     TEST_FLAG_CALI_NBIOT_RXFAT_UIS8811,      NBIOT_CALIB_RXFAT},
    {NBIOT_CALIB_TXFAT,    FLAGMODE_NBIOT,  L"NBIOT_CALIB_TXFAT",    "NBIOT_CALIB_TXFAT",     TEST_FLAG_CALI_NBIOT_TXFAT_UIS8811,      NBIOT_CALIB_TXFAT}
};
CCheckCaliMMIFlagUIS8811::~CCheckCaliMMIFlagUIS8811(void)
{
}

BOOL CCheckCaliMMIFlagUIS8811::LoadXMLConfig(void)
{
    // MMI Flag [5/27/2017 jian.zhong]
    MMI_TestItem wholeMMI = { WHOLE_FLAG,{ 0, {0} } };
    MMI_TestItem BBATMMI = { BBAT_FLAG,{ 2, {0} } };
    MMI_TestItem PCBAMMI = { PCBA_FLAG,{ 1, {0} } };
    m_arrTestItem.clear();

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
    m_nNBIOTflag = 0;
    wstring wstrTmpFlagInfo;
    BOOL bSelected = FALSE;
    for (uint16 i = NBIOT_CALIB_AFC; i < FLAG_INDEX_MAX; i++)
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

SPRESULT CCheckCaliMMIFlagUIS8811::__InitAction()
{

    return SP_OK;
}

SPRESULT CCheckCaliMMIFlagUIS8811::CheckMMI()
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

SPRESULT CCheckCaliMMIFlagUIS8811::CheckUID()
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

SPRESULT CCheckCaliMMIFlagUIS8811::CheckCaliFlag()
{
    if (m_nNBIOTflag == 0)
    {
        LogFmtStrA(SPLOGLV_INFO, "None Cali Flag checked , test Skip");
        return SP_OK;
    }

    SPRESULT result = SP_OK;
    int nNBIOTFlagFromNV = 0;
    AIOT_DIAG_CALIB_GETFLAG_REQ_T  NBIOT_Flag_req;
    AIOT_DIAG_CALIB_GETFLAG_CNF_T  NBIOT_Flag_cnf;
    NBIOT_Flag_req.Flag = 1;
    NBIOT_Flag_cnf.FlagVal = 0;
    result = SP_AiotReadCalFlag(m_hDUT, CALIB_AIOT_STUB_READ_CALIB_FLAG_STATE, &NBIOT_Flag_req, &NBIOT_Flag_cnf);

    if (SP_OK != result)
    {
        LogFmtStrA(SPLOGLV_ERROR, "Read NBIOT calibration flag  fail!");
        return result;
    }
    else
    {
        //SP_DataEndianConv32(g_hDiagPhone, (unsigned char *)&adc_reserved7, 4);
        nNBIOTFlagFromNV = NBIOT_Flag_cnf.FlagVal;
    }

    BOOL bCheckAllPass = TRUE;
    BOOL bOK = FALSE;
    CHAR szItemName[64] = {NULL};
    int* pCalFlagFromNV = NULL;
    for ( int i = NBIOT_CALIB_AFC; i < FLAG_INDEX_MAX; i++)
    {
        // 先判断界面是否有勾选 [5/27/2017 jian.zhong]
        m_pCalFlag = GetFlag(m_FlagInfo[i].flagMode);
        if (IS_BIT_SET(*m_pCalFlag, m_FlagInfo[i].FlagMask))
        {
            // 判断NV中的标准位是否为非0 [5/27/2017 jian.zhong]
            switch (m_FlagInfo[i].flagMode)
            {
            case    FLAGMODE_NBIOT:
                pCalFlagFromNV = &nNBIOTFlagFromNV;
                break;
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

SPRESULT CCheckCaliMMIFlagUIS8811::__PollAction(void)
{ 
    SPRESULT res = SP_OK;

    res |= CheckMMI();
    res |= CheckCaliFlag();
    res |= CheckUID();

    return res;
}

int* CCheckCaliMMIFlagUIS8811::GetFlag(FLAG_MODE_E flagmode)
{
    switch(flagmode)
    {
    case    FLAGMODE_NBIOT:
        return &m_nNBIOTflag;
        break;
    default:
        LogFmtStrA(SPLOGLV_ERROR, "No Flag Mode Match, please Check");
        break;
    }
    return NULL;
}
