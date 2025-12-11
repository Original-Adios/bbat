#include "StdAfx.h"
#include "CheckCaliMMIFlag.h"
#include "ModeSwitch.h"
#include <atlconv.h>
//#include <io.h>
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CCheckCaliMMIFlag)

static char *szNativeMMIType[] = {("Whole Phone Native MMI Flag"), ("PCBA Native MMI Flag"), ("BBAT Native MMI Flag")};

#define LTEFLAG(mask) (IS_BIT_SET(mask, CALI_FLAG_LTE_AFC_TDD)              \
                ||IS_BIT_SET(mask, CALI_FlAG_LTE_AGC_TDD)              \
                ||IS_BIT_SET(mask, CALI_FLAG_LTE_APC_TDD)              \
                ||IS_BIT_SET(mask, CALI_FLAG_LTE_AFC_FDD)              \
                ||IS_BIT_SET(mask, CALI_FlAG_LTE_AGC_FDD)              \
                ||IS_BIT_SET(mask, CALI_FLAG_LTE_APC_FDD)              \
                ||IS_BIT_SET(mask, FINAL_FLAG_LTE_TDD)                 \
                ||IS_BIT_SET(mask, ANT_FLAG_LTE_TDD)                   \
                ||IS_BIT_SET(mask, FINAL_FLAG_LTE_FDD)                 \
                ||IS_BIT_SET(mask, ANT_FLAG_LTE_FDD))                  

#define WCDMAFLAG(mask) (IS_BIT_SET(mask, WCDMA_TESTFLAG_CALI_BANDI)        \
                ||IS_BIT_SET(mask, WCDMA_TESTFLAG_CALI_BANDII)         \
                ||IS_BIT_SET(mask, WCDMA_TESTFLAG_CALI_BANDV)          \
                ||IS_BIT_SET(mask, WCDMA_TESTFLAG_CALI_BANDVIII)       \
                ||IS_BIT_SET(mask, WCDMA_TESTFLAG_CALI_BANDIV)         \
                ||IS_BIT_SET(mask, WCDMA_TESTFLAG_FINAL_TEST)) 

#define CDMAFLAG(mask) (IS_BIT_SET(mask, C2K_TESTFLAG_CALI_BAND0)        \
				||IS_BIT_SET(mask, C2K_TESTFLAG_FT_BAND0))

///
CCheckCaliMMIFlag::CCheckCaliMMIFlag(void)
: m_bCheckWholeMMI(FALSE),
m_bCheckBBATMMI(FALSE),
m_bCheckPCBAMMI(FALSE),
m_pCalFlag(NULL),
m_bCheckAPKMMI(FALSE),
m_bCheckUID(FALSE)
/*m_bCheckAPKMMI(FALSE)*/
{
    m_arrTestItem.clear();
	m_nGSMFlag = 0;
	m_nWCDMAflag = 0; 
	m_nLTEflag = 0;
	m_nCDMAflag = 0;
}

CCheckCaliMMIFlag::FlagInfo  CCheckCaliMMIFlag::m_FlagInfo[FLAG_INDEX_MAX] = {
    {PCS_CALI_AGC,     FLAGMODE_GSM,  L"PCS_CALI_AGC",         "PCS_CALI_AGC",     TEST_FLAG_CALI_PCS_AGC, PCS_CALI_AGC},
    {PCS_CALI_APC,     FLAGMODE_GSM,  L"PCS_CALI_APC",         "PCS_CALI_APC",     TEST_FLAG_CALI_PCS_APC, PCS_CALI_APC},
    {DCS_CALI_AGC,     FLAGMODE_GSM,  L"DCS_CALI_AGC",         "DCS_CALI_AGC",     TEST_FLAG_CALI_DCS_AGC, DCS_CALI_AGC},
    {DCS_CALI_APC, 	   FLAGMODE_GSM,  L"DCS_CALI_APC",         "DCS_CALI_APC",     TEST_FLAG_CALI_DCS_APC, DCS_CALI_APC},
    {EGSM_CALI_AGC,    FLAGMODE_GSM,  L"EGSM_CALI_AGC",        "EGSM_CALI_AGC",    TEST_FLAG_CALI_EGSM_AGC, EGSM_CALI_AGC},
    {EGSM_CALI_APC,    FLAGMODE_GSM,  L"EGSM_CALI_APC",        "EGSM_CALI_APC",    TEST_FLAG_CALI_EGSM_APC, EGSM_CALI_APC},
    {GSM850_CALI_AGC,  FLAGMODE_GSM,  L"GSM850_CALI_AGC",      "GSM850_CALI_AGC",  TEST_FLAG_CALI_GSM850_AGC, GSM850_CALI_AGC},
    {GSM850_CALI_APC,  FLAGMODE_GSM,  L"GSM850_CALI_APC",      "GSM850_CALI_APC",  TEST_FLAG_CALI_GSM850_APC, GSM850_CALI_APC},
    {GSM_CALI_AFC, 	   FLAGMODE_GSM,  L"GSM_CALI_AFC",         "GSM_CALI_AFC",     TEST_FLAG_CALI_AFC,     GSM_CALI_AFC},
    {MISC_ADC, 		   FLAGMODE_GSM,  L"MISC_ADC",             "MISC_ADC",         TEST_FLAG_CALI_ADC,     MISC_ADC},
    {PCS_FINAL, 	   FLAGMODE_GSM,  L"PCS_FINAL",            "PCS_FINAL",        TEST_FLAG_FINAL_PCS,    PCS_FINAL},
    {DCS_FINAL, 	   FLAGMODE_GSM,  L"DCS_FINAL",            "DCS_FINAL",        TEST_FLAG_FINAL_DCS,    DCS_FINAL},
    {EGSM_FINAL,       FLAGMODE_GSM,  L"EGSM_FINAL",           "EGSM_FINAL",       TEST_FLAG_FINAL_EGSM,   EGSM_FINAL},
    {GSM850_FINAL,     FLAGMODE_GSM,  L"GSM850_FINAL",         "GSM850_FINAL",     TEST_FLAG_FINAL_GSM850, GSM850_FINAL},
    {TD_CALI_AFC,	   FLAGMODE_GSM,  L"TD_CALI_AFC",          "TD_CALI_AFC",      TEST_FLAG_CALI_TD_AFC,  TD_CALI_AFC},
    {TD_CALI_APC,	   FLAGMODE_GSM,  L"TD_CALI_APC",          "TD_CALI_APC",      TEST_FLAG_CALI_TD_APC,  TD_CALI_APC},
    {TD_CALI_AGC,	   FLAGMODE_GSM,  L"TD_CALI_AGC",          "TD_CALI_AGC",      TEST_FLAG_CALI_TD_AGC,  TD_CALI_AGC},
    {TD_FINAL,	       FLAGMODE_GSM,  L"TD_FINAL",             "TD_FINAL",         TEST_FLAG_FINAL_TD,     TD_FINAL},
    {GSM_ANTENNA,      FLAGMODE_GSM,  L"GSM_ANTENNA",          "GSM_ANTENNA",      TEST_FLAG_ANTENNA,      GSM_ANTENNA},
    {EDGE_CALI, 	   FLAGMODE_GSM,  L"EDGE_CALI",            "EDGE_CALI",        TEST_FLAG_CALI_EDGE,    EDGE_CALI},
    {EDGE_FINAL,       FLAGMODE_GSM,  L"EDGE_FINAL",           "EDGE_FINAL",       TEST_FLAG_FINAL_EDGE,   EDGE_FINAL},
    {MISC_MMI_CIT, 	   FLAGMODE_GSM,  L"MISC_MMI_CIT",         "MISC_MMI_CIT",     TEST_FLAG_MMI_CIT,      MISC_MMI_CIT},
    {W_CALI_BANDI,     FLAGMODE_WCDMA,L"W_CALI_BANDI",         "W_CALI_BANDI",     WCDMA_TESTFLAG_CALI_BANDI,  0},
    {W_CALI_BANDII,    FLAGMODE_WCDMA,L"W_CALI_BANDII",        "W_CALI_BANDII",    WCDMA_TESTFLAG_CALI_BANDII, 1},
    {W_CALI_BANDV,     FLAGMODE_WCDMA,L"W_CALI_BANDV",         "W_CALI_BANDV",     WCDMA_TESTFLAG_CALI_BANDV,  2},
    {W_CALI_BANDVIII,  FLAGMODE_WCDMA,L"W_CALI_BANDVIII",      "W_CALI_BANDVIII",  WCDMA_TESTFLAG_CALI_BANDVIII, 3},
    {W_CALI_BANDIV,    FLAGMODE_WCDMA,L"W_CALI_BANDIV",        "W_CALI_BANDIV",    WCDMA_TESTFLAG_CALI_BANDIV, 6},
    {W_FINAL_TEST,     FLAGMODE_WCDMA,L"W_FINAL_TEST",         "W_FINAL_TEST",     WCDMA_TESTFLAG_FINAL_TEST,  12},
    {LTE_CALI_AFC_TDD, FLAGMODE_LTE,  L"LTE_CALI_AFC_TDD",     "LTE_CALI_AFC_TDD", CALI_FLAG_LTE_AFC_TDD,      0},
    {LTE_CALI_AGC_TDD, FLAGMODE_LTE,  L"LTE_CALI_AGC_TDD",     "LTE_CALI_AGC_TDD", CALI_FlAG_LTE_AGC_TDD,      1},
    {LTE_CALI_APC_TDD, FLAGMODE_LTE,  L"LTE_CALI_APC_TDD",     "LTE_CALI_APC_TDD", CALI_FLAG_LTE_APC_TDD,      2},
    {LTE_CALI_AFC_FDD, FLAGMODE_LTE,  L"LTE_CALI_AFC_FDD",     "LTE_CALI_AFC_FDD", CALI_FLAG_LTE_AFC_FDD,      3},
    {LTE_CALI_AGC_FDD, FLAGMODE_LTE,  L"LTE_CALI_AGC_FDD",     "LTE_CALI_AGC_FDD", CALI_FlAG_LTE_AGC_FDD,      4},
    {LTE_CALI_APC_FDD, FLAGMODE_LTE,  L"LTE_CALI_APC_FDD",     "LTE_CALI_APC_FDD", CALI_FLAG_LTE_APC_FDD,      5},
    {LTE_FINAL_TDD,    FLAGMODE_LTE,  L"LTE_FINAL_TDD",        "LTE_FINAL_TDD",    FINAL_FLAG_LTE_TDD,         12},
    {LTE_ANTENNA_TDD,  FLAGMODE_LTE,  L"LTE_ANTENNA_TDD",      "LTE_ANTENNA_TDD",  ANT_FLAG_LTE_TDD,           13},
    {LTE_FINAL_FDD,    FLAGMODE_LTE,  L"LTE_FINAL_FDD",        "LTE_FINAL_FDD",    FINAL_FLAG_LTE_FDD,         14},
    {LTE_ANTENNA_FDD,  FLAGMODE_LTE,  L"LTE_ANTENNA_FDD",      "LTE_ANTENNA_FDD",  ANT_FLAG_LTE_FDD,           15},
	{CDMA_CALI_BAND0,  FLAGMODE_CDMA, L"CDMA_CALI_BAND0",      "CDMA_CALI_BAND0",  C2K_TESTFLAG_CALI_BAND0,    0},
	{CDMA_FT_BAND0,	   FLAGMODE_CDMA, L"CDMA_FT_BAND0",        "CDMA_FT_BAND0",    C2K_TESTFLAG_FT_BAND0,      1},
};
CCheckCaliMMIFlag::~CCheckCaliMMIFlag(void)
{
}



BOOL CCheckCaliMMIFlag::LoadXMLConfig(void)
{
    // MMI Flag [5/27/2017 jian.zhong]
    MMI_TestItem wholeMMI = { WHOLE_FLAG,{ 0, {0} } };
    MMI_TestItem BBATMMI = { BBAT_FLAG,{ 2, {0} } };
    MMI_TestItem PCBAMMI = { PCBA_FLAG,{ 1, {0} } };
    m_arrTestItem.clear();

    m_bCheckWholeMMI = GetConfigValue(L"Option:NativeMMIFlag:WholeMMI", FALSE);
    m_bCheckBBATMMI = GetConfigValue(L"Option:BBATFlag:BBAT", FALSE);
    m_bCheckPCBAMMI = GetConfigValue(L"Option:NativeMMIFlag:PCBAMMI", FALSE);
    m_bCheckAPKMMI = GetConfigValue(L"Option:APKMMIFlag:APKMMI", FALSE);
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
    m_nWCDMAflag = 0;
    m_nLTEflag = 0;
	m_nCDMAflag = 0;
    wstring wstrTmpFlagInfo;
    BOOL bSelected = FALSE;
    for (uint16 i = PCS_CALI_AGC; i < FLAG_INDEX_MAX; i++)
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

SPRESULT CCheckCaliMMIFlag::__InitAction()
{

    return SP_OK;
}

SPRESULT CCheckCaliMMIFlag::CheckMMI()
{
    if (m_arrTestItem.size() <= 0 && m_bCheckAPKMMI!=TRUE)
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

#define CheckFlags(bCheck, mask, bit, item) \
    if (bCheck) { \
        bOk &= IS_BIT_SET((mask), (bit)) ? TRUE : FALSE; \
        if (!bOk) { \
            bCheckState = FALSE; \
        } \
        NOTIFY((item), LEVEL_ITEM, 1, (bOk ? 1 : 0), 1, NULL, -1, NULL); \
    }
    BOOL bCheckState = TRUE;
    uint32 u32Flag = 0;
    if (m_bCheckAPKMMI)
    {
        CHKRESULT_WITH_NOTIFY(SP_gsmLoadCalFlag(m_hDUT, &u32Flag), "SP_gsmLoadCalFlag");
        CheckFlags(m_bCheckAPKMMI, u32Flag, TEST_FLAG_MMI_CIT, "Check [APKMMI]");
    }

    if (!bOk)
    {
        LogFmtStrA(SPLOGLV_ERROR, "MMI Check Fail!");
        return SP_E_SPAT_CHECK_MMI_FLAG;
    }
    return SP_OK;
}

SPRESULT CCheckCaliMMIFlag::CheckUID()
{
    if (!m_bCheckUID)
    {
        LogFmtStrA(SPLOGLV_INFO, "Check UID Skip");
        return SP_OK;
    }

    BYTE UID[256] = {0};
    CHKRESULT(SP_GetUID_V2(m_hDUT, UID, sizeof(UID)));

    CHAR szItemName[64] = {NULL};
    sprintf_s(szItemName, "UID : [%s]", UID);
    NOTIFY(szItemName, LEVEL_UI | LEVEL_REPORT, 1, 1, 1, NULL, -1, NULL);
    // 目前仅仅显示UID，不做其他判断，后续有需求再增加 [5/29/2017 jian.zhong]
    return SP_OK;
}

SPRESULT CCheckCaliMMIFlag::CheckCaliFlag()
{
    if (m_nGSMFlag == 0 && m_nWCDMAflag == 0 && m_nLTEflag == 0 && m_nCDMAflag == 0)
    {
        LogFmtStrA(SPLOGLV_INFO, "None Cali Flag checked , test Skip");
        return SP_OK;
    }

    SPRESULT result = SP_OK;
    int nGSMFlagFromNV = 0;
    int nWCDMAFlagFromNV = 0;
    int nLTEFlagFromNV = 0;
	int nCDMAFlagFromNV = 0;
    DWORD adc_reserved7 = 0;
    result = SP_gsmLoadParam(
        m_hDUT, 
        true, 
        CALI_ADC, 
        BI_EGSM, 
        11, 
        4, 
        (void *)&adc_reserved7,
        TIMEOUT_3S);
    if (SP_OK != result)
    {
        LogFmtStrA(SPLOGLV_ERROR, "Read GSM calibration flag  fail!");
        return FALSE;
    }
    else
    {
        //SP_DataEndianConv32(g_hDiagPhone, (unsigned char *)&adc_reserved7, 4);
        nGSMFlagFromNV = adc_reserved7;
    }
    // Lte FLAG [5/27/2017 jian.zhong]
    m_pCalFlag = GetFlag(FLAGMODE_LTE);
    if(LTEFLAG(*m_pCalFlag))
    {
        //unsigned short dwFlagLTE = 0;
        PC_LTE_NV_DATA_T  LTEFlag;
        ZeroMemory(&LTEFlag, sizeof(LTEFlag));
        LTEFlag.eNvType= LTE_NV_TYPE_CALI_PARAM_FLAG;
        LTEFlag.nDataCount= 1;
        result = SP_lteLoadNV(m_hDUT, &LTEFlag);
        if (SP_OK != result)
        {
            LogFmtStrA(SPLOGLV_ERROR, "Read LTE calibration flag  fail!");
            return result;
        }
         
        nLTEFlagFromNV = LTEFlag.nData[0];
    }
    // Wcdma Flag [5/27/2017 jian.zhong]
    m_pCalFlag = GetFlag(FLAGMODE_WCDMA);
    if (WCDMAFLAG(*m_pCalFlag))
    {
        unsigned short usFlagWCDMA = 0;
        result = SP_wcdmaLoadCalFlag(m_hDUT, &usFlagWCDMA);
        if(SP_OK != result)
        {
            LogFmtStrA(SPLOGLV_ERROR, "Read WCDMA calibration flag fail!");
            return result;
        }
        else
        {
            // 待确认是否需要此命令 [5/27/2017 jian.zhong]
            //SP_DataEndianConv16(m_hDUT, (unsigned char *)&dwFlagWCDMA, 2);
            nWCDMAFlagFromNV = usFlagWCDMA;
        }
    }


	//CDMA flag teana hu 2019.06.05
	m_pCalFlag = GetFlag(FLAGMODE_CDMA);
	if(CDMAFLAG(*m_pCalFlag))
	{
		PC_C2K_RF_NV_DATA_REQ_CMD_T stNvReq;
		ZeroMemory( &stNvReq, sizeof( PC_C2K_RF_NV_DATA_REQ_CMD_T ) );
		stNvReq.eNvType = NVM_C2K_CAL_DATA_CAL_FLAG;
		PC_C2K_RF_NV_DATA_PARAM_T stNvData;
		ZeroMemory( &stNvData, sizeof( PC_C2K_RF_NV_DATA_PARAM_T ) );
		result = SP_C2K_Nv_Read( m_hDUT, &stNvReq, &stNvData);
		if(SP_OK != result)
		{
			LogFmtStrA(SPLOGLV_ERROR, "Read CDMA calibration flag fail!");
			return result;
		}
		else
		{
			uint16 cflag = 0;
			CopyMemory( &cflag, &stNvData.nData[0], sizeof( uint16 ) );
			nCDMAFlagFromNV = cflag;

		}
	}

    BOOL bCheckAllPass = TRUE;
    BOOL bOK = FALSE;
    CHAR szItemName[64] = {NULL};
    int* pCalFlagFromNV = NULL;
    for ( int i = PCS_CALI_AGC; i < FLAG_INDEX_MAX; i++)
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
            case    FLAGMODE_WCDMA:
                pCalFlagFromNV = &nWCDMAFlagFromNV;
                break;
            case    FLAGMODE_LTE:
                pCalFlagFromNV = &nLTEFlagFromNV;
                break;
			case	FLAGMODE_CDMA:
				pCalFlagFromNV = &nCDMAFlagFromNV;
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

SPRESULT CCheckCaliMMIFlag::__PollAction(void)
{ 
    SPRESULT res = SP_OK;

    res |= CheckMMI();
    res |= CheckCaliFlag();
    res |= CheckUID();

    return res;
}

int* CCheckCaliMMIFlag::GetFlag(FLAG_MODE_E flagmode)
{
    switch(flagmode)
    {
    case    FLAGMODE_GSM:
        return &m_nGSMFlag;
        break;
    case    FLAGMODE_WCDMA:
        return &m_nWCDMAflag;
        break;
    case    FLAGMODE_LTE:
        return &m_nLTEflag;
        break;
	case	FLAGMODE_CDMA:
		return &m_nCDMAflag;
    default:
        LogFmtStrA(SPLOGLV_ERROR, "No Flag Mode Match, please Check");
        break;
    }
    return NULL;
}
