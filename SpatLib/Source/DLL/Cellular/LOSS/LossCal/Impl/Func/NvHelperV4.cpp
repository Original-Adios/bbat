#include "StdAfx.h"
#include "NvHelperV4.h"
#include "LteUtility.h"

CNvHelperV4::CNvHelperV4( LPCWSTR lpName, CFuncCenter* pFuncCenter) : CFuncBase(lpName, pFuncCenter)
{
	m_pNvV4 = new CModV4Nv(FUNC_INV, pFuncCenter);
	
	m_bSameDownloadChecksum = FALSE;
	m_nBandCount = 0;
	m_arrBandInfo.clear();
	ZeroMemory(&m_arrpDownload_HelperInfo[0], sizeof(NvData_HelperInfo)*MAX_NV_BAND_NUMBER_MV3);
}

CNvHelperV4::~CNvHelperV4(void)
{
	if (m_pNvV4 != NULL)
	{
		delete m_pNvV4;
		m_pNvV4 = NULL;
	}
}

SPRESULT CNvHelperV4::Load()
{
	CHKRESULT(m_pNvV4->Load());

	m_nBandCount = m_pNvV4->m_nBandCnt;
	for(int i = 0;i < m_nBandCount;i++)
	{
		m_arrpDownload_HelperInfo[i].Band = m_pNvV4->m_arrpDownload_BandInfo[i].band_num;
		m_arrpDownload_HelperInfo[i].DivDisable = m_pNvV4->m_arrpDownload_BandInfo[i].div_disable;
		m_arrpDownload_HelperInfo[i].TxCa = m_pNvV4->m_arrpDownload_BandInfo[i].ulca_pcc_scc;
		m_arrpDownload_HelperInfo[i].RxCa = m_pNvV4->m_arrpDownload_BandInfo[i].dlca_pcc_scc;
	}

	m_arrBandInfo.resize(m_nBandCount);
	for(int j = 0;j < m_nBandCount;j++)
	{
		m_arrBandInfo[j].nBand = m_pNvV4->m_arrBandInfo[j].nBand;
		m_arrBandInfo[j].nIndicator = m_pNvV4->m_arrBandInfo[j].nIndicator;
		m_arrBandInfo[j].nNumeral = m_pNvV4->m_arrBandInfo[j].nNumeral;
		m_arrBandInfo[j].nCount2rx = m_pNvV4->m_BandPathInfo[j].RFChannel.cnt_2rx;
		m_arrBandInfo[j].nCount4rx = m_pNvV4->m_BandPathInfo[j].RFChannel.cnt_4rx;

		if (m_pNvV4->m_BandPathInfoEn[j].RxChain.priPathinfo[RF_CHAINNAL0].size() != 0)
		{
			m_arrBandInfo[j].RxChannelId[0] = m_pNvV4->m_BandPathInfoEn[j].RxChain.priPathinfo[RF_CHAINNAL0][0].rf_channel_id;
			m_arrBandInfo[j].RxAntIndx[0] = m_pNvV4->m_BandPathInfoEn[j].RxChain.priPathinfo[RF_CHAINNAL0][0].AntIndex;
		}

		if (m_pNvV4->m_BandPathInfoEn[j].RxChain.divPathinfo[RF_CHAINNAL1].size() != 0)
		{
			m_arrBandInfo[j].RxChannelId[1] = m_pNvV4->m_BandPathInfoEn[j].RxChain.divPathinfo[RF_CHAINNAL1][0].rf_channel_id;
			m_arrBandInfo[j].RxAntIndx[1] = m_pNvV4->m_BandPathInfoEn[j].RxChain.divPathinfo[RF_CHAINNAL1][0].AntIndex;
		}
		if (m_pNvV4->m_BandPathInfoEn[j].RxChain.priPathinfo[RF_CHAINNAL2].size() != 0)
		{
			m_arrBandInfo[j].RxChannelId[2] = m_pNvV4->m_BandPathInfoEn[j].RxChain.priPathinfo[RF_CHAINNAL2][0].rf_channel_id;
			m_arrBandInfo[j].RxAntIndx[2] = m_pNvV4->m_BandPathInfoEn[j].RxChain.priPathinfo[RF_CHAINNAL2][0].AntIndex;
		}
		if (m_pNvV4->m_BandPathInfoEn[j].RxChain.divPathinfo[RF_CHAINNAL3].size() != 0)
		{
			m_arrBandInfo[j].RxChannelId[3] = m_pNvV4->m_BandPathInfoEn[j].RxChain.divPathinfo[RF_CHAINNAL3][0].rf_channel_id;
			m_arrBandInfo[j].RxAntIndx[3] = m_pNvV4->m_BandPathInfoEn[j].RxChain.divPathinfo[RF_CHAINNAL3][0].AntIndex;
		}

		if(m_pNvV4->m_BandPathInfoEn[j].TxChain.priPathinfo[RF_CHAINNAL0].size() != 0)
		{
			m_arrBandInfo[j].TxChannelId[0] = m_pNvV4->m_BandPathInfoEn[j].TxChain.priPathinfo[RF_CHAINNAL0][0].rf_channel_id;
			m_arrBandInfo[j].TxAntIndx[0] = m_pNvV4->m_BandPathInfoEn[j].TxChain.priPathinfo[RF_CHAINNAL0][0].AntIndex;
		}

		if (m_pNvV4->m_BandPathInfoEn[j].TxChain.priPathinfo[RF_CHAINNAL1].size() != 0)
		{
			m_arrBandInfo[j].TxChannelId[1] = m_pNvV4->m_BandPathInfoEn[j].TxChain.priPathinfo[RF_CHAINNAL1][0].rf_channel_id;
			m_arrBandInfo[j].TxAntIndx[1] = m_pNvV4->m_BandPathInfoEn[j].TxChain.priPathinfo[RF_CHAINNAL1][0].AntIndex;
		}

		if (m_pNvV4->m_BandPathInfoEn[j].TxChain.priPathinfo[RF_CHAINNAL2].size() != 0)
		{
			m_arrBandInfo[j].TxChannelId[2] = m_pNvV4->m_BandPathInfoEn[j].TxChain.priPathinfo[RF_CHAINNAL2][0].rf_channel_id;
			m_arrBandInfo[j].TxAntIndx[2] = m_pNvV4->m_BandPathInfoEn[j].TxChain.priPathinfo[RF_CHAINNAL2][0].AntIndex;
		}
	
		if (m_pNvV4->m_BandPathInfoEn[j].TxChain.priPathinfo[RF_CHAINNAL3].size() != 0)
		{
			m_arrBandInfo[j].TxChannelId[3] = m_pNvV4->m_BandPathInfoEn[j].TxChain.priPathinfo[RF_CHAINNAL3][0].rf_channel_id;
			m_arrBandInfo[j].TxAntIndx[3] = m_pNvV4->m_BandPathInfoEn[j].TxChain.priPathinfo[RF_CHAINNAL3][0].AntIndex;
		}

		for (int i = LTE_RF_ANT_MAIN; i < MAX_LTE_RF_ANTENNA; i++)
		{
			if (i < LTE_RF_ANT_DIVERSITY)
			{
				m_arrBandInfo[j].Tx[i] = m_pNvV4->m_BandPathInfoEn[j].TxAnt.antTxPathNum[i] > 0;
			}
			m_arrBandInfo[j].Rx[i] = m_pNvV4->m_BandPathInfoEn[j].RxAnt.antRxPathNum[i] > 0;
		}
	}

	return SP_OK;
}

SPRESULT CNvHelperV4::PreInit()
{
	CHKRESULT(m_pNvV4->PreInit());

	return SP_OK;
}

SPRESULT CNvHelperV4::LoadClcPorComp()
{
	CHKRESULT(m_pNvV4->LoadClcPorComp());

	return SP_OK;
}