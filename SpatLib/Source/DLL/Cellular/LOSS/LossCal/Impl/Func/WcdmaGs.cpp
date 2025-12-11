#include "StdAfx.h"
#include "WcdmaGs.h"
#include "ImpBase.h"
#include "assert.h"
#include <algorithm>

CWcdmaGs::CWcdmaGs(CImpBase* pSpat)
	: CWcdmaBase(pSpat)
	, m_bApt(FALSE)
{
}

CWcdmaGs::~CWcdmaGs(void)
{
}

SPRESULT CWcdmaGs::InitData()
{
	CHKRESULT(m_pImp->m_pRFTester->SetNetMode(NM_WCDMA));
	CHKRESULT(m_pImp->m_pRFTester->InitDev(TM_NON_SIGNAL, SUB_CAL_COMMON, NULL));
	if (m_pImp->m_eModemVer == MV_V2)
	{
		CHKRESULT(SP_wcdmaActive(m_pImp->m_hDUT, TRUE));
	}
	else if (m_pImp->m_eModemVer == MV_V3)
	{
		CHKRESULT(SP_ModemV3_WCDMA_CalActive(m_pImp->m_hDUT, TRUE));
	}
	CHKRESULT(LoadAndCheckFeatureSupport());
	CHKRESULT(LoadNV());
	return SP_OK;
}

SPRESULT CWcdmaGs::LoadNV()
{
	if (m_pImp->m_eModemVer == MV_V2)
	{
		PC_WCDMA_NV_PARAM_T tNV;
		ZeroMemory(&tNV, sizeof(tNV));
		tNV.eNvType = WCDMA_DOWNLOAD_NV_INFOR_CAL_CONFIG;
		tNV.nDataSize = sizeof(DOL_PA_APT_HDET);

		CHKRESULT(SP_wcdmaLoadDLNV(m_pImp->m_hDUT, &tNV));
		DOL_PA_APT_HDET tPaAptHdet = *((DOL_PA_APT_HDET*)tNV.nData);

		m_bApt = tPaAptHdet.dcdc.dcdc_enable & 1;
	}
	else if (m_pImp->m_eModemVer == MV_V3)
	{
		PC_MODEM_RF_V3_DATA_REQ_CMD_T tNVReadCmd;
		ZeroMemory(&tNVReadCmd, sizeof(tNVReadCmd));
		tNVReadCmd.eNvType = NVM_WCD_CAL_CONFIG_TX_COM_CFG_COM;

		PC_MODEM_RF_V3_DATA_PARAM_T   tNVData;
		ZeroMemory(&tNVData, sizeof(tNVData));
		tNVData.DataSize = sizeof(PC_WCD_DL_TX_COM_CFG_T);

		CHKRESULT(SP_ModemV3_Nv_Read(m_pImp->m_hDUT, &tNVReadCmd, &tNVData));
		PC_WCD_DL_TX_COM_CFG_T tTxComCfg;
		memcpy(&tTxComCfg, tNVData.nData, sizeof(PC_WCD_DL_TX_COM_CFG_T));
		m_bApt = tTxComCfg.dcdc_enable;
	}
	return SP_OK;
}

SPRESULT CWcdmaGs::InitSeq(PC_CALI_WCDMA_TX_REQ_T* pSeq)
{
	if (NULL == pSeq)
	{
		assert(0);
		return SP_E_SPAT_INVALID_POINTER;
	}

	pSeq->iq_com = 0;
	pSeq->on_off = RF_ON;
	pSeq->path = RF_PATH_PRIMARY;
	pSeq->cmd_mask = m_bApt ? (FDT_TX_DCDC_CTRL_TYPE | FDT_TX_ENABLE_PA_DCDC) : FDT_TX_MASK_DEFAULT;
	pSeq->mode = RF_TX_HIGH_MODE;
	pSeq->dcdc_value = 0;

	return SP_OK;
}

SPRESULT CWcdmaGs::Run_V2()
{
	PC_CALI_WCDMA_TX_REQ_T tReq;
	CHKLOSSRESULT_WITH_NOTIFY(InitSeq(&tReq), "Initialize sequence");
	tReq.gain_index = 100;

	std::list<double>::iterator it = m_listFreq.begin();
	for (; it != m_listFreq.end(); ++it)
	{
		double dFreq = *it;
		
		BOOL bFind = FALSE;
		SP_BAND_INFO bi = BI_W_B1;
		for (int j = 0; j < BI_W_MAX_BAND; j++)
		{
			if (!m_pImp->m_bWcdmaBandNum[j])
			{
				continue;
			}

			bi = (SP_BAND_INFO)j;
			double dLow = CwcdmaUtility::WUlChan2MHz(bi, CwcdmaUtility::g_W_BAND_ARFCN[j][0][0]);
			double dHigh = CwcdmaUtility::WUlChan2MHz(bi, CwcdmaUtility::g_W_BAND_ARFCN[j][0][1]);
			if (IN_RANGE(dLow, dFreq, dHigh))
			{
				bi = SP_BAND_INFO(j);
				bFind = TRUE;
				break;
			}
		}

		if (!bFind)
		{
			m_pImp->LogFmtStrA(SPLOGLV_ERROR, "The frequency table is invalid");
			m_pImp->NOTIFY("Check frequency", LEVEL_UI, 1, 0, 1);
			return SP_E_SPAT_LOSS_FREQ_TABLE_IVALID;
		}

		U_RF_PARAM param;
		param.eBand = bi;
		CHKLOSSRESULT_WITH_NOTIFY(m_pImp->m_pRFTester->SetParameter(PT_BAND_INFO, param),
			"Set band information");

		RF_ANT_E eAnt = m_pImp->m_eWcdmaBandAnt[bi];
		CHKRESULT(LoadBandAntInfo((int)bi, eAnt));
		CHKRESULT(AntSwitch(eAnt));

		m_pImp->m_gs.wcdma[m_pImp->m_gs.common.nWcdmaCnt].nFreq = (uint16)(dFreq * 10);

		m_pImp->m_gs.wcdma[m_pImp->m_gs.common.nWcdmaCnt].data[0].nBand = (uint8)bi;
		if (m_pImp->m_eWcdmaBandAnt[bi] == RF_ANT_3rd)
		{
			m_pImp->m_gs.wcdma[m_pImp->m_gs.common.nWcdmaCnt].data[0].nBand |= 0x40;
		}

		tReq.band = bi;
		tReq.arfcn = CwcdmaUtility::WUlMHz2Chan(bi, dFreq);

		param.dUlFreq = dFreq;
		CHKLOSSRESULT_WITH_NOTIFY(m_pImp->m_pRFTester->SetParameter(PT_UPLINK_FREQ, param),
			"Set uplink frequency");

		tReq.on_off = RF_ON;
		CHKLOSSRESULT_WITH_NOTIFY(SP_wcdmaTxOnOff(m_pImp->m_hDUT, &tReq), "TX ON");
		Sleep(100);
		CHKLOSSRESULT_WITH_NOTIFY(m_pImp->m_pRFTester->InitTest(TI_PWR, (LPCVOID)& m_pImp->m_dWcdmaTargetPwr),
			"Initialize power test");

		/// Ignore indicator returned from instrument,because power measured
		/// may be overload or under-driven due to the gain index needs to be tuned.
		PWR_RLT pwrRlt;
		SPRESULT spRlt = m_pImp->m_pRFTester->FetchResult(TI_PWR, &pwrRlt);
		CHKLOSSRESULT_WITH_NOTIFY(TunePwr(m_pImp->m_dWcdmaTargetPwr, spRlt, pwrRlt),
			"Tune power");

		tReq.on_off = RF_OFF;
		CHKLOSSRESULT_WITH_NOTIFY(SP_wcdmaTxOnOff(m_pImp->m_hDUT, &tReq), "TX OFF");
		int nGainIndex = (int)(tReq.gain_index + (pwrRlt.dAvgPwr - m_pImp->m_dWcdmaTargetPwr) * 8);
		tReq.gain_index = (uint16)(nGainIndex < 0 ? 0 : nGainIndex);

		double dTotalPwr = 0;
		for (int j = 0; j < 3; j++)
		{
			tReq.on_off = RF_ON;
			CHKLOSSRESULT_WITH_NOTIFY(SP_wcdmaTxOnOff(m_pImp->m_hDUT, &tReq), "TX ON");
			Sleep(100);
			CHKLOSSRESULT_WITH_NOTIFY(m_pImp->m_pRFTester->InitTest(TI_PWR, (LPCVOID)& m_pImp->m_dWcdmaTargetPwr),
				"Initialize power test");
			CHKLOSSRESULT_WITH_NOTIFY(m_pImp->m_pRFTester->FetchResult(TI_PWR, &pwrRlt),
				"Fetch power");
			dTotalPwr += pwrRlt.dAvgPwr;

			tReq.on_off = RF_OFF;
			CHKLOSSRESULT_WITH_NOTIFY(SP_wcdmaTxOnOff(m_pImp->m_hDUT, &tReq), "TX OFF");
		}

		double dPwr = dTotalPwr / 3;
		m_pImp->NOTIFY("WCDMA GS", LEVEL_REPORT | LEVEL_UI, -30, dPwr, 30, CwcdmaUtility::W_BAND_NAME[bi],
			tReq.arfcn, "dBm", "Index:%3d;Freq = %.1lf", tReq.gain_index, param.dUlFreq);//金板制作添加频点打印 by zzq 2021.8.17
		if (dPwr > 30 || dPwr < -30)
		{
			return SP_E_SPAT_OUT_OF_RANGE;
		}
		m_pImp->m_gs.wcdma[m_pImp->m_gs.common.nWcdmaCnt].data[0].nExpPwr = (uint16)(dPwr * 100);
		m_pImp->m_gs.wcdma[m_pImp->m_gs.common.nWcdmaCnt].data[0].nIndex = tReq.gain_index;
		m_pImp->m_gs.common.nWcdmaCnt++;
	}

	return SP_OK;
}

SPRESULT CWcdmaGs::Run_V3()
{
	/// wphy tx的打开依赖于rx，很多配置在rx的操作中进行处理，在tx之前必须先rx on，并且在tx off之后rx off；
	PC_CAL_WCD_RX_REQ_T tRxReq;
	tRxReq.ant = RF_ANT_PRIMARY;
	tRxReq.pri_gain_index = 42;
	PC_CAL_WCD_RX_RLT_T tRxRlt;

	PC_CAL_WCD_TX_REQ_T tReq;
	PC_CAL_WCD_TX_RLT_T tRlt;
	tReq.iq_com = 0;
	tReq.op_type = RF_WCD_OP_ON;
	tReq.ant = RF_ANT_PRIMARY;
	tReq.cmd_mask = CAL_WCD_TX_CMD_MASK_E(m_bApt ? (RF_WCD_TX_CMD_MASK_DCDC_CTRL_TYPE | RF_WCD_TX_CMD_MASK_DAC_OUTPUT_DC) : 0);
	tReq.mode = RF_WCD_TX_HIGH_MODE;
	tReq.dcdc_value = 0;

	tReq.gain_index = 100;

	std::list<double>::iterator it = m_listFreq.begin();
	for (; it != m_listFreq.end(); ++it)
	{
		double dFreq = *it;
		
		BOOL bFind = FALSE;
		SP_BAND_INFO bi = BI_W_B1;
		for (int j = 0; j < BI_W_MAX_BAND; j++)
		{
			if (!m_pImp->m_bWcdmaBandNum[j])
			{
				continue;
			}

			bi = (SP_BAND_INFO)j;
			double dLow = CwcdmaUtility::WUlChan2MHz(bi, CwcdmaUtility::g_W_BAND_ARFCN[j][0][0]);
			double dHigh = CwcdmaUtility::WUlChan2MHz(bi, CwcdmaUtility::g_W_BAND_ARFCN[j][0][1]);
			if (IN_RANGE(dLow, dFreq, dHigh))
			{
				bi = SP_BAND_INFO(j);
				bFind = TRUE;
				break;
			}
		}

		if (!bFind)
		{
			m_pImp->LogFmtStrA(SPLOGLV_ERROR, "The frequency table is invalid");
			m_pImp->NOTIFY("Check frequency", LEVEL_UI, 1, 0, 1);
			return SP_E_SPAT_LOSS_FREQ_TABLE_IVALID;
		}

		U_RF_PARAM param;
		param.eBand = bi;
		CHKLOSSRESULT_WITH_NOTIFY(m_pImp->m_pRFTester->SetParameter(PT_BAND_INFO, param),
			"Set band information");

		RF_ANT_E eAnt = m_pImp->m_eWcdmaBandAnt[bi];
		CHKRESULT(LoadBandAntInfo((int)bi, eAnt));
		CHKRESULT(AntSwitch(eAnt));

		m_pImp->m_gs.wcdma[m_pImp->m_gs.common.nWcdmaCnt].nFreq = (uint16)(dFreq * 10);
		m_pImp->m_gs.wcdma[m_pImp->m_gs.common.nWcdmaCnt].data[0].nBand = (uint8)bi;
		if (m_pImp->m_eWcdmaBandAnt[bi] == RF_ANT_3rd)
		{
			m_pImp->m_gs.wcdma[m_pImp->m_gs.common.nWcdmaCnt].data[0].nBand |= 0x40;
		}

		tReq.eBand = bi;
		tReq.uarfcn = CwcdmaUtility::WUlMHz2Chan(bi, dFreq);

		param.dUlFreq = dFreq;
		CHKLOSSRESULT_WITH_NOTIFY(m_pImp->m_pRFTester->SetParameter(PT_UPLINK_FREQ, param),
			"Set uplink frequency");

		tRxReq.eBand = bi;
		if (!CwcdmaUtility::WArfcnGetDlFromUl(bi, tReq.uarfcn, tRxReq.uarfcn))
		{
			m_pImp->LogFmtStrA(SPLOGLV_ERROR, "Get downlink channel from uplink channel failed.");
			return SP_E_SPAT_LOSS_FREQ_IVALID;
		}
		tRxReq.op_type = RF_WCD_OP_ON;
		CHKLOSSRESULT_WITH_NOTIFY(SP_ModemV3_WCDMA_SetRx(m_pImp->m_hDUT, &tRxReq, &tRxRlt), "RX ON");

		tReq.op_type = RF_WCD_OP_ON;
		CHKLOSSRESULT_WITH_NOTIFY(SP_ModemV3_WCDMA_SetTx(m_pImp->m_hDUT, &tReq, &tRlt), "TX ON");
		Sleep(100);
		CHKLOSSRESULT_WITH_NOTIFY(m_pImp->m_pRFTester->InitTest(TI_PWR, (LPCVOID)& m_pImp->m_dWcdmaTargetPwr),
			"Initialize power test");

		/// Ignore indicator returned from instrument,because power measured
		/// may be overload or under-driven due to the gain index needs to be tuned.
		PWR_RLT pwrRlt;
		SPRESULT spRlt = m_pImp->m_pRFTester->FetchResult(TI_PWR, &pwrRlt);
		CHKLOSSRESULT_WITH_NOTIFY(TunePwr(m_pImp->m_dWcdmaTargetPwr, spRlt, pwrRlt),
			"Tune power");

		tReq.op_type = RF_WCD_OP_OFF;
		CHKLOSSRESULT_WITH_NOTIFY(SP_ModemV3_WCDMA_SetTx(m_pImp->m_hDUT, &tReq, &tRlt), "TX OFF");

		tRxReq.op_type = RF_WCD_OP_OFF;
		CHKLOSSRESULT_WITH_NOTIFY(SP_ModemV3_WCDMA_SetRx(m_pImp->m_hDUT, &tRxReq, &tRxRlt), "RX OFF");

		int nGainIndex = (int)(tReq.gain_index + (pwrRlt.dAvgPwr - m_pImp->m_dWcdmaTargetPwr) * 8);
		tReq.gain_index = (uint16)(nGainIndex < 0 ? 0 : nGainIndex);

		double dTotalPwr = 0;
		for (int j = 0; j < 3; j++)
		{
			tRxReq.op_type = RF_WCD_OP_ON;
			CHKLOSSRESULT_WITH_NOTIFY(SP_ModemV3_WCDMA_SetRx(m_pImp->m_hDUT, &tRxReq, &tRxRlt), "RX ON");

			tReq.op_type = RF_WCD_OP_ON;
			CHKLOSSRESULT_WITH_NOTIFY(SP_ModemV3_WCDMA_SetTx(m_pImp->m_hDUT, &tReq, &tRlt), "TX ON");
			Sleep(100);
			CHKLOSSRESULT_WITH_NOTIFY(m_pImp->m_pRFTester->InitTest(TI_PWR, (LPCVOID)& m_pImp->m_dWcdmaTargetPwr),
				"Initialize power test");
			CHKLOSSRESULT_WITH_NOTIFY(m_pImp->m_pRFTester->FetchResult(TI_PWR, &pwrRlt),
				"Fetch power");
			dTotalPwr += pwrRlt.dAvgPwr;

			tReq.op_type = RF_WCD_OP_OFF;
			CHKLOSSRESULT_WITH_NOTIFY(SP_ModemV3_WCDMA_SetTx(m_pImp->m_hDUT, &tReq, &tRlt), "TX OFF");

			tRxReq.op_type = RF_WCD_OP_OFF;
			CHKLOSSRESULT_WITH_NOTIFY(SP_ModemV3_WCDMA_SetRx(m_pImp->m_hDUT, &tRxReq, &tRxRlt), "RX OFF");
		}

		double dPwr = dTotalPwr / 3;
		m_pImp->NOTIFY("WCDMA GS", LEVEL_REPORT | LEVEL_UI, -30, dPwr, 30, CwcdmaUtility::W_BAND_NAME[bi],
			tReq.uarfcn, "dBm", "Index:%3d;Freq = %.1lf", tReq.gain_index, param.dUlFreq);//金板制作添加频点打印 by zzq 2021.8.17
		if (dPwr > 30 || dPwr < -30)
		{
			return SP_E_SPAT_OUT_OF_RANGE;
		}
		m_pImp->m_gs.wcdma[m_pImp->m_gs.common.nWcdmaCnt].data[0].nExpPwr = (uint16)(dPwr * 100);
		m_pImp->m_gs.wcdma[m_pImp->m_gs.common.nWcdmaCnt].data[0].nIndex = tReq.gain_index;
		m_pImp->m_gs.common.nWcdmaCnt++;
	}

	return SP_OK;
}

SPRESULT CWcdmaGs::Run()
{
	ConfigFreq();
	CHKLOSSRESULT_WITH_NOTIFY(InitData(), "Initialize data");

	m_pImp->m_gs.common.nWcdmaCnt = 0;

	if (m_pImp->m_eModemVer == MV_V2)
	{
		CHKRESULT(Run_V2());
	}
	else if (m_pImp->m_eModemVer == MV_V3)
	{
		CHKRESULT(Run_V3());
		CHKRESULT(SP_ModemV3_WCDMA_CalActive(m_pImp->m_hDUT, FALSE));
	}

	return SP_OK;
}

void CWcdmaGs::ConfigFreq()
{
	m_listFreq.clear();
	for (int i = 0; i < BI_W_MAX_BAND; i++)
	{
		if (!m_pImp->m_bWcdmaBandNum[i])
		{
			continue;
		}
		SP_BAND_INFO bi = (SP_BAND_INFO)i;
		double dLow = CwcdmaUtility::WUlChan2MHz(bi, CwcdmaUtility::g_W_BAND_ARFCN[i][0][0]);
		double dHigh = CwcdmaUtility::WUlChan2MHz(bi, CwcdmaUtility::g_W_BAND_ARFCN[i][0][1]);
		double dMid = (dLow + dHigh) / 2;

		m_listFreq.push_back(dLow);
		m_listFreq.push_back(dMid);
		m_listFreq.push_back(dHigh);
	}
	m_listFreq.sort();
}

SPRESULT CWcdmaGs::TunePwr(double dPwr, SPRESULT spRlt, PWR_RLT& pwrRlt)
{
	int j;
	for (j = 0; j < 10; j++)
	{
		if (spRlt == SP_E_RF_OVER_RANGE)
		{
			dPwr += 10;
		}
		else if (spRlt == SP_E_RF_UNDER_RANGE)
		{
			dPwr -= 10;
		}
		else if (spRlt == SP_OK)
		{
			return SP_OK;
		}
		else
		{
			return SP_E_SPAT_LOSS_TUNE_PWR;
		}
		CHKRESULT(m_pImp->m_pRFTester->InitTest(TI_PWR, (LPCVOID)& dPwr));
		spRlt = m_pImp->m_pRFTester->FetchResult(TI_PWR, &pwrRlt);
	}

	if (j == 10)
	{
		return SP_E_SPAT_LOSS_TUNE_PWR;
	}

	return SP_OK;
}

SPRESULT CWcdmaGs::Init()
{
	return SP_OK;
}

SPRESULT CWcdmaGs::Release()
{
	return SP_OK;
}