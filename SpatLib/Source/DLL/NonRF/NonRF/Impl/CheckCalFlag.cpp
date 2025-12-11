#include "StdAfx.h"
#include "CheckCalFlag.h"

//
IMPLEMENT_RUNTIME_CLASS(CCheckCalFlag)

//////////////////////////////////////////////////////////////////////////
CCheckCalFlag::CCheckCalFlag(void)
{
}

CCheckCalFlag::~CCheckCalFlag(void)
{
}

BOOL CCheckCalFlag::LoadXMLConfig(void)
{
	m_Flags.bADC = (BOOL)GetConfigValue(L"Option:CaliFlag:ADC", 1);
	m_Flags.bAFC = (BOOL)GetConfigValue(L"Option:CaliFlag:AFC", 1);

	LPCWSTR GSM[BI_GSM_MAX_BAND] = { L"GSM850", L"EGSM900", L"DCS1800", L"PCS1900" };
	std::wstring strKey = L"";
	for (int i = 0; i < BI_GSM_MAX_BAND; i++)
	{
		strKey = (std::wstring)L"Option:CaliFlag:GSM:" + GSM[i] + (std::wstring)L":AGC";
		m_Flags.bCalAGC2G[i] = (BOOL)GetConfigValue(strKey.c_str(), 1);

		strKey = (std::wstring)L"Option:CaliFlag:GSM:" + GSM[i] + (std::wstring)L":APC";
		m_Flags.bCalAPC2G[i] = (BOOL)GetConfigValue(strKey.c_str(), 1);

		strKey = (std::wstring)L"Option:CaliFlag:GSM:" + GSM[i] + (std::wstring)L":FT";
		m_Flags.bFT2G[i] = (BOOL)GetConfigValue(strKey.c_str(), 1);
	}

	m_Flags.bCalEDGE = (BOOL)GetConfigValue(L"Option:CaliFlag:EDGE:CALI", 1);
	m_Flags.bFTEDGE = (BOOL)GetConfigValue(L"Option:CaliFlag:EDGE:FT", 1);

	m_Flags.bCalAGCTD = (BOOL)GetConfigValue(L"Option:CaliFlag:TD:AGC", 1);
	m_Flags.bCalAPCTD = (BOOL)GetConfigValue(L"Option:CaliFlag:TD:APC", 1);
	m_Flags.bFTTD = (BOOL)GetConfigValue(L"Option:CaliFlag:TD:FT", 1);

	LPCWSTR WBAND[8] = { L"BANDI", L"BANDII", L"BANDIII", L"BANDIV", L"BANDV", L"BANDVI", L"BANDVIII", L"BANDXIX" };
	for (int i = 0; i < 8; i++)
	{
		strKey = (std::wstring)L"Option:CaliFlag:WCDMA:" + WBAND[i] + (std::wstring)L":CALI";
		m_Flags.bCalWCDMA[i] = (BOOL)GetConfigValue(strKey.c_str(), 0);
	}
	m_Flags.bFTWCDMA = (BOOL)GetConfigValue(L"Option:CaliFlag:WCDMA:FT", 1);

	m_Flags.bCalTDD = (BOOL)GetConfigValue(L"Option:CaliFlag:LTE:TDD:CALI", 1);
	m_Flags.bCalFDD = (BOOL)GetConfigValue(L"Option:CaliFlag:LTE:FDD:CALI", 1);
	m_Flags.bFTTDD = (BOOL)GetConfigValue(L"Option:CaliFlag:LTE:TDD:FT", 1);
	m_Flags.bFTFDD = (BOOL)GetConfigValue(L"Option:CaliFlag:LTE:FDD:FT", 1);
	m_Flags.bANTTDD = (BOOL)GetConfigValue(L"Option:CaliFlag:LTE:TDD:ANT", 1);//add TDD Antenna flag Bug 1478777  2021.3.1
	m_Flags.bANTFDD = (BOOL)GetConfigValue(L"Option:CaliFlag:LTE:FDD:ANT", 1);//add FDD Antenna flag Bug 1478777  2021.3.1
	m_Flags.bAFCTDD = (BOOL)GetConfigValue(L"Option:CaliFlag:LTE:TDD:AFC", 1);//add TDD AFC flag Bug 1692513  2021.11.11
	m_Flags.bAFCFDD = (BOOL)GetConfigValue(L"Option:CaliFlag:LTE:FDD:AFC", 1);//add FDD AFC flag Bug 1692513  2021.11.11

	m_Flags.bCalCDMA = (BOOL)GetConfigValue(L"Option:CaliFlag:CDMA:CALI", 1);
	m_Flags.bFTCDMA = (BOOL)GetConfigValue(L"Option:CaliFlag:CDMA:FT", 1);

	m_Flags.bAntenna = (BOOL)GetConfigValue(L"Option:CaliFlag:Antenna", 1);

	m_Flags.bDMRAFC = (BOOL)GetConfigValue(L"Option:CaliFlag:DMR:AFC", 0);
	m_Flags.bDMRAPC = (BOOL)GetConfigValue(L"Option:CaliFlag:DMR:APC", 0);
	m_Flags.bDMRAGC = (BOOL)GetConfigValue(L"Option:CaliFlag:DMR:AGC", 0);
	m_Flags.bDMRFT = (BOOL)GetConfigValue(L"Option:CaliFlag:DMR:FT", 0);

	return TRUE;
}

SPRESULT CCheckCalFlag::__PollAction(void)
{
	BOOL bCheckState = TRUE;

#define CheckFlags(bCheck, mask, bit, item) \
    if (bCheck) { \
        BOOL bOK = IS_BIT_SET((mask), (bit)) ? TRUE : FALSE; \
        if (!bOK) { \
            bCheckState = FALSE; \
        } \
        NOTIFY((item), LEVEL_ITEM, 1, (bOK ? 1 : 0), 1, NULL, -1, NULL); \
    }

	CHAR szItem[32] = { 0 };

	if (m_Flags.bADC
		|| m_Flags.bAFC
		|| m_Flags.bCalAGC2G[BI_GSM_850] || m_Flags.bCalAGC2G[BI_EGSM] || m_Flags.bCalAGC2G[BI_DCS] || m_Flags.bCalAGC2G[BI_PCS]
		|| m_Flags.bCalAPC2G[BI_GSM_850] || m_Flags.bCalAPC2G[BI_EGSM] || m_Flags.bCalAPC2G[BI_DCS] || m_Flags.bCalAPC2G[BI_PCS]
		|| m_Flags.bFT2G[BI_GSM_850] || m_Flags.bFT2G[BI_EGSM] || m_Flags.bFT2G[BI_DCS] || m_Flags.bFT2G[BI_PCS]
		|| m_Flags.bCalEDGE || m_Flags.bFTEDGE
		|| m_Flags.bCalAGCTD || m_Flags.bCalAPCTD || m_Flags.bFTTD
		|| m_Flags.bAntenna
		)
	{
		uint32 u32Flag = 0;
		CHKRESULT_WITH_NOTIFY(SP_gsmLoadCalFlag(m_hDUT, &u32Flag), "SP_gsmLoadCalFlag");

		CheckFlags(m_Flags.bADC, u32Flag, TEST_FLAG_CALI_ADC, "Check [ADC]");
		CheckFlags(m_Flags.bAFC, u32Flag, TEST_FLAG_CALI_AFC, "Check [AFC]");

		const uint32 GSM_AGC_BIT[BI_GSM_MAX_BAND] = { TEST_FLAG_CALI_GSM850_AGC, TEST_FLAG_CALI_EGSM_AGC, TEST_FLAG_CALI_DCS_AGC, TEST_FLAG_CALI_PCS_AGC };
		const uint32 GSM_APC_BIT[BI_GSM_MAX_BAND] = { TEST_FLAG_CALI_GSM850_APC, TEST_FLAG_CALI_EGSM_APC, TEST_FLAG_CALI_DCS_APC, TEST_FLAG_CALI_PCS_APC };
		const uint32 GSM_FT_BIT[BI_GSM_MAX_BAND] = { TEST_FLAG_FINAL_GSM850,    TEST_FLAG_FINAL_EGSM,    TEST_FLAG_FINAL_DCS,    TEST_FLAG_FINAL_PCS };
		LPCSTR     GSM_BAND_NAME[BI_GSM_MAX_BAND] = { "GSM850", "EGSM900", "DCS1800", "PCS1900" };

		for (int i = 0; i < BI_GSM_MAX_BAND; i++)
		{
			sprintf_s(szItem, "Check [%s AGC]", GSM_BAND_NAME[i]);
			CheckFlags(m_Flags.bCalAGC2G[i], u32Flag, GSM_AGC_BIT[i], szItem);

			sprintf_s(szItem, "Check [%s APC]", GSM_BAND_NAME[i]);
			CheckFlags(m_Flags.bCalAPC2G[i], u32Flag, GSM_APC_BIT[i], szItem);

			sprintf_s(szItem, "Check [%s FT]", GSM_BAND_NAME[i]);
			CheckFlags(m_Flags.bFT2G[i], u32Flag, GSM_FT_BIT[i], szItem);
		}

		CheckFlags(m_Flags.bCalEDGE, u32Flag, TEST_FLAG_CALI_EDGE, "Check [EDGE CAL]");
		CheckFlags(m_Flags.bFTEDGE, u32Flag, TEST_FLAG_FINAL_EDGE, "Check [EDGE FT]");

		CheckFlags(m_Flags.bCalAPCTD, u32Flag, TEST_FLAG_CALI_TD_APC, "Check [TD APC]");
		CheckFlags(m_Flags.bCalAGCTD, u32Flag, TEST_FLAG_CALI_TD_AGC, "Check [TD AGC]");
		CheckFlags(m_Flags.bFTTD, u32Flag, TEST_FLAG_FINAL_TD, "Check [TD FT]");

		CheckFlags(m_Flags.bAntenna, u32Flag, TEST_FLAG_ANTENNA, "Check [Antenna]");
	}

	{
		uint16 u16Flag = 0;
		const uint32 BITS[8] = {
			WCDMA_TESTFLAG_CALI_BANDI,      //  1
			WCDMA_TESTFLAG_CALI_BANDII,     //  2
			WCDMA_TESTFLAG_CALI_BANDIII,    //  3
			WCDMA_TESTFLAG_CALI_BANDIV,     //  4
			WCDMA_TESTFLAG_CALI_BANDV,      //  5
			WCDMA_TESTFLAG_CALI_BANDVI,     //  6
			WCDMA_TESTFLAG_CALI_BANDVIII,   //  8
			WCDMA_TESTFLAG_CALI_BANDXIX,    // 19
		};

		LPSTR WBAND[8] = { "BANDI", "BANDII", "BANDIII", "BANDIV", "BANDV", "BANDVI", "BANDVIII", "BANDXIX" };

		//调整wcdmaLoadCalFlag的实现逻辑。任意一个BAND勾选时，执行一次后break。 Bug 1769814  2021.12.22 Runde.Qiu
		//增加对m_Flags.bFTWCDMA的判断。 Bug 1809651 2022.01.19 Runde.Qiu
		for (int i = 0; i < 8; i++)
		{
			if (m_Flags.bCalWCDMA[i] || m_Flags.bFTWCDMA)
			{
				CHKRESULT_WITH_NOTIFY(SP_wcdmaLoadCalFlag(m_hDUT, &u16Flag), "SP_wcdmaLoadCalFlag");
				break;
			}
		}

		for (int i = 0; i < 8; i++)
		{
			if (!m_Flags.bCalWCDMA[i])
			{
				continue;
			}

			sprintf_s(szItem, "Check [%s CAL]", WBAND[i]);
			CheckFlags(m_Flags.bCalWCDMA[i], u16Flag, BITS[i], szItem);
		}

		CheckFlags(m_Flags.bFTWCDMA, u16Flag, WCDMA_TESTFLAG_FINAL_TEST, "Check [WCDMA FT]");
	}
	
	{
		if (m_Flags.bCalTDD || m_Flags.bCalFDD || m_Flags.bFTTDD || m_Flags.bFTFDD)
		{
			uint16 u16Flag = 0;
			CHKRESULT_WITH_NOTIFY(SP_lteLoadCalFlag(m_hDUT, &u16Flag), "SP_lteLoadCalFlag");

			CheckFlags(m_Flags.bCalTDD, u16Flag, CALI_FlAG_LTE_AGC_TDD, "Check [TDD CALI AGC]");
			CheckFlags(m_Flags.bCalTDD, u16Flag, CALI_FLAG_LTE_APC_TDD, "Check [TDD CALI APC]");
			CheckFlags(m_Flags.bCalFDD, u16Flag, CALI_FlAG_LTE_AGC_FDD, "Check [FDD CALI AGC]");
			CheckFlags(m_Flags.bCalFDD, u16Flag, CALI_FLAG_LTE_APC_FDD, "Check [FDD CALI APC]");

			CheckFlags(m_Flags.bFTTDD, u16Flag, FINAL_FLAG_LTE_TDD, "Check [TDD FT]");
			CheckFlags(m_Flags.bFTFDD, u16Flag, FINAL_FLAG_LTE_FDD, "Check [FDD FT]");
		}

		//add TDD/FDD Antenna flag,判断如果为真,OK Bug 1478777  2021.3.1
		if (m_Flags.bANTTDD || m_Flags.bANTFDD)
		{
			PC_LTE_NV_DATA_T  LTEFlag;
			ZeroMemory(&LTEFlag, sizeof(LTEFlag));
			LTEFlag.eNvType = LTE_NV_TYPE_CALI_PARAM_FLAG;
			LTEFlag.nDataCount = 1;
			CHKRESULT_WITH_NOTIFY(SP_lteLoadNV(m_hDUT, &LTEFlag), "SP_lteLoadNV");

			unsigned short uFlag = LTEFlag.nData[0];

			CheckFlags(m_Flags.bANTTDD, uFlag, ANT_FLAG_LTE_TDD, "Check [TDD ANT]");
			CheckFlags(m_Flags.bANTFDD, uFlag, ANT_FLAG_LTE_FDD, "Check [FDD ANT]");
		}

		//add LTE TDD/FDD AFC flag,Bug 1692513  2021.11.11
        if (m_Flags.bAFCTDD || m_Flags.bAFCFDD)
        {
            PC_LTE_NV_DATA_T  LTEFlag;
            ZeroMemory(&LTEFlag, sizeof(LTEFlag));
            LTEFlag.eNvType = LTE_NV_TYPE_CALI_PARAM_FLAG;
            LTEFlag.nDataCount = 1;
            CHKRESULT_WITH_NOTIFY(SP_lteLoadNV(m_hDUT, &LTEFlag), "SP_lteLoadNV");

            CheckFlags(m_Flags.bAFCTDD, LTEFlag.nData[0], CALI_FLAG_LTE_AFC_TDD, "Check [LTE TDD AFC]");
            CheckFlags(m_Flags.bAFCFDD, LTEFlag.nData[0], CALI_FLAG_LTE_AFC_FDD, "Check [LTE FDD AFC]");
        }
						
		//CDMA
		if (m_Flags.bCalCDMA || m_Flags.bFTCDMA)
		{
			uint16 u16Flag = 0;
			PC_C2K_RF_NV_DATA_REQ_CMD_T stNvReq;
			ZeroMemory(&stNvReq, sizeof(PC_C2K_RF_NV_DATA_REQ_CMD_T));
			stNvReq.eNvType = NVM_C2K_CAL_DATA_CAL_FLAG;
			PC_C2K_RF_NV_DATA_PARAM_T stNvData;
			ZeroMemory(&stNvData, sizeof(PC_C2K_RF_NV_DATA_PARAM_T));
			CHKRESULT_WITH_NOTIFY(SP_C2K_Nv_Read(m_hDUT, &stNvReq, &stNvData), "SP_C2K_Nv_Read");

			CopyMemory(&u16Flag, &stNvData.nData[0], sizeof(uint16));

			CheckFlags(m_Flags.bCalCDMA, u16Flag, C2K_TESTFLAG_CALI_BAND0, "Check [CDMA CALI]");
			CheckFlags(m_Flags.bFTCDMA, u16Flag, C2K_TESTFLAG_FT_BAND0, "Check [CDMA FT]");
		}

		{
			if (m_Flags.bDMRAFC || m_Flags.bDMRAGC || m_Flags.bDMRAPC || m_Flags.bDMRFT)
			{
				uint16 u16Flag = 0;
				CHKRESULT_WITH_NOTIFY(SP_dmrLoadFlag(m_hDUT, u16Flag), "SP_dmrLoadFlag");

				CheckFlags(m_Flags.bDMRAFC, u16Flag, DMR_AFC_FLAG, "Check [DMR AFC]");
				CheckFlags(m_Flags.bDMRAPC, u16Flag, DMR_APC_FLAG, "Check [DMR APC]");
				CheckFlags(m_Flags.bDMRAGC, u16Flag, DMR_AGC_FLAG, "Check [DMR AGC]");
				CheckFlags(m_Flags.bDMRFT, u16Flag, DMR_FT_FLAG, "Check [DMR FT]");
			}
		}

		if (bCheckState)
		{
			return SP_OK;
		}
		else
		{
			return SP_E_SPAT_CHECK_CAL_FLAG;
		}
	}
}