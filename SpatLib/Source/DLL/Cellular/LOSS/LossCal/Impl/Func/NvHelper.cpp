#include "StdAfx.h"
#include "NvHelper.h"
#include "LteUtility.h"

CNvHelper::CNvHelper( LPCWSTR lpName, CFuncCenter* pFuncCenter ,BOOL bV2) : CFuncBase(lpName, pFuncCenter)
,m_pNvV2( NULL )
,m_pNvV3 (NULL)
{
	m_bV2 = bV2;
	if (m_bV2)
	{
		m_pNvV2 = new CNv(FUNC_INV, pFuncCenter);
	}
	else
	{
		m_pNvV3 = new CModV3Nv(FUNC_INV, pFuncCenter);
	}
	
	m_bSameDownloadChecksum = FALSE;
	m_nBandCount = 0;
	m_nVersion = 0;
	m_arrBandInfo.clear();
	ZeroMemory(&m_arrpDownload_HelperInfo[0], sizeof(NvData_HelperInfo)*MAX_NV_BAND_NUMBER_MV3);
}

CNvHelper::~CNvHelper(void)
{
	if (m_pNvV3 != NULL)
	{
		delete m_pNvV3;
		m_pNvV3 = NULL;
	}

	if (m_pNvV2 != NULL)
	{
		delete m_pNvV2;
		m_pNvV2 = NULL;
	}
}

SPRESULT CNvHelper::Load()
{
	if (m_bV2)
	{
		CHKRESULT(m_pNvV2->Load());
		m_bSameDownloadChecksum = m_pNvV2->m_bSameDownloadChecksum;
		m_nVersion = m_pNvV2->m_pCalibration_Version->Version;
		m_nBandCount = m_pNvV2->m_nBandCount;
		for(int i = 0;i < m_nBandCount;i++)
		{
			m_arrpDownload_HelperInfo[i].Band = m_pNvV2->m_arrpDownload_BandInfo[i]->Band;
			m_arrpDownload_HelperInfo[i].DivDisable = m_pNvV2->m_arrpDownload_BandInfo[i]->DivDisable;
			m_arrpDownload_HelperInfo[i].TxCa = m_pNvV2->m_arrpDownload_BandInfo[i]->TxCa;
			m_arrpDownload_HelperInfo[i].RxCa = m_pNvV2->m_arrpDownload_BandInfo[i]->RxCa;
		}

		m_arrBandInfo.resize(m_nBandCount);
		for(int j = 0;j < m_nBandCount;j++)
		{
			m_arrBandInfo[j].nBand = m_pNvV2->m_arrBandInfo[j].nBand;
			m_arrBandInfo[j].nIndicator =  m_pNvV2->m_arrBandInfo[j].nIndicator;
			m_arrBandInfo[j].nNumeral = m_pNvV2->m_arrBandInfo[j].nNumeral;
		}
	}
	else
	{
		CHKRESULT(m_pNvV3->Load());
		m_nVersion = m_pNvV3->m_nCaliParamVersion;
		m_nBandCount = m_pNvV3->m_nBandCnt;
		for(int i = 0;i < m_nBandCount;i++)
		{
			m_arrpDownload_HelperInfo[i].Band = m_pNvV3->m_arrpDownload_BandInfo[i].band_num;
			m_arrpDownload_HelperInfo[i].DivDisable = m_pNvV3->m_arrpDownload_BandInfo[i].div_disable;
			m_arrpDownload_HelperInfo[i].TxCa = m_pNvV3->m_arrpDownload_BandInfo[i].ulca_pcc_scc;
			m_arrpDownload_HelperInfo[i].RxCa = m_pNvV3->m_arrpDownload_BandInfo[i].dlca_pcc_scc;
		}

		m_arrBandInfo.resize(m_nBandCount);
		for(int j = 0;j < m_nBandCount;j++)
		{
			m_arrBandInfo[j].nBand = m_pNvV3->m_arrBandInfo[j].nBand;
			m_arrBandInfo[j].nIndicator =  m_pNvV3->m_arrBandInfo[j].nIndicator;
			m_arrBandInfo[j].nNumeral = m_pNvV3->m_arrBandInfo[j].nNumeral;
		}
	}

	return SP_OK;
}

SPRESULT CNvHelper::LoadClcPorComp()
{
	if (!m_bV2)
	{
		CHKRESULT(m_pNvV3->LoadClcPorComp());
	}
	return SP_OK;
}

void CNvHelper::ApplyChecksum()
{
	if (m_bV2)
	{
		m_pNvV2->ApplyChecksum();
	}
	else
	{
	}
}

SPRESULT CNvHelper::PreInit()
{
	if (m_bV2)
	{
		CHKRESULT(m_pNvV2->PreInit());
	}
	else
	{
		CHKRESULT(m_pNvV3->PreInit());
	}
	return SP_OK;
}