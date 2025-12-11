#include "StdAfx.h"
#include "GsmBase.h"
#include "ImpBase.h"

CGsmBase::CGsmBase(CImpBase *pSpat)
	: ILossFunc(pSpat)
{
}

CGsmBase::~CGsmBase(void)
{
}

SPRESULT CGsmBase::AntSwitch(RF_ANT_E eAnt)
{
	U_RF_PARAM param;
	RF_PORT rfPort = {eAnt, RS_INOUT};
	param.pPort = &rfPort;
	CHKRESULT(m_pImp->m_pRFTester->SetParameter(PT_RF_PORT, param));

	return SP_OK;
}

SPRESULT CGsmBase::LoadAndCheckFeatureSupport()
{
	if (m_pImp->m_eModemVer == MV_V3)
	{
		GSM_VERSION_T NvExport_Version;
		CHKRESULT(SP_ModemV3_GSM_COMMOM_GetVersion(m_pImp->m_hDUT, &NvExport_Version));
		if(1 != NvExport_Version.CAL_GSM_ANT_MAP_REQ)
		{
			m_pImp->LogFmtStrA(SPLOGLV_ERROR, "CAL_GSM_ANT_MAP_REQ(%d) is not match !", NvExport_Version.CAL_GSM_ANT_MAP_REQ);
			return SP_E_GSM_CALI_INVALID_CALI_VERSION;
		}
	}
	return SP_OK;
}

SPRESULT CGsmBase::LoadBandAntInfo(int nBand, RF_ANT_E &eAnt)
{
	if (m_pImp->m_eModemVer == MV_V3)
	{
		READ_NV_PARAM_RLT_ANT_MAP antMap;
		CHKRESULT(SP_ReadAntMap(m_pImp->m_hDUT, RF_MODE_GSM, nBand, &antMap));
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