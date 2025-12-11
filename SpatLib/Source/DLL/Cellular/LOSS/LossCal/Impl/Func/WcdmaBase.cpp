#include "StdAfx.h"
#include "WcdmaBase.h"
#include "ImpBase.h"

CWcdmaBase::CWcdmaBase(CImpBase *pSpat)
	: ILossFunc(pSpat)
{
}

CWcdmaBase::~CWcdmaBase(void)
{
}

SPRESULT CWcdmaBase::AntSwitch(RF_ANT_E eAnt)
{
	U_RF_PARAM param;
	RF_PORT rfPort = {eAnt, RS_IN};
	param.pPort = &rfPort;
	CHKRESULT(m_pImp->m_pRFTester->SetParameter(PT_RF_PORT, param));

	return SP_OK;
}

SPRESULT CWcdmaBase::LoadAndCheckFeatureSupport()
{
	if (m_pImp->m_eModemVer == MV_V3)
	{
		WCDMA_VERSION_T NvExport_Version;
		CHKRESULT(SP_ModemV3_WCDMA_COMMOM_GetVersion(m_pImp->m_hDUT, &NvExport_Version));
		if(1 != NvExport_Version.CAL_WCD_ANT_MAP_REQ)
		{
			m_pImp->LogFmtStrA(SPLOGLV_ERROR, "%s: NvExport_Version.CAL_WCD_ANT_MAP_REQ(%d) is not match !", __FUNCTION__, NvExport_Version.CAL_WCD_ANT_MAP_REQ);
			return SP_E_WCDMA_CALI_INVALID_CALI_VERSION;
		}
	}

	return SP_OK;
}

SPRESULT CWcdmaBase::LoadBandAntInfo(int nBand, RF_ANT_E &eAnt)
{
	if (m_pImp->m_eModemVer == MV_V3)
	{
		READ_NV_PARAM_RLT_ANT_MAP antMap;
		CHKRESULT(SP_ReadAntMap(m_pImp->m_hDUT, RF_MODE_WCDMA, nBand + 1, &antMap));
		//0 : Primary
		if (antMap.Tx[0] != antMap.Rx[0])
		{
			m_pImp->LogFmtStrA(SPLOGLV_ERROR, "Incorrect antMap configuration, PriTxAnt : %d, PriRxAnt : %d", antMap.Tx[0], antMap.Rx[0]);
			return SP_E_PHONE_INVALID_DATA; 
		}

		eAnt = (RF_ANT_E)(antMap.Tx[0] - 1);

		if (!IN_RANGE(RF_ANT_INVALID+1, eAnt, MAX_RF_ANT-1))
		{
			m_pImp->LogFmtStrA(SPLOGLV_ERROR, "Incorrect configuration, PriTxAnt : %d", eAnt);
			return SP_E_SPDB_INVALID_PARAMETER; 
		}
	}

	return SP_OK;
}