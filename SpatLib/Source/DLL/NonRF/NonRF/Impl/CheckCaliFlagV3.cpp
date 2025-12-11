#include "StdAfx.h"
#include "CheckCaliFlagV3.h"
#include "ModeSwitch.h"
#include <atlconv.h>

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CCheckCaliFlagV3)

#define GSMFLAG(mask) (IS_BIT_SET(mask, TEST_FLAG_CALI_ADC)              \
	||IS_BIT_SET(mask, TEST_FLAG_CALI_PCS_AGC)              \
	||IS_BIT_SET(mask, TEST_FLAG_CALI_PCS_APC)              \
	||IS_BIT_SET(mask, TEST_FLAG_CALI_DCS_AGC)              \
	||IS_BIT_SET(mask, TEST_FLAG_CALI_DCS_APC)              \
	||IS_BIT_SET(mask, TEST_FLAG_CALI_EGSM_AGC)                 \
	||IS_BIT_SET(mask, TEST_FLAG_CALI_EGSM_APC)                   \
	||IS_BIT_SET(mask, TEST_FLAG_CALI_GSM850_AGC)                 \
	||IS_BIT_SET(mask, TEST_FLAG_CALI_GSM850_APC)		\
	||IS_BIT_SET(mask, TEST_FLAG_FINAL_PCS)		\
	||IS_BIT_SET(mask, TEST_FLAG_FINAL_DCS)		\
	||IS_BIT_SET(mask, TEST_FLAG_FINAL_EGSM)		\
	||IS_BIT_SET(mask, TEST_FLAG_FINAL_GSM850)		\
	||IS_BIT_SET(mask, TEST_FLAG_CALI_EDGE)		\
	||IS_BIT_SET(mask, TEST_FLAG_FINAL_EDGE))                  

#define WCDMAFLAG(mask) (IS_BIT_SET(mask, WCDMA_TESTFLAG_CALI_BANDI)        \
	||IS_BIT_SET(mask, WCDMA_TESTFLAG_CALI_BANDII)         \
	||IS_BIT_SET(mask, WCDMA_TESTFLAG_CALI_BANDIII)			\
	||IS_BIT_SET(mask, WCDMA_TESTFLAG_CALI_BANDIV)          \
	||IS_BIT_SET(mask, WCDMA_TESTFLAG_CALI_BANDV)       \
	||IS_BIT_SET(mask, WCDMA_TESTFLAG_CALI_BANDVI)         \
	||IS_BIT_SET(mask, WCDMA_TESTFLAG_CALI_BANDVIII)		\
	||IS_BIT_SET(mask, WCDMA_TESTFLAG_CALI_BANDXIX)) 

#define TDFLAG(mask) (IS_BIT_SET(mask, TEST_FLAG_CALI_TD_APC_V3)        \
	||IS_BIT_SET(mask, TEST_FLAG_CALI_TD_AGC_V3)		\
	||IS_BIT_SET(mask, TEST_FLAG_FINAL_TD_V3))

#define AntennaFLAG(mask) (IS_BIT_SET(mask, TEST_FLAG_ANTENNA))

///
CCheckCaliFlagV3::CCheckCaliFlagV3(void)
	: m_pCalFlag(NULL)
{
	m_nGSMFlag = 0;
	m_nTDFlag = 0;
	m_nWCDMAflag = 0;
	m_nAntennaFlag = 0;

	m_bFlagAFC = FALSE;
	m_bFlagLTEAuto = TRUE;
	m_bFlagNRAuto = TRUE;

}

CCheckCaliFlagV3::FlagInfo  CCheckCaliFlagV3::m_FlagInfo[FLAG_INDEX_MAX] = {
	{PCS_CALI_AGC,     FLAGMODE_GSM,  L"GSM:PCS1900:AGC",         "GSM:PCS1900:AGC",     TEST_FLAG_CALI_PCS_AGC, PCS_CALI_AGC},
	{PCS_CALI_APC,     FLAGMODE_GSM,  L"GSM:PCS1900:APC",         "GSM:PCS1900:APC",     TEST_FLAG_CALI_PCS_APC, PCS_CALI_APC},
	{DCS_CALI_AGC,     FLAGMODE_GSM,  L"GSM:DCS1800:AGC",         "GSM:DCS1800:AGC",     TEST_FLAG_CALI_DCS_AGC, DCS_CALI_AGC},
	{DCS_CALI_APC, 	   FLAGMODE_GSM,  L"GSM:DCS1800:APC",         "GSM:DCS1800:APC",     TEST_FLAG_CALI_DCS_APC, DCS_CALI_APC},
	{EGSM_CALI_AGC,    FLAGMODE_GSM,  L"GSM:EGSM900:AGC",         "GSM:EGSM900:AGC",    TEST_FLAG_CALI_EGSM_AGC, EGSM_CALI_AGC},
	{EGSM_CALI_APC,    FLAGMODE_GSM,  L"GSM:EGSM900:APC",		  "GSM:EGSM900:APC",    TEST_FLAG_CALI_EGSM_APC, EGSM_CALI_APC},
	{GSM850_CALI_AGC,  FLAGMODE_GSM,  L"GSM:GSM850:AGC",          "GSM:GSM850:AGC",  TEST_FLAG_CALI_GSM850_AGC, GSM850_CALI_AGC},
	{GSM850_CALI_APC,  FLAGMODE_GSM,  L"GSM:GSM850:APC",          "GSM:GSM850:APC",  TEST_FLAG_CALI_GSM850_APC, GSM850_CALI_APC},
	{PCS_FINAL, 	   FLAGMODE_GSM,  L"GSM:PCS1900:FT",          "GSM:PCS1900:FT",        TEST_FLAG_FINAL_PCS,    PCS_FINAL},
	{DCS_FINAL, 	   FLAGMODE_GSM,  L"GSM:DCS1800:FT",          "GSM:DCS1800:FT",        TEST_FLAG_FINAL_DCS,    DCS_FINAL},
	{EGSM_FINAL,       FLAGMODE_GSM,  L"GSM:EGSM900:FT",          "GSM:EGSM900:FT",       TEST_FLAG_FINAL_EGSM,   EGSM_FINAL},
	{GSM850_FINAL,     FLAGMODE_GSM,  L"GSM:GSM850:FT",           "GSM:GSM850:FT",     TEST_FLAG_FINAL_GSM850, GSM850_FINAL},
	{EDGE_CALI, 	   FLAGMODE_GSM,  L"EDGE:CALI",               "EDGE:CALI",        TEST_FLAG_CALI_EDGE,    EDGE_CALI},
	{EDGE_FINAL,       FLAGMODE_GSM,  L"EDGE:FT",                 "EDGE:FT",       TEST_FLAG_FINAL_EDGE,   EDGE_FINAL},
	{TD_CALI_APC,	   FLAGMODE_TD,   L"TD:APC",                  "TD:APC",      TEST_FLAG_CALI_TD_APC_V3,  TD_CALI_APC},
	{TD_CALI_AGC,	   FLAGMODE_TD,   L"TD:AGC",                  "TD:AGC",      TEST_FLAG_CALI_TD_AGC_V3,  TD_CALI_AGC},
	{TD_FINAL,	       FLAGMODE_TD,   L"TD:FT",                   "TD:FT",         TEST_FLAG_FINAL_TD_V3,     TD_FINAL},

	{W_CALI_BANDI,     FLAGMODE_WCDMA,L"WCDMA:BANDI:CALI",         "WCDMA:BANDI:CALI",     WCDMA_TESTFLAG_CALI_BANDI,  W_CALI_BANDI},
	{W_CALI_BANDII,    FLAGMODE_WCDMA,L"WCDMA:BANDII:CALI",        "WCDMA:BANDII:CALI",    WCDMA_TESTFLAG_CALI_BANDII, W_CALI_BANDII},
	{W_CALI_BANDII,    FLAGMODE_WCDMA,L"WCDMA:BANDIII:CALI",       "WCDMA:BANDIII:CALI",   WCDMA_TESTFLAG_CALI_BANDIII,W_CALI_BANDIII},
	{W_CALI_BANDIV,    FLAGMODE_WCDMA,L"WCDMA:BANDIV:CALI",        "WCDMA:BANDIV:CALI",    WCDMA_TESTFLAG_CALI_BANDIV, W_CALI_BANDIV},
	{W_CALI_BANDV,     FLAGMODE_WCDMA,L"WCDMA:BANDV:CALI",         "WCDMA:BANDV:CALI",     WCDMA_TESTFLAG_CALI_BANDV,  W_CALI_BANDV},
	{W_CALI_BANDVI,    FLAGMODE_WCDMA,L"WCDMA:BANDVI:CALI",        "WCDMA:BANDVI:CALI",    WCDMA_TESTFLAG_CALI_BANDVI, W_CALI_BANDVI},
	{W_CALI_BANDVIII,  FLAGMODE_WCDMA,L"WCDMA:BANDVIII:CALI",      "WCDMA:BANDVIII:CALI",  WCDMA_TESTFLAG_CALI_BANDVIII, W_CALI_BANDVIII},
	{W_CALI_BANDXIX,   FLAGMODE_WCDMA,L"WCDMA:BANDXIX:CALI",       "WCDMA:BANDXIX:CALI",   WCDMA_TESTFLAG_CALI_BANDXIX, W_CALI_BANDXIX},
	{WCDMA_FINAL,	   FLAGMODE_WCDMA,L"WCDMA:FT",				   "WCDMA:FT",			   WCDMA_TESTFLAG_FINAL_TEST,  WCDMA_FINAL},
	{ANTENNA_FLAG,	   FLAGMODE_ANTENNA,L"Antenna",				   "Antenna",			   TEST_FLAG_ANTENNA,  ANTENNA_FLAG},
};



CCheckCaliFlagV3::~CCheckCaliFlagV3(void)
{
}



BOOL CCheckCaliFlagV3::LoadXMLConfig(void)
{
	m_pCalFlag = NULL;
	m_nGSMFlag = 0;
	m_nTDFlag = 0;
	m_nWCDMAflag = 0;
	m_nAntennaFlag = 0;
	wstring wstrTmpFlagInfo;
	BOOL bSelected = FALSE;
	for (uint16 i = PCS_CALI_AGC; i < FLAG_INDEX_MAX; i++)
	{
		wstrTmpFlagInfo = (wstring)L"Option:CaliFlag:" + m_FlagInfo[i].NameW;
		bSelected = (BOOL)GetConfigValue(wstrTmpFlagInfo.c_str(), 0);
		if (bSelected)
		{
			m_pCalFlag = GetFlag(m_FlagInfo[i].flagMode);
			*m_pCalFlag |= m_FlagInfo[i].FlagMask;
		}
	}

	m_bFlagAFC = (BOOL)GetConfigValue(L"Option:CaliFlag:AFC", 0);
	if (_wcsicmp(GetConfigValue(L"Option:CaliFlag:LTE:Mode", L"Auto"), L"Auto") == 0)
	{
		m_bFlagLTEAuto = TRUE;
	}
	else
	{
		m_bFlagLTEAuto = FALSE;
		m_vecFlagLTECali.clear();
		m_vecFlagLTEFT.clear();
		m_vecFlagLTEAnt.clear();
		for (int i = 0; i < MAX_LTE_BAND; i++)
		{
			wstrTmpFlagInfo = (wstring)L"Option:CaliFlag:LTE:" + CLteUtility::m_BandInfo[i].NameW + (wstring)L":CALI";
			if (GetConfigValue(wstrTmpFlagInfo.c_str(), 0) == 1)
			{
				m_vecFlagLTECali.push_back(CLteUtility::m_BandInfo[i].NameW);
			}

			wstrTmpFlagInfo = (wstring)L"Option:CaliFlag:LTE:" + CLteUtility::m_BandInfo[i].NameW + (wstring)L":FT";
			if (GetConfigValue(wstrTmpFlagInfo.c_str(), 0) == 1)
			{
				m_vecFlagLTEFT.push_back(CLteUtility::m_BandInfo[i].NameW);
			}

			wstrTmpFlagInfo = (wstring)L"Option:CaliFlag:LTE:" + CLteUtility::m_BandInfo[i].NameW + (wstring)L":Antenna";
			if (GetConfigValue(wstrTmpFlagInfo.c_str(), 0) == 1)
			{
				m_vecFlagLTEAnt.push_back(CLteUtility::m_BandInfo[i].NameW);
			}
		}
	}

	if (_wcsicmp(GetConfigValue(L"Option:CaliFlag:NR:Mode", L"Auto"), L"Auto") == 0)
	{
		m_bFlagNRAuto = TRUE;
	}
	else
	{
		m_bFlagNRAuto = FALSE;
		m_vecFlagNRCali.clear();
		m_vecFlagNRFT.clear();
		m_vecFlagNRAnt.clear();
		for (int i = 0; i < MAX_NR_BAND; i++)
		{
			wstrTmpFlagInfo = (wstring)L"Option:CaliFlag:NR:" + CNrUtility::m_BandInfo[i].NameW + (wstring)L":CALI";
			if (GetConfigValue(wstrTmpFlagInfo.c_str(), 0) == 1)
			{
				m_vecFlagNRCali.push_back(CNrUtility::m_BandInfo[i].NameW);
			}

			wstrTmpFlagInfo = (wstring)L"Option:CaliFlag:NR:" + CNrUtility::m_BandInfo[i].NameW + (wstring)L":FT";
			if (GetConfigValue(wstrTmpFlagInfo.c_str(), 0) == 1)
			{
				m_vecFlagNRFT.push_back(CNrUtility::m_BandInfo[i].NameW);
			}

			wstrTmpFlagInfo = (wstring)L"Option:CaliFlag:NR:" + CNrUtility::m_BandInfo[i].NameW + (wstring)L":Antenna";
			if (GetConfigValue(wstrTmpFlagInfo.c_str(), 0) == 1)
			{
				m_vecFlagNRAnt.push_back(CNrUtility::m_BandInfo[i].NameW);
			}
		}
	}

	return TRUE;
}

SPRESULT CCheckCaliFlagV3::__InitAction()
{

	return SP_OK;
}

SPRESULT CCheckCaliFlagV3::CheckCaliFlag()
{
	if (m_nGSMFlag == 0 &&
		m_nWCDMAflag == 0 &&
		m_nTDFlag == 0 &&
		m_nAntennaFlag == 0 &&
		!m_bFlagAFC &&
		!m_bFlagLTEAuto &&
		!m_bFlagNRAuto &&
		m_vecFlagLTECali.size() == 0 &&
		m_vecFlagLTEFT.size() == 0 &&
		m_vecFlagLTEAnt.size() == 0 &&
		m_vecFlagNRCali.size() == 0 &&
		m_vecFlagNRFT.size() == 0 &&
		m_vecFlagNRAnt.size() == 0)
	{
		LogFmtStrA(SPLOGLV_INFO, "None Cali Flag checked , test Skip");
		return SP_OK;
	}

	SPRESULT result = SP_OK;
	unsigned int nGSMFlagFromNV = 0;
	unsigned int nWCDMAFlagFromNV = 0;
	unsigned int nTDFlagFromNV = 0;
	BOOL bCheckAllPass = TRUE;
	CHAR szItemName[64] = { NULL };

	//GSM
	if (*GetFlag(FLAGMODE_GSM) || *GetFlag(FLAGMODE_ANTENNA))
	{
		uint32 gsmflag = 0;
		result = SP_gsmLoadCalFlag(m_hDUT, &gsmflag);
		if (SP_OK != result)
		{
			LogFmtStrA(SPLOGLV_ERROR, "Read GSM calibration flag fail!");
			NOTIFY("CheckCaliFlagV3", LEVEL_UI | LEVEL_REPORT, 1, 0, 1, NULL, -1, NULL);
			return SP_E_SPAT_CHECK_CAL_FLAG;
		}
		else
		{
			nGSMFlagFromNV = gsmflag;
		}
	}

	//AFC
	CHKRESULT(RunAfcCheckFlag(bCheckAllPass));

	//TD
	if (*GetFlag(FLAGMODE_TD))
	{
		result = SP_tdActive(m_hDUT, TRUE);
		if (SP_OK != result)
		{
			LogFmtStrA(SPLOGLV_ERROR, "TD Active fail!");
			NOTIFY("CheckCaliFlagV3", LEVEL_UI | LEVEL_REPORT, 1, 0, 1, NULL, -1, NULL);
			return SP_E_SPAT_CHECK_CAL_FLAG;
		}

		PC_TD_NV_T stNv;
		stNv.eNvType = TD_NV_TYPE_CALI_FLAG;
		stNv.nDataNum = 1;
		result = SP_tdLoadNV(m_hDUT, &stNv);
		if (SP_OK != result)
		{
			LogFmtStrA(SPLOGLV_ERROR, "Read TD calibration flag fail!");
			NOTIFY("CheckCaliFlagV3", LEVEL_UI | LEVEL_REPORT, 1, 0, 1, NULL, -1, NULL);
			return SP_E_SPAT_CHECK_CAL_FLAG;
		}
		else
		{
			nTDFlagFromNV = stNv.nData[0];
		}
		result = SP_tdActive(m_hDUT, FALSE);
		if (SP_OK != result)
		{
			LogFmtStrA(SPLOGLV_ERROR, "TD DeActive fail!");
			NOTIFY("CheckCaliFlagV3", LEVEL_UI | LEVEL_REPORT, 1, 0, 1, NULL, -1, NULL);
			return SP_E_SPAT_CHECK_CAL_FLAG;
		}
	}

	//WCDMA
	if (*GetFlag(FLAGMODE_WCDMA))
	{
		uint16 nTestFlag = 0;
		result = SP_ModemV3_WCDMA_LoadCalFlag(m_hDUT, &nTestFlag);
		if (SP_OK != result)
		{
			LogFmtStrA(SPLOGLV_ERROR, "Read WCDMA calibration flag fail!");
			NOTIFY("CheckCaliFlagV3", LEVEL_UI | LEVEL_REPORT, 1, 0, 1, NULL, -1, NULL);
			return SP_E_SPAT_CHECK_CAL_FLAG;
		}
		else
		{
			nWCDMAFlagFromNV = nTestFlag;
		}
	}

	BOOL bOK = FALSE;
	unsigned int* pCalFlagFromNV = NULL;
	for (int i = PCS_CALI_AGC; i < FLAG_INDEX_MAX; i++)
	{
		m_pCalFlag = GetFlag(m_FlagInfo[i].flagMode);
		if (IS_BIT_SET(*m_pCalFlag, m_FlagInfo[i].FlagMask))
		{
			switch (m_FlagInfo[i].flagMode)
			{
			case    FLAGMODE_GSM:
				pCalFlagFromNV = &nGSMFlagFromNV;
				break;
			case    FLAGMODE_WCDMA:
				pCalFlagFromNV = &nWCDMAFlagFromNV;
				break;
			case	FLAGMODE_TD:
				pCalFlagFromNV = &nTDFlagFromNV;
				break;
			case	FLAGMODE_ANTENNA:
				pCalFlagFromNV = &nGSMFlagFromNV;
				break;
			}

			if (NULL == pCalFlagFromNV)
			{
				bCheckAllPass = FALSE;
			}
			else
			{
				bOK = IS_BIT_SET(*pCalFlagFromNV, m_FlagInfo[i].FlagMask);

				sprintf_s(szItemName, "Check %s", m_FlagInfo[i].NameA);
				NOTIFY(szItemName, LEVEL_UI | LEVEL_REPORT, 1, (bOK) ? 1 : 0, 1, NULL, -1, NULL);
				if (!bOK)
				{
					bCheckAllPass = FALSE;
				}
			}
		}
	}


	//LTE
	CHKRESULT(RunLteCheckFlag(bCheckAllPass));
	//NR
	CHKRESULT(RunNrCheckFlag(bCheckAllPass));

	if (!bCheckAllPass)
	{
		LogFmtStrA(SPLOGLV_ERROR, "Check Cali Flag Fail!");
		return SP_E_SPAT_CHECK_CAL_FLAG;
	}

	return SP_OK;
}

SPRESULT CCheckCaliFlagV3::__PollAction(void)
{
	SPRESULT res = SP_OK;

	res = CheckCaliFlag();

	return res;
}

int* CCheckCaliFlagV3::GetFlag(FLAG_MODE_E flagmode)
{
	switch (flagmode)
	{
	case    FLAGMODE_GSM:
		return &m_nGSMFlag;
	case    FLAGMODE_WCDMA:
		return &m_nWCDMAflag;
	case    FLAGMODE_TD:
		return &m_nTDFlag;
	case	FLAGMODE_ANTENNA:
		return &m_nAntennaFlag;
	default:
		LogFmtStrA(SPLOGLV_ERROR, "No Flag Mode Match, please Check");
		break;
	}
	return NULL;
}

SPRESULT CCheckCaliFlagV3::RunAfcCheckFlag(BOOL& bCheckAllPass)
{
	LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);
	SPRESULT result = SP_OK;

	CHAR szItemName[64] = { NULL };
	//for V3 AFC
	if (m_bFlagAFC)
	{
		AFC_CALI_NV_V3 nv;
		PC_MODEM_RF_V3_DATA_REQ_CMD_T tReq;
		ZeroMemory(&tReq, sizeof(tReq));

		tReq.eNvType = NVM_COM_CAL_DATA_AFC_RFIC;

		PC_MODEM_RF_V3_DATA_PARAM_T   tNVData;
		ZeroMemory(&tNVData, sizeof(tNVData));
		tNVData.DataSize = sizeof(AFC_CALI_NV_V3);

		result = SP_ModemV3_Nv_Read(m_hDUT, &tReq, &tNVData);
		if (SP_OK != result)
		{
			LogFmtStrA(SPLOGLV_ERROR, "Read GSM calibration flag for AFC fail!");
			NOTIFY("CheckCaliFlagV3", LEVEL_UI | LEVEL_REPORT, 1, 0, 1, NULL, -1, NULL);
			return SP_E_SPAT_CHECK_CAL_FLAG;
		}
		else
		{
			memcpy(&nv, &tNVData.nData, sizeof(AFC_CALI_NV_V3));
			bCheckAllPass = nv.Cali_Flag ? TRUE : FALSE;

			NOTIFY("Check AFC", LEVEL_UI | LEVEL_REPORT, 1, (bCheckAllPass) ? 1 : 0, 1, NULL, -1, NULL);
		}
	}
	return SP_OK;
}

SPRESULT CCheckCaliFlagV3::RunLteCheckFlag(BOOL& bCheckAllPass)
{
	LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);
	SPRESULT result = SP_OK;

	CHAR szItemName[64] = { NULL };
	if (m_bFlagLTEAuto || m_vecFlagLTECali.size() > 0 || m_vecFlagLTEFT.size() > 0 || m_vecFlagLTEAnt.size() > 0)
	{
		result = SP_ModemV3_LTE_Active(m_hDUT, TRUE);
		if (SP_OK != result)
		{
			LogFmtStrA(SPLOGLV_ERROR, "LTE Active fail!");
			NOTIFY("CheckCaliFlagV3", LEVEL_UI | LEVEL_REPORT, 1, 0, 1, NULL, -1, NULL);
			return SP_E_SPAT_CHECK_CAL_FLAG;
		}

		LTE_BAND_FLAG gFlag[MAX_LTE_BAND_NUM+10]; //此处的buffer大小需要大于MAX_LTE_BAND_NUM，目前工具内部的宏比较混乱，故留了10个值的大小作为buffer
		int nBandCnt = 0;
		result = SP_ModemV3_LTE_LoadCalFlag(m_hDUT, &nBandCnt, gFlag);

		if (SP_OK != result)
		{
			LogFmtStrA(SPLOGLV_ERROR, "Read LTE calibration flag fail!");
			NOTIFY("CheckCaliFlagV3", LEVEL_UI | LEVEL_REPORT, 1, 0, 1, NULL, -1, NULL);
			return SP_E_SPAT_CHECK_CAL_FLAG;
		}

		if (m_bFlagLTEAuto)
		{
			for (int i = 0; i < nBandCnt; i++)
			{
				if ((gFlag[i].nFlag & V3_LTE_CALI_FLAG) == 0 || ((gFlag[i].nFlag & V3_LTE_FT_FLAG) >> 8) == 0 || ((gFlag[i].nFlag & V3_LTE_ANT_FLAG) >> 16) == 0)
				{
					bCheckAllPass = FALSE;
				}

				sprintf_s(szItemName, "Check LTE:%s:Cali", gFlag[i].szBand);
				NOTIFY(szItemName, LEVEL_UI | LEVEL_REPORT, 1, ((gFlag[i].nFlag & V3_LTE_CALI_FLAG) == 0) ? 0 : 1, 1, NULL, -1, NULL);

				sprintf_s(szItemName, "Check LTE:%s:FT", gFlag[i].szBand);
				NOTIFY(szItemName, LEVEL_UI | LEVEL_REPORT, 1, (((gFlag[i].nFlag & V3_LTE_FT_FLAG) >> 8) == 0) ? 0 : 1, 1, NULL, -1, NULL);

				sprintf_s(szItemName, "Check LTE:%s:Antenna", gFlag[i].szBand);
				NOTIFY(szItemName, LEVEL_UI | LEVEL_REPORT, 1, (((gFlag[i].nFlag & V3_LTE_ANT_FLAG) >> 16) == 0) ? 0 : 1, 1, NULL, -1, NULL);

			}
		}
		else
		{
			//for LTE CALI
			int nSize = (int)m_vecFlagLTECali.size();
			for (int j = 0; j < nSize; j++)
			{
				BOOL bFindBand = FALSE;
				for (int i = 0; i < nBandCnt; i++)
				{
					if (wcscmp(m_vecFlagLTECali[j], _A2CW(gFlag[i].szBand)) == 0)
					{
						if ((gFlag[i].nFlag & V3_LTE_CALI_FLAG) == 0)
						{
							bCheckAllPass = FALSE;
						}
						bFindBand = TRUE;
						sprintf_s(szItemName, "Check LTE:%s:Cali", gFlag[i].szBand);
						NOTIFY(szItemName, LEVEL_UI | LEVEL_REPORT, 1, ((gFlag[i].nFlag & V3_LTE_CALI_FLAG) == 0) ? 0 : 1, 1, NULL, -1, NULL);
						break;
					}
				}

				if (!bFindBand)
				{
					bCheckAllPass = FALSE;
					sprintf_s(szItemName, "Check LTE:%s:Cali", _W2CA(m_vecFlagLTECali[j]));
					NOTIFY(szItemName, LEVEL_UI | LEVEL_REPORT, 1, 0, 1, NULL, -1, "Not Support");
				}
			}

			//for LTE FT
			nSize = (int)m_vecFlagLTEFT.size();
			for (int j = 0; j < nSize; j++)
			{
				BOOL bFindBand = FALSE;
				for (int i = 0; i < nBandCnt; i++)
				{
					if (wcscmp(m_vecFlagLTEFT[j], _A2CW(gFlag[i].szBand)) == 0)
					{
						if (((gFlag[i].nFlag & V3_LTE_FT_FLAG) >> 8) == 0)
						{
							bCheckAllPass = FALSE;
						}
						bFindBand = TRUE;
						sprintf_s(szItemName, "Check LTE:%s:FT", gFlag[i].szBand);
						NOTIFY(szItemName, LEVEL_UI | LEVEL_REPORT, 1, (((gFlag[i].nFlag & V3_LTE_FT_FLAG) >> 8) == 0) ? 0 : 1, 1, NULL, -1, NULL);
						break;
					}
				}

				if (!bFindBand)
				{
					bCheckAllPass = FALSE;
					sprintf_s(szItemName, "Check LTE:%s:FT", _W2CA(m_vecFlagLTEFT[j]));
					NOTIFY(szItemName, LEVEL_UI | LEVEL_REPORT, 1, 0, 1, NULL, -1, "Not Support");
				}
			}

			//for LTE Antenna
			nSize = (int)m_vecFlagLTEAnt.size();
			for (int j = 0; j < nSize; j++)
			{
				BOOL bFindBand = FALSE;
				for (int i = 0; i < nBandCnt; i++)
				{
					if (wcscmp(m_vecFlagLTEAnt[j], _A2CW(gFlag[i].szBand)) == 0)
					{
						if (((gFlag[i].nFlag & V3_LTE_ANT_FLAG) >> 16) == 0)
						{
							bCheckAllPass = FALSE;
						}
						bFindBand = TRUE;
						sprintf_s(szItemName, "Check LTE:%s:Antenna", gFlag[i].szBand);
						NOTIFY(szItemName, LEVEL_UI | LEVEL_REPORT, 1, (((gFlag[i].nFlag & V3_LTE_ANT_FLAG) >> 16) == 0) ? 0 : 1, 1, NULL, -1, NULL);
						break;
					}
				}

				if (!bFindBand)
				{
					bCheckAllPass = FALSE;
					sprintf_s(szItemName, "Check LTE:%s:Antenna", _W2CA(m_vecFlagLTEAnt[j]));
					NOTIFY(szItemName, LEVEL_UI | LEVEL_REPORT, 1, 0, 1, NULL, -1, "Not Support");
				}
			}
		}

		result = SP_ModemV3_LTE_Active(m_hDUT, FALSE);
		if (SP_OK != result)
		{
			LogFmtStrA(SPLOGLV_ERROR, "LTE V3 DeActive fail!");
			NOTIFY("CheckCaliFlagV3", LEVEL_UI | LEVEL_REPORT, 1, 0, 1, NULL, -1, NULL);
			return SP_E_SPAT_CHECK_CAL_FLAG;
		}
	}
	return SP_OK;
}

SPRESULT CCheckCaliFlagV3::RunNrCheckFlag(BOOL& bCheckAllPass)
{
	LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

	SPRESULT result = SP_OK;
	CHAR szItemName[64] = { NULL };

	if (m_bFlagNRAuto || m_vecFlagNRCali.size() > 0 || m_vecFlagNRFT.size() > 0 || m_vecFlagNRAnt.size() > 0)
	{
		int nCount = 0;
		NR_BAND_FLAG FlagInfo[MAX_NR_BAND];
		ZeroMemory(FlagInfo, sizeof(FlagInfo));

		NR_BAND_E SupportBand[MAX_NR_BAND];

		CHKRESULT(SP_NR_CAL_Active(m_hDUT));
		CHKRESULT(SP_NR_Integrated_Init(m_hDUT));
		CHKRESULT(SP_NR_CAL_SupportBand(m_hDUT, &nCount, SupportBand));

		for (int i = 0; i < nCount; i++)
		{
			NV_NR_CalFlag pFlag;
			CHKRESULT(SP_NR_Integrated_ReadFlag(m_hDUT, SupportBand[i], &pFlag));

			FlagInfo[i].nBand = CNrUtility::m_BandInfo[SupportBand[i]].nNumeral;
			FlagInfo[i].Calibration = pFlag.CalInfo;
			FlagInfo[i].FtTest = pFlag.Ft;
			FlagInfo[i].Ant = pFlag.Ant;
		}

		if (SP_OK != result)
		{
			LogFmtStrA(SPLOGLV_ERROR, "SP_NR_Integrated_ReadFlag for band info fail!");
			NOTIFY("CheckCaliFlagV3", LEVEL_UI | LEVEL_REPORT, 1, 0, 1, NULL, -1, NULL);
			return SP_E_SPAT_CHECK_CAL_FLAG;
		}

		if (m_bFlagNRAuto)
		{
			for (int i = 0; i < nCount; i++)
			{
				WCHAR szBand[10] = { 0 };
				if ((FlagInfo[i].Calibration) == 0 || (FlagInfo[i].FtTest) == 0 || (FlagInfo[i].Ant) == 0)
				{
					bCheckAllPass = FALSE;
				}

				if (FlagInfo[i].nBand == 138)
				{
					wsprintf(szBand, L"Band28A");
				}
				else if (FlagInfo[i].nBand == 139)
				{
					wsprintf(szBand, L"Band28B");
				}
                else if (FlagInfo[i].nBand == 141)
                {
                    wsprintf(szBand, L"Band28D");
                }
				else if (FlagInfo[i].nBand == 0x9A)
				{
					wsprintf(szBand, L"Band83A");
				}
				else if (FlagInfo[i].nBand == 0x9B)
				{
					wsprintf(szBand, L"Band83B");
				}
				else
				{
					wsprintf(szBand, L"Band%d", FlagInfo[i].nBand);
				}

				sprintf_s(szItemName, "Check NR:%ws:Cali", szBand);
				NOTIFY(szItemName, LEVEL_UI | LEVEL_REPORT, 1, ((FlagInfo[i].Calibration) == 0) ? 0 : 1, 1, NULL, -1, NULL);

				sprintf_s(szItemName, "Check NR:%ws:FT", szBand);
				NOTIFY(szItemName, LEVEL_UI | LEVEL_REPORT, 1, ((FlagInfo[i].FtTest) == 0) ? 0 : 1, 1, NULL, -1, NULL);

				sprintf_s(szItemName, "Check NR:%ws:Antenna", szBand);
				NOTIFY(szItemName, LEVEL_UI | LEVEL_REPORT, 1, ((FlagInfo[i].Ant) == 0) ? 0 : 1, 1, NULL, -1, NULL);
			}
		}
		else
		{
			WCHAR szBand[10] = { 0 };
			//for NR CALI
			int nSize = (int)m_vecFlagNRCali.size();
			for (int j = 0; j < nSize; j++)
			{
				BOOL bFindBand = FALSE;
				for (int i = 0; i < nCount; i++)
				{
					if (FlagInfo[i].nBand == 138)
					{
						wsprintf(szBand, L"Band28A");
					}
					else if (FlagInfo[i].nBand == 139)
					{
						wsprintf(szBand, L"Band28B");
					}
                    else if (FlagInfo[i].nBand == 141)
                    {
                        wsprintf(szBand, L"Band28D");
                    }
					else if (FlagInfo[i].nBand == 0x9A)
					{
						wsprintf(szBand, L"Band83A");
					}
					else if (FlagInfo[i].nBand == 0x9B)
					{
						wsprintf(szBand, L"Band83B");
					}
					else
					{
						wsprintf(szBand, L"Band%d", FlagInfo[i].nBand);
					}

					if (wcscmp(m_vecFlagNRCali[j], szBand) == 0)
					{
						if ((FlagInfo[i].Calibration) == 0)
						{
							bCheckAllPass = FALSE;
						}
						bFindBand = TRUE;
						sprintf_s(szItemName, "Check NR:%ws:Cali", szBand);
						NOTIFY(szItemName, LEVEL_UI | LEVEL_REPORT, 1, ((FlagInfo[i].Calibration) == 0) ? 0 : 1, 1, NULL, -1, NULL);
						break;
					}
				}

				if (!bFindBand)
				{
					bCheckAllPass = FALSE;
					sprintf_s(szItemName, "Check NR:%s:Cali", _W2CA(m_vecFlagNRCali[j]));
					NOTIFY(szItemName, LEVEL_UI | LEVEL_REPORT, 1, 0, 1, NULL, -1, "Not Support");
				}
			}

			//for NR FT
			nSize = (int)m_vecFlagNRFT.size();
			for (int j = 0; j < nSize; j++)
			{
				BOOL bFindBand = FALSE;
				for (int i = 0; i < nCount; i++)
				{
					if (FlagInfo[i].nBand == 138)
					{
						wsprintf(szBand, L"Band28A");
					}
					else if (FlagInfo[i].nBand == 139)
					{
						wsprintf(szBand, L"Band28B");
					}
                    else if (FlagInfo[i].nBand == 141)
                    {
                        wsprintf(szBand, L"Band28D");
                    }
					else if (FlagInfo[i].nBand == 0x9A)
					{
						wsprintf(szBand, L"Band83A");
					}
					else if (FlagInfo[i].nBand == 0x9B)
					{
						wsprintf(szBand, L"Band83B");
					}
					else
					{
						wsprintf(szBand, L"Band%d", FlagInfo[i].nBand);
					}

					if (wcscmp(m_vecFlagNRFT[j], szBand) == 0)
					{
						if ((FlagInfo[i].FtTest) == 0)
						{
							bCheckAllPass = FALSE;
						}
						bFindBand = TRUE;
						sprintf_s(szItemName, "Check NR:%ws:FT", szBand);
						NOTIFY(szItemName, LEVEL_UI | LEVEL_REPORT, 1, ((FlagInfo[i].FtTest) == 0) ? 0 : 1, 1, NULL, -1, NULL);
						break;
					}
				}

				if (!bFindBand)
				{
					bCheckAllPass = FALSE;
					sprintf_s(szItemName, "Check NR:%s:FT", _W2CA(m_vecFlagNRFT[j]));
					NOTIFY(szItemName, LEVEL_UI | LEVEL_REPORT, 1, 0, 1, NULL, -1, "Not Support");
				}
			}

			//for NR Antenna
			nSize = (int)m_vecFlagNRAnt.size();
			for (int j = 0; j < nSize; j++)
			{
				BOOL bFindBand = FALSE;
				for (int i = 0; i < nCount; i++)
				{
					if (FlagInfo[i].nBand == 138)
					{
						wsprintf(szBand, L"Band28A");
					}
					else if (FlagInfo[i].nBand == 139)
					{
						wsprintf(szBand, L"Band28B");
					}
                    else if (FlagInfo[i].nBand == 141)
                    {
                        wsprintf(szBand, L"Band28D");
                    }
					else if (FlagInfo[i].nBand == 0x9A)
					{
						wsprintf(szBand, L"Band83A");
					}
					else if (FlagInfo[i].nBand == 0x9B)
					{
						wsprintf(szBand, L"Band83B");
					}
					else
					{
						wsprintf(szBand, L"Band%d", FlagInfo[i].nBand);
					}

					if (wcscmp(m_vecFlagNRAnt[j], szBand) == 0)
					{
						if ((FlagInfo[i].Ant) == 0)
						{
							bCheckAllPass = FALSE;
						}
						bFindBand = TRUE;
						sprintf_s(szItemName, "Check NR:%ws:Antenna", szBand);
						NOTIFY(szItemName, LEVEL_UI | LEVEL_REPORT, 1, ((FlagInfo[i].Ant) == 0) ? 0 : 1, 1, NULL, -1, NULL);
						break;
					}
				}

				if (!bFindBand)
				{
					bCheckAllPass = FALSE;
					sprintf_s(szItemName, "Check NR:%s:Antenna", _W2CA(m_vecFlagNRAnt[j]));
					NOTIFY(szItemName, LEVEL_UI | LEVEL_REPORT, 1, 0, 1, NULL, -1, "Not Support");
				}
			}
		}
		CHKRESULT(SP_NR_CAL_Deactive(m_hDUT));
	}
}
