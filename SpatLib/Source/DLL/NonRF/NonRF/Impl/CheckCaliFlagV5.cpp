#include "StdAfx.h"
#include "CheckCaliFlagV5.h"

#define MAX_NV_BAND_NUMBER_MV5 30

#define V5_LTE_CALI_TX_FLAG 0x01
#define V5_LTE_CALI_RX_FLAG 0x02
#define V5_LTE_CALI_PDET_FLAG 0x04
#define V5_LTE_CALI_PADROOP_FLAG 0x08
#define V5_LTE_CALI_TXIRR_FLAG 0x10
#define V5_LTE_CALI_DPD_FLAG 0x20
#define V5_LTE_CALI_CALRESERVED_FLAG 0xFC0

#define V5_LTE_FT_NORMAL_FLAG 0x10000
#define V5_LTE_FT_ANT_FLAG 0x20000
#define V5_LTE_FT_FTRESERVED_FLAG 0xFFC0000

IMPLEMENT_RUNTIME_CLASS(CCheckCaliFlagV5)

CCheckCaliFlagV5::CCheckCaliFlagV5(void)
{
}

CCheckCaliFlagV5::~CCheckCaliFlagV5(void)
{
}

SPRESULT CCheckCaliFlagV5::RunAfcCheckFlag(BOOL& bCheckAllPass)
{
	LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);
	SPRESULT result = SP_OK;

	CHAR szItemName[64] = { NULL };
	//for V5 AFC
	if (m_bFlagAFC)
	{
		AFC_CALI_NV_V5 nv;
		PC_MODEM_RF_V3_DATA_REQ_CMD_T tReq;
		ZeroMemory(&tReq, sizeof(tReq));

		tReq.eNvType = NVM_COM_CAL_DATA_AFC_RFIC;

		PC_MODEM_RF_V3_DATA_PARAM_T   tNVData;
		ZeroMemory(&tNVData, sizeof(tNVData));
		tNVData.DataSize = sizeof(AFC_CALI_NV_V5);

		result = SP_ModemV3_Nv_Read(m_hDUT, &tReq, &tNVData);

		if (SP_OK != result)
		{
			LogFmtStrA(SPLOGLV_ERROR, "Read GSM calibration flag for AFC fail!");
			NOTIFY("CheckCaliFlagV5", LEVEL_UI | LEVEL_REPORT, 1, 0, 1, NULL, -1, NULL);
			return SP_E_SPAT_CHECK_CAL_FLAG;
		}
		else
		{
			memcpy(&nv, &tNVData.nData, sizeof(AFC_CALI_NV_V5));
			if (FALSE == nv.Cali_Flag)
			{
				bCheckAllPass = FALSE;
			}

			NOTIFY("Check AFC", LEVEL_UI | LEVEL_REPORT, 1, (nv.Cali_Flag) ? 1 : 0, 1, NULL, -1, NULL);
		}
	}
	return SP_OK;
}

SPRESULT CCheckCaliFlagV5::RunLteCheckFlag(BOOL& bCheckAllPass)
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
			NOTIFY("CheckCaliFlagV5", LEVEL_UI | LEVEL_REPORT, 1, 0, 1, NULL, -1, NULL);
			return SP_E_SPAT_CHECK_CAL_FLAG;
		}

		LTE_BAND_FLAG gFlag[MAX_NV_BAND_NUMBER_MV5];
		int nBandCnt = 0;
		result = SP_ModemV5_LTE_LoadCalFlag(m_hDUT, &nBandCnt, gFlag);

		if (SP_OK != result)
		{
			LogFmtStrA(SPLOGLV_ERROR, "Read LTE calibration flag fail!");
			NOTIFY("CheckCaliFlagV5", LEVEL_UI | LEVEL_REPORT, 1, 0, 1, NULL, -1, NULL);
			return SP_E_SPAT_CHECK_CAL_FLAG;
		}

		if (m_bFlagLTEAuto)
		{
			for (int i = 0; i < nBandCnt; i++)
			{
				if ((gFlag[i].nFlag & V5_LTE_CALI_TX_FLAG) == 0 || ((gFlag[i].nFlag & V5_LTE_FT_NORMAL_FLAG)) == 0 || ((gFlag[i].nFlag & V5_LTE_FT_ANT_FLAG)) == 0)
				{
					bCheckAllPass = FALSE;
				}

				sprintf_s(szItemName, "Check LTE:%s:Cali", gFlag[i].szBand);
				NOTIFY(szItemName, LEVEL_UI | LEVEL_REPORT, 1, ((gFlag[i].nFlag & V5_LTE_CALI_TX_FLAG) == 0) ? 0 : 1, 1, NULL, -1, NULL);

				sprintf_s(szItemName, "Check LTE:%s:FT", gFlag[i].szBand);
				NOTIFY(szItemName, LEVEL_UI | LEVEL_REPORT, 1, (((gFlag[i].nFlag & V5_LTE_FT_NORMAL_FLAG)) == 0) ? 0 : 1, 1, NULL, -1, NULL);

				sprintf_s(szItemName, "Check LTE:%s:Antenna", gFlag[i].szBand);
				NOTIFY(szItemName, LEVEL_UI | LEVEL_REPORT, 1, (((gFlag[i].nFlag & V5_LTE_FT_ANT_FLAG)) == 0) ? 0 : 1, 1, NULL, -1, NULL);

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
						if ((gFlag[i].nFlag & V5_LTE_CALI_TX_FLAG) == 0)
						{
							bCheckAllPass = FALSE;
						}
						bFindBand = TRUE;
						sprintf_s(szItemName, "Check LTE:%s:Cali", gFlag[i].szBand);
						NOTIFY(szItemName, LEVEL_UI | LEVEL_REPORT, 1, ((gFlag[i].nFlag & V5_LTE_CALI_TX_FLAG) == 0) ? 0 : 1, 1, NULL, -1, NULL);
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
						if (((gFlag[i].nFlag & V5_LTE_FT_NORMAL_FLAG)) == 0)
						{
							bCheckAllPass = FALSE;
						}
						bFindBand = TRUE;
						sprintf_s(szItemName, "Check LTE:%s:FT", gFlag[i].szBand);
						NOTIFY(szItemName, LEVEL_UI | LEVEL_REPORT, 1, (((gFlag[i].nFlag & V5_LTE_FT_NORMAL_FLAG)) == 0) ? 0 : 1, 1, NULL, -1, NULL);
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
						if (((gFlag[i].nFlag & V5_LTE_FT_ANT_FLAG)) == 0)
						{
							bCheckAllPass = FALSE;
						}
						bFindBand = TRUE;
						sprintf_s(szItemName, "Check LTE:%s:Antenna", gFlag[i].szBand);
						NOTIFY(szItemName, LEVEL_UI | LEVEL_REPORT, 1, (((gFlag[i].nFlag & V5_LTE_FT_ANT_FLAG)) == 0) ? 0 : 1, 1, NULL, -1, NULL);
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
			LogFmtStrA(SPLOGLV_ERROR, "LTE V5 DeActive fail!");
			NOTIFY("CheckCaliFlagV5", LEVEL_UI | LEVEL_REPORT, 1, 0, 1, NULL, -1, NULL);
			return SP_E_SPAT_CHECK_CAL_FLAG;
		}
	}
	return SP_OK;
}

SPRESULT CCheckCaliFlagV5::RunNrCheckFlag(BOOL& bCheckAllPass)
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

		NV_NR_ModemV5_CaliFlag pNv;
		for (int i = 0; i < nCount; i++)
		{
			CHKRESULT(SP_NR_NV_ModemV5_Load_CalInfo(m_hDUT, &pNv, SupportBand[i], TRUE));
			FlagInfo[i].nBand = CNrUtility::m_BandInfo[SupportBand[i]].nNumeral;
			FlagInfo[i].Calibration = pNv.TX;
			FlagInfo[i].FtTest = pNv.FtNormal;
			FlagInfo[i].Ant = pNv.Ant;
		}

		if (SP_OK != result)
		{
			LogFmtStrA(SPLOGLV_ERROR, "SP_NR_Integrated_ReadFlag for band info fail!");
			NOTIFY("CheckCaliFlagV5", LEVEL_UI | LEVEL_REPORT, 1, 0, 1, NULL, -1, NULL);
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
	return SP_OK;
}
