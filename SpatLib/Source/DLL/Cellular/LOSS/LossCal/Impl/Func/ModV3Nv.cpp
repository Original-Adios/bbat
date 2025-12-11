#include "StdAfx.h"
#include "ModV3Nv.h"
#include "LteUtility.h"

CModV3Nv::CModV3Nv( LPCWSTR lpName, CFuncCenter* pFuncCenter ) : CFuncBase(lpName, pFuncCenter)
{
	m_nMaxBandCount = MAX_NV_BAND_NUMBER_MV3;
	m_hDUT = NULL;
	m_stNvmParam.NVM_Uninitialize();

	ZeroMemory(m_nBand, sizeof(uint8)*MAX_NV_BAND_NUMBER_MV3);
	ZeroMemory(&m_BandSupportInfo, sizeof(NvData_LTE_BAND_SUPPORT_INFO)*MAX_NV_BAND_NUMBER_MV3);
	ZeroMemory(&m_BandListEnFlag, sizeof(NvData_LTE_BAND_LIST_INFOR));

	m_pFileConfig = NULL;
	m_nBandCnt = 0;
	m_nCaliParamVersion = 0;
	m_stNvRsp.NVM_Uninitialize();
	m_stProgramNvData.NVM_Uninitialize();

	m_arrGS_PortComp.clear();
	m_arrNvPos.clear();

	m_bBandAdaptation = FALSE;
}

CModV3Nv::~CModV3Nv(void)
{

}

SPRESULT CModV3Nv::PreInit()
{
	CHKRESULT(__super::PreInit());

	m_hDUT = m_pSpatBase->m_hDUT;
	m_pFileConfig = (CFileConfig*)m_pFuncCenter->GetFunc(FUNC_FILE_CONFIG);

	return SP_OK;
}

SPRESULT CModV3Nv::Load()
{
	LogFmtStrA(SPLOGLV_INFO, "%s: Load NV start", __FUNCTION__);

	m_stNvRsp.NVM_Uninitialize();

	CHKRESULT(ReadNv( &m_stNvRsp.stRspNv, NVM_LTE_CAL_DATA_CAL_VERSION ));
	memcpy(&m_nCaliParamVersion, &m_stNvRsp.stRspNv.nData[0], sizeof(m_nCaliParamVersion));

	CHKRESULT(LoadAndCheckFeatureSupport());
	CHKRESULT(InitBandNv());

	for ( uint32 i = 0; i < m_nBandCnt; i++ )
	{
		int nBnd = 0;

		for ( ; nBnd < m_nMaxBandCount; nBnd++ )
		{
			if ( CLteUtility::m_BandInfo[m_nBand[i]].BandIdent == m_BandSupportInfo[nBnd].band_num )
			{
				break;
			}
		}

		if ( nBnd == m_nMaxBandCount )
		{
			UiSendMsg( "Not Support Band", LEVEL_UI, 1, 0, 1, CLteUtility::m_BandInfo[m_nBand[i]].NameA, -1, NULL, NULL );
			return SP_E_LTE_CALI_NOT_SUPPORT_BAND;
		}

		m_arrNvPos.push_back(nBnd);

		CHKRESULT( LoadDownloadAntMap( m_nBand[i], m_BandSupportInfo[nBnd].band_num ) );
	}

	CHKRESULT( GetPortComp() );

	LogFmtStrA(SPLOGLV_INFO, "%s: Load NV End", __FUNCTION__);
	return SP_OK;
}

SPRESULT CModV3Nv::LoadClcPorComp()
{
	LogFmtStrA(SPLOGLV_INFO, "%s: Load NV start", __FUNCTION__);

	CHKRESULT(LoadAndCheckFeatureSupport());
	CHKRESULT(LoadSupportBandInfo());

	for ( uint32 i = 0; i < m_nClcBand.size(); i++ )
	{
		int nBnd = 0;

		for ( ; nBnd < m_nMaxBandCount; nBnd++ )
		{
			if ( CLteUtility::m_BandInfo[m_nClcBand[i]].BandIdent == m_BandSupportInfo[nBnd].band_num )
			{
				break;
			}
		}

		if ( nBnd == m_nMaxBandCount )
		{
			UiSendMsg( "Not Support Band", LEVEL_UI, 1, 0, 1, CLteUtility::m_BandInfo[m_nClcBand[i]].NameA, -1, NULL, NULL );
			return SP_E_LTE_CALI_NOT_SUPPORT_BAND;
		}

		CHKRESULT( LoadDownloadAntMap( m_nClcBand[i], m_BandSupportInfo[nBnd].band_num ) );
	}

	CHKRESULT( GetPortComp() );

	LogFmtStrA(SPLOGLV_INFO, "%s: Load NV End", __FUNCTION__);
	return SP_OK;
}

SPRESULT CModV3Nv::ReadNv(  PC_MODEM_RF_V3_DATA_PARAM_T *pNvRsp,
	int eNvType,
	int nNvPos,
	int nRfChain,
	int nBwId,
	int nAnt,
	int nDataOffset,
	int nDataSize )
{
	LogFmtStrA(SPLOGLV_INFO, "%s: Read NV", __FUNCTION__);
	if (NULL == pNvRsp )
	{
		return SP_E_ALLOC_MEMORY;
	}

	LogFmtStrA(SPLOGLV_INFO, "%s: Read NV %s NV TYPE = %d, Band = %d, RfChain = %d", 
		__FUNCTION__, 
		CLteUtility::m_mapNvmLte.find(eNvType)->second, 
		eNvType, 
		nNvPos, 
		nRfChain);

	m_stNvmParam.NVM_Uninitialize();

	m_stNvmParam.stReqNv.eNvType = (uint16)eNvType;
	m_stNvmParam.stReqNv.BandId = (uint8)nNvPos;
	m_stNvmParam.stReqNv.HwChanID = (uint8)nRfChain;
	m_stNvmParam.stReqNv.BwId = (uint8)nBwId;
	m_stNvmParam.stReqNv.AntId = (uint8)nAnt;
	m_stNvmParam.stReqNv.DataOffset = (uint16)nDataOffset;
	m_stNvmParam.stReqNv.DataSize = (uint16)nDataSize;

	CHKRESULT(SP_ModemV3_Nv_Read( m_hDUT, &m_stNvmParam.stReqNv, &m_stNvmParam.stRspNv));
	//pNvRsp = &(m_stNvmParam.stRspNv);
	CopyMemory(pNvRsp,&m_stNvmParam.stRspNv,sizeof(PC_MODEM_RF_V3_DATA_PARAM_T));

	LogFmtStrA(SPLOGLV_INFO, "%s: NvDataSize = %d", __FUNCTION__, pNvRsp->DataSize );

	return SP_OK;
}

SPRESULT CModV3Nv::WriteNv( PC_MODEM_RF_V3_DATA_PARAM_T *pNvData, int eNvType, int BandId, int nDataSize, int nRfChain, int nAnt, int nDataOffset, int nBwId )
{
	LogFmtStrA(SPLOGLV_INFO, "%s: Write NV", __FUNCTION__);
	if (NULL == pNvData )
	{
		return SP_E_ALLOC_MEMORY;
	}

	LogFmtStrA(SPLOGLV_INFO, "%s: Write NV %s NV TYPE = %d, Band = %d, RfChain = %d", 
		__FUNCTION__, 
		CLteUtility::m_mapNvmLte.find(eNvType)->second, 
		eNvType, 
		BandId, 
		nRfChain);

	m_stNvmParam.NVM_Uninitialize();
	m_stNvmParam.stReqNv.eNvType = (uint16)eNvType;
	m_stNvmParam.stReqNv.AntId = (uint8)nAnt;
	m_stNvmParam.stReqNv.BandId = (uint8)BandId;
	m_stNvmParam.stReqNv.DataOffset = (uint16)nDataOffset;
	m_stNvmParam.stReqNv.DataSize = (uint16)nDataSize;
	m_stNvmParam.stReqNv.BwId = (uint8)nBwId;
	m_stNvmParam.stReqNv.HwChanID = (uint8)nRfChain;

	m_stNvmParam.stRspNv.DataSize = nDataSize;

	CopyMemory(&m_stNvmParam.stRspNv.nData, pNvData->nData, nDataSize);

	CHKRESULT(SP_ModemV3_Nv_Write( m_hDUT, &m_stNvmParam.stReqNv, &m_stNvmParam.stRspNv));

	LogFmtStrA(SPLOGLV_INFO, "%s: NvDataSize = %d", __FUNCTION__, pNvData->DataSize );

	return SP_OK;
}

SPRESULT CModV3Nv::InsertBand( uint8 nBand )
{
	Band_Info* pBandInfo = &m_arrBandInfo[m_nBandCnt];

	pBandInfo->nBand = (LTE_BAND_E)nBand;
	pBandInfo->nNumeral = CLteUtility::m_BandInfo[nBand].BandIdent;
	pBandInfo->nIndicator = CLteUtility::m_BandInfo[nBand].nIndicator;

	for (int i = 0; i < m_nMaxBandCount; i++)
	{
		if (m_BandSupportInfo[i].band_num == pBandInfo->nNumeral)
		{
			m_arrpDownload_BandInfo[m_nBandCnt] = m_BandSupportInfo[i];
			break;
		}
	}

	m_nBand[m_nBandCnt] = nBand;
	m_nBandCnt++;
	return SP_OK;
}

SPRESULT CModV3Nv::InitBandNv()
{
	m_arrBandSelected.clear();
	m_nBandCnt = 0;
	m_arrpDownload_BandInfo.resize(m_nMaxBandCount);
	m_arrBandInfo.resize(m_nMaxBandCount);

	CHKRESULT(ReadBandList());
	CHKRESULT(LoadSupportBandInfo());

	for (int i = 0; i < MAX_LTE_BAND; i++)
	{
		if (m_pFileConfig->m_arrBandEnable[i])
		{
			BOOL bSupport = FALSE;

			for (uint32 j = 0; j < m_BandListEnFlag.BandCount; j++)
			{
				if ((uint32)CLteUtility::m_BandInfo[i].BandIdent == m_BandListEnFlag.BandList.Band[j])
				{
					bSupport= TRUE;
					m_arrBandSelected.push_back(i);
					break;
				}
			}

			if (!bSupport)
			{
				UiSendMsg("Not Support Band", LEVEL_UI, 1, 0, 1, CLteUtility::m_BandInfo[i].NameA, -1, NULL, NULL);
				return SP_E_LTE_CALI_NOT_SUPPORT_BAND; 
			}
		}
	}

	if (m_arrBandSelected.size() == 0)
	{
		UiSendMsg("No Band Selected", LEVEL_UI, 1, 1, 1, NULL, -1, NULL, NULL);
		return SP_E_LTE_CALI_NO_BAND_SELECTED;
	}

	for (uint32 i = 0; i < m_arrBandSelected.size(); i++)
	{
		CHKRESULT(InsertBand((uint8)m_arrBandSelected[i]));
	}

	return SP_OK;
}

SPRESULT CModV3Nv::ReadBandList( void )
{
	PC_MODEM_RF_V3_LTE_BANDLIST_RSP_T stBandListRsp;
	ZeroMemory(&stBandListRsp, sizeof(PC_MODEM_RF_V3_LTE_BANDLIST_RSP_T));

	stBandListRsp.pBand = new uint8[MAX_NV_BAND_NUMBER_MV3];

	CHKRESULT(SP_ModemV3_LTE_Infor_Query(m_hDUT, &stBandListRsp, _msize(stBandListRsp.pBand)));

	m_BandListEnFlag.BandCount = stBandListRsp.nBandCnt;

	memcpy(&m_BandListEnFlag.BandList.Band[0], stBandListRsp.pBand, sizeof(NvData_LTE_BAND_LIST_INFOR_UINT));

	delete[] stBandListRsp.pBand;

	return SP_OK;
}

SPRESULT CModV3Nv::LoadSupportBandInfo( void )
{
	m_stNvRsp.NVM_Uninitialize();

	CHKRESULT(ReadNv( &m_stNvRsp.stRspNv, NVM_LTE_DOWNLOAD_BAND_INFOR ));
	memcpy(&m_BandSupportInfo[0], &m_stNvRsp.stRspNv.nData[0], sizeof(NvData_LTE_BAND_SUPPORT_INFO)*MAX_NV_BAND_NUMBER_MV3);
	return SP_OK;
}


SPRESULT CModV3Nv::GetPortComp( void )
{
	m_arrGS_PortComp.clear();
	READ_NV_PARAM_RLT_ANT_MAP portComp;
	int iStart = 0;
	for(;iStart < MAX_LTE_BAND;iStart++)
	{
		if((m_arrDownload_AntMap[iStart].Tx[0] != 0) 
			|| (m_arrDownload_AntMap[iStart].Rx[0] != 0)
			|| (m_arrDownload_AntMap[iStart].Rx[1] != 0))
		{
			portComp.Tx[0] = m_arrDownload_AntMap[iStart].Tx[0];
			portComp.Rx[0] = m_arrDownload_AntMap[iStart].Rx[0];
			portComp.Rx[1] = m_arrDownload_AntMap[iStart].Rx[1];
			m_arrGS_PortComp.push_back(portComp);
			break;
		}
	}

	for (int i = iStart; i < MAX_LTE_BAND; i++)
	{
		if((m_arrDownload_AntMap[i].Tx[0] == 0) 
			&& (m_arrDownload_AntMap[i].Rx[0] == 0)
			&& (m_arrDownload_AntMap[i].Rx[1] == 0))
		{
			continue;
		}

		uint8 iTxInsPort = m_arrDownload_AntMap[i].Tx[0];
		uint8 iRx0InsPort = m_arrDownload_AntMap[i].Rx[0];
		uint8 iRx1InsPort = m_arrDownload_AntMap[i].Rx[1];

		uint32 n = 0;
		for(;n < m_arrGS_PortComp.size();n++)
		{
			if((m_arrGS_PortComp[n].Tx[0] == iTxInsPort)
				&& (m_arrGS_PortComp[n].Rx[0] == iRx0InsPort) 
				&& (m_arrGS_PortComp[n].Rx[1] == iRx1InsPort))
			{
				break;
			}
		}
		if(n == m_arrGS_PortComp.size())
		{
			portComp.Tx[0] = iTxInsPort;
			portComp.Rx[0] = iRx0InsPort;
			portComp.Rx[1] = iRx1InsPort;
			m_arrGS_PortComp.push_back(portComp);
		}

	}

	return SP_OK;
}

SPRESULT CModV3Nv::LoadDownloadAntMap( int nBandIndex, int nBandPos )
{
	READ_NV_PARAM_RLT_ANT_MAP Map;
	CHKRESULT( SP_ReadAntMap( m_hDUT, RF_MODE_LTE, nBandPos, &Map) );
	CopyMemory( &m_arrDownload_AntMap[nBandIndex], &Map, sizeof( READ_NV_PARAM_RLT_ANT_MAP ) );

	for(int i = 0;i < MAX_LTE_RF_ANTENNA;i++)
	{
		if( m_arrDownload_AntMap[nBandIndex].Rx[i] > 0 )
		{
			m_arrDownload_AntMap[nBandIndex].Rx[i] -= 1;
		}
		if ( m_arrDownload_AntMap[nBandIndex].Tx[i] > 0 )
		{
			m_arrDownload_AntMap[nBandIndex].Tx[i] -= 1;
		}
	}

	return SP_OK;
}


SPRESULT CModV3Nv::LoadAndCheckFeatureSupport()
{
	LTE_VERSION_T NvExport_Version;
	CHKRESULT(SP_ModemV3_LTE_COMMOM_GetVersion(m_hDUT, &NvExport_Version));

	if(1 != NvExport_Version.CAL_LTE_ANT_MAP_REQ)
	{
		LogFmtStrA( SPLOGLV_ERROR, "%s: Not support atenna map feature !", __FUNCTION__ );
		return SP_E_LTE_CALI_INVALID_CALI_VERSION;
	}

	m_bBandAdaptation = (BOOL)NvExport_Version.CAL_LTE_Band_Adaptation;

	return SP_OK;
}
