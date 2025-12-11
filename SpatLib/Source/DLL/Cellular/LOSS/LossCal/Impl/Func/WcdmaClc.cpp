#include "StdAfx.h"
#include "WcdmaClc.h"
#include "ImpBase.h"
#include "assert.h"

CWcdmaClc::CWcdmaClc(CImpBase* pSpat)
	: CWcdmaBase(pSpat)
	, m_bApt(FALSE)
{
}

CWcdmaClc::~CWcdmaClc(void)
{
}

SPRESULT CWcdmaClc::InitData()
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

SPRESULT CWcdmaClc::LoadNV()
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

SPRESULT CWcdmaClc::InitSeq(PC_CALI_WCDMA_TX_REQ_T* pSeq)
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

SPRESULT CWcdmaClc::Run_V2()
{
	PC_CALI_WCDMA_TX_REQ_T tReq;
	CHKLOSSRESULT_WITH_NOTIFY(InitSeq(&tReq), "Initialize sequence");

	RF_CABLE_LOSS_UNIT_EX* pLoss = &(m_pImp->m_lossVal.wcdmaLoss);//{0};
	ZeroMemory(pLoss, sizeof(*pLoss));
	for (int i = 0; i < m_pImp->m_gs.common.nWcdmaCnt; i++)
	{
		uint8 nBand = m_pImp->m_gs.wcdma[i].data[0].nBand;
		RF_ANT_E eAnt = RF_ANT_1st;
		if (nBand & 0x40)
		{
			eAnt = RF_ANT_3rd;
		}

		CHKRESULT(LoadBandAntInfo(nBand, eAnt));
		CHKRESULT(AntSwitch(eAnt));

		SP_BAND_INFO bi = (SP_BAND_INFO)(nBand & 0x3F);
		U_RF_PARAM param;
		param.eBand = bi;
		CHKLOSSRESULT_WITH_NOTIFY(m_pImp->m_pRFTester->SetParameter(PT_BAND_INFO, param),
			"Set band information");

		double dFreq = m_pImp->m_gs.wcdma[i].nFreq / 10.0;

		param.dUlFreq = dFreq;
		CHKLOSSRESULT_WITH_NOTIFY(m_pImp->m_pRFTester->SetParameter(PT_UPLINK_FREQ, param),
			"Set uplink frequency");
		tReq.gain_index = m_pImp->m_gs.wcdma[i].data[0].nIndex;

		tReq.band = bi;
		tReq.arfcn = CwcdmaUtility::WUlMHz2Chan(bi, dFreq);

		pLoss->arrPoint[i].nBand = nBand;
		pLoss->arrPoint[i].uArfcn[RF_IO_TX] = tReq.arfcn;
		uint16 usDlArfcn = 0;
		CwcdmaUtility::WArfcnGetDlFromUl(bi, pLoss->arrPoint[i].uArfcn[RF_IO_TX], usDlArfcn);
		pLoss->arrPoint[i].uArfcn[RF_IO_RX] = usDlArfcn;
		pLoss->arrPoint[i].dFreq[RF_IO_TX] = dFreq;
		pLoss->arrPoint[i].dFreq[RF_IO_RX] = dFreq;

		double dTotalPwr = 0;
		int16 nTargetPwr = (int16)m_pImp->m_gs.wcdma[i].data[0].nExpPwr;
		double dTargetPwr = nTargetPwr / 100.0;
		for (int j = 0; j < 3; j++)
		{
			tReq.on_off = RF_ON;
			CHKLOSSRESULT_WITH_NOTIFY(SP_wcdmaTxOnOff(m_pImp->m_hDUT, &tReq),
				"TX ON");

			Sleep(100);
			CHKLOSSRESULT_WITH_NOTIFY(m_pImp->m_pRFTester->InitTest(TI_PWR, (LPCVOID)& dTargetPwr),
				"Initialize power test");

			PWR_RLT pwrRlt;
			SPRESULT spRlt = m_pImp->m_pRFTester->FetchResult(TI_PWR, &pwrRlt);
			CHKLOSSRESULT_WITH_NOTIFY(TunePwr(dTargetPwr, spRlt, pwrRlt), "Tune power");
			dTotalPwr += pwrRlt.dAvgPwr;
			tReq.on_off = RF_OFF;
			CHKLOSSRESULT_WITH_NOTIFY(SP_wcdmaTxOnOff(m_pImp->m_hDUT, &tReq), "TX OFF");
		}

		double dPwr = dTotalPwr / 3;

		for (int j = 0; j < MUL_ANT; j++)
		{
			pLoss->arrPoint[i].dLoss[j][RF_IO_TX] = dTargetPwr - dPwr + m_pImp->m_dPreLoss;
			pLoss->arrPoint[i].dLoss[j][RF_IO_RX] = pLoss->arrPoint[i].dLoss[j][RF_IO_TX];
		}
		pLoss->nCount++;
		m_pImp->NOTIFY("WCDMA Loss", LEVEL_ITEM, m_pImp->m_dLossValLower, pLoss->arrPoint[i].dLoss[0][RF_IO_TX], m_pImp->m_dLossValUpper,
			CwcdmaUtility::W_BAND_NAME[bi], tReq.arfcn, "dB", "Index = %3d;Freq = %.1lf", tReq.gain_index, pLoss->arrPoint[i].dFreq[RF_IO_TX]);//加入频点打印 by zzq2021.8.17
		CHKLOSSRESULT_WITH_NOTIFY(m_pImp->CheckLoss(pLoss->arrPoint[i].dLoss[0][RF_IO_TX], "WCDMA"), "Check loss");
	}

	return SP_OK;
}

SPRESULT CWcdmaClc::Run_V3()
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

	RF_CABLE_LOSS_UNIT_EX* pLoss = &(m_pImp->m_lossVal.wcdmaLoss);//{0};
	ZeroMemory(pLoss, sizeof(*pLoss));
	for (int i = 0; i < m_pImp->m_gs.common.nWcdmaCnt; i++)
	{
		uint8 nBand = m_pImp->m_gs.wcdma[i].data[0].nBand;
		RF_ANT_E eAnt = RF_ANT_1st;
		if (nBand & 0x40)
		{
			eAnt = RF_ANT_3rd;
		}

		CHKRESULT(LoadBandAntInfo(nBand, eAnt));
		CHKRESULT(AntSwitch(eAnt));

		SP_BAND_INFO bi = (SP_BAND_INFO)(nBand & 0x3F);
		U_RF_PARAM param;
		param.eBand = bi;
		CHKLOSSRESULT_WITH_NOTIFY(m_pImp->m_pRFTester->SetParameter(PT_BAND_INFO, param),
			"Set band information");

		double dFreq = m_pImp->m_gs.wcdma[i].nFreq / 10.0;

		param.dUlFreq = dFreq;
		CHKLOSSRESULT_WITH_NOTIFY(m_pImp->m_pRFTester->SetParameter(PT_UPLINK_FREQ, param),
			"Set uplink frequency");
		tReq.gain_index = m_pImp->m_gs.wcdma[i].data[0].nIndex;

		tReq.eBand = bi;
		tReq.uarfcn = CwcdmaUtility::WUlMHz2Chan(bi, dFreq);

		pLoss->arrPoint[i].nBand = nBand;
		pLoss->arrPoint[i].uArfcn[RF_IO_TX] = tReq.uarfcn;
		uint16 usDlArfcn = 0;
		CwcdmaUtility::WArfcnGetDlFromUl(bi, pLoss->arrPoint[i].uArfcn[RF_IO_TX], usDlArfcn);
		pLoss->arrPoint[i].uArfcn[RF_IO_RX] = usDlArfcn;
		pLoss->arrPoint[i].dFreq[RF_IO_TX] = dFreq;
		pLoss->arrPoint[i].dFreq[RF_IO_RX] = dFreq;

		double dTotalPwr = 0;
		int16 nTargetPwr = (int16)m_pImp->m_gs.wcdma[i].data[0].nExpPwr;
		double dTargetPwr = nTargetPwr / 100.0;
		for (int j = 0; j < 3; j++)
		{
			tRxReq.eBand = bi;
			if (!CwcdmaUtility::WArfcnGetDlFromUl(bi, tReq.uarfcn, tRxReq.uarfcn))
			{
				m_pImp->LogFmtStrA(SPLOGLV_ERROR, "Get downlink channel from uplink channel failed.");
				return SP_E_SPAT_LOSS_FREQ_IVALID;
			}
			tRxReq.op_type = RF_WCD_OP_ON;
			CHKLOSSRESULT_WITH_NOTIFY(SP_ModemV3_WCDMA_SetRx(m_pImp->m_hDUT, &tRxReq, &tRxRlt), "RX ON");

			tReq.op_type = RF_WCD_OP_ON;
			CHKLOSSRESULT_WITH_NOTIFY(SP_ModemV3_WCDMA_SetTx(m_pImp->m_hDUT, &tReq, &tRlt),
				"TX ON");

			Sleep(100);
			CHKLOSSRESULT_WITH_NOTIFY(m_pImp->m_pRFTester->InitTest(TI_PWR, (LPCVOID)& dTargetPwr),
				"Initialize power test");

			PWR_RLT pwrRlt;
			SPRESULT spRlt = m_pImp->m_pRFTester->FetchResult(TI_PWR, &pwrRlt);
			CHKLOSSRESULT_WITH_NOTIFY(TunePwr(dTargetPwr, spRlt, pwrRlt), "Tune power");
			dTotalPwr += pwrRlt.dAvgPwr;
			tReq.op_type = RF_WCD_OP_OFF;
			CHKLOSSRESULT_WITH_NOTIFY(SP_ModemV3_WCDMA_SetTx(m_pImp->m_hDUT, &tReq, &tRlt), "TX OFF");

			tRxReq.op_type = RF_WCD_OP_OFF;
			CHKLOSSRESULT_WITH_NOTIFY(SP_ModemV3_WCDMA_SetRx(m_pImp->m_hDUT, &tRxReq, &tRxRlt), "RX OFF");
		}

		double dPwr = dTotalPwr / 3;

		for (int j = 0; j < MAX_RF_ANT; j++)
		{
			pLoss->arrPoint[i].dLoss[j][RF_IO_TX] = dTargetPwr - dPwr + m_pImp->m_dPreLoss;
			pLoss->arrPoint[i].dLoss[j][RF_IO_RX] = pLoss->arrPoint[i].dLoss[j][RF_IO_TX];
		}
		pLoss->nCount++;
		m_pImp->NOTIFY("WCDMA Loss", LEVEL_ITEM, m_pImp->m_dLossValLower, pLoss->arrPoint[i].dLoss[0][RF_IO_TX], m_pImp->m_dLossValUpper,
			CwcdmaUtility::W_BAND_NAME[bi], tReq.uarfcn, "dB", "Index = %3d;Freq = %.1lf", tReq.gain_index, pLoss->arrPoint[i].dFreq[RF_IO_TX]);//加入频点打印 by zzq2021.8.17
		CHKLOSSRESULT_WITH_NOTIFY(m_pImp->CheckLoss(pLoss->arrPoint[i].dLoss[0][RF_IO_TX], "WCDMA"), "Check loss");
	}

	return SP_OK;
}

SPRESULT CWcdmaClc::Run()
{
	CHKLOSSRESULT_WITH_NOTIFY(InitData(), "Initialize data");
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

SPRESULT CWcdmaClc::TunePwr(double dPwr, SPRESULT spRlt, PWR_RLT& pwrRlt)
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

SPRESULT CWcdmaClc::Init()
{
	return SP_OK;
}

SPRESULT CWcdmaClc::Release()
{
	return SP_OK;
}