#include "StdAfx.h"
#include "NvHelperV5.h"
#include "LteUtility.h"

CNvHelperV5::CNvHelperV5( LPCWSTR lpName, CFuncCenter* pFuncCenter) : CFuncBase(lpName, pFuncCenter)
{
	m_pNvV5 = new CModV5Nv(FUNC_INV, pFuncCenter);
	
	m_bSameDownloadChecksum = FALSE;
	m_nBandCount = 0;
	m_arrBandInfo.clear();
	ZeroMemory(&m_arrpDownload_HelperInfo[0], sizeof(NvData_HelperInfo)*MAX_NV_BAND_NUMBER_MV3);
}

CNvHelperV5::~CNvHelperV5(void)
{
	if (m_pNvV5 != NULL)
	{
		delete m_pNvV5;
		m_pNvV5 = NULL;
	}
}

SPRESULT CNvHelperV5::Load()
{
	CHKRESULT(m_pNvV5->Load());

	m_nBandCount = m_pNvV5->m_nBandCnt;
	for(int i = 0;i < m_nBandCount;i++)
	{
		m_arrpDownload_HelperInfo[i].Band = m_pNvV5->m_arrpDownload_BandInfo[i].band_num;
		m_arrpDownload_HelperInfo[i].DivDisable = m_pNvV5->m_arrpDownload_BandInfo[i].div_disable;
		m_arrpDownload_HelperInfo[i].TxCa = m_pNvV5->m_arrpDownload_BandInfo[i].ulca_pcc_class;
		m_arrpDownload_HelperInfo[i].RxCa = m_pNvV5->m_arrpDownload_BandInfo[i].dlca_pcc_class;
	}

	m_arrBandInfo.resize(m_nBandCount);
	for(int j = 0;j < m_nBandCount;j++)
	{
		m_arrBandInfo[j].nBand = m_pNvV5->m_arrBandInfo[j].nBand;
		m_arrBandInfo[j].nIndicator = m_pNvV5->m_arrBandInfo[j].nIndicator;
		m_arrBandInfo[j].nNumeral = m_pNvV5->m_arrBandInfo[j].nNumeral;

		m_arrBandInfo[j].nTriggerGroupIndex = m_pNvV5->m_BandPathInfo[j].Groups.PathGroup[0].group_id;
		m_arrBandInfo[j].nTriggerPathId = m_pNvV5->m_BandPathInfo[j].Groups.PathGroup[0].tx_pathinfo[0].path_id;
		m_arrBandInfo[j].nTriggerUpLinkStream = m_pNvV5->m_BandPathInfo[j].Groups.PathGroup[0].rx_pathinfo[0].dl_stream;

		m_arrBandInfo[j].nTxPathId = m_pNvV5->m_BandPathInfo[j].Groups.PathGroup[0].tx_pathinfo[0].path_id;
		m_arrBandInfo[j].nTxPathGroupIndex = m_pNvV5->m_BandPathInfo[j].Groups.PathGroup[0].group_id;



		for (int i = LTE_RF_ANT_MAIN; i < MAX_LTE_RF_ANTENNA; i++)
		{
			if (i < LTE_RF_ANT_DIVERSITY)
			{
				m_arrBandInfo[j].Tx[i] = m_pNvV5->m_BandPathInfoEn[j].TxAnt.antTxPathNum > 0;
			}
			m_arrBandInfo[j].Rx[i] = m_pNvV5->m_BandPathInfoEn[j].RxAnt.antRxPathNum[i] > 0;
			if (m_arrBandInfo[j].Rx[i])
			{
				m_arrBandInfo[j].nRxPathId[i] = m_pNvV5->m_BandPathInfoEn[j].RxChain.divPathinfo[i][0].rf_channel_id; //m_pNvV5->m_BandPathInfo[j].Groups.PathGroup[0].rx_pathinfo[0].path_id;
				m_arrBandInfo[j].nRxPathGroupIndex[i] = m_pNvV5->m_BandPathInfoEn[j].RxChain.divPathinfo[i][0].Group;//m_pNvV5->m_BandPathInfo[j].Groups.PathGroup[0].group_id;
			}
		}
	}

	return SP_OK;
}

SPRESULT CNvHelperV5::PreInit()
{
	CHKRESULT(m_pNvV5->PreInit());

	return SP_OK;
}

SPRESULT CNvHelperV5::LoadClcPorComp()
{
	CHKRESULT(m_pNvV5->LoadClcPorComp());

	return SP_OK;
}

SPRESULT CNvHelperV5::ReadBandList()
{
	CHKRESULT(m_pNvV5->ReadBandList());

	return SP_OK;
}

