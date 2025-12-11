#include "StdAfx.h"
#include "ModV4Nv.h"
#include "LteUtility.h"

CModV4Nv::CModV4Nv( LPCWSTR lpName, CFuncCenter* pFuncCenter ) : CFuncBase(lpName, pFuncCenter)
{
	m_nMaxBandCount = MAX_NV_BAND_NUMBER_MV3;
	m_hDUT = NULL;
	m_stNvmParam.NVM_Uninitialize();

	ZeroMemory(m_nBand, sizeof(uint8)*MAX_NV_BAND_NUMBER_MV3);
	ZeroMemory(&m_BandSupportInfo, sizeof(NvData_LTE_BAND_SUPPORT_INFO)*MAX_NV_BAND_NUMBER_MV3);
	ZeroMemory(&m_BandListEnFlag, sizeof(NvData_LTE_BAND_LIST_INFOR));
    ZeroMemory(m_BandPathInfo, sizeof(NvData_LTE_BAND_RF_PATH) * MAX_NV_BAND_NUMBER_MV3);

	m_pFileConfig = NULL;
	m_nBandCnt = 0;
	m_nCaliParamVersion = 0;
	m_stNvRsp.NVM_Uninitialize();
	m_stProgramNvData.NVM_Uninitialize();

	m_arrGS_PortComp.clear();
	m_arrNvPos.clear();

	m_bBandAdaptation = FALSE;
}

CModV4Nv::~CModV4Nv(void)
{

}

SPRESULT CModV4Nv::PreInit()
{
	CHKRESULT(__super::PreInit());

	m_hDUT = m_pSpatBase->m_hDUT;
	m_pFileConfig = (CFileConfig*)m_pFuncCenter->GetFunc(FUNC_FILE_CONFIG);

	return SP_OK;
}

SPRESULT CModV4Nv::ParsePathInfo(int nIndex,LTE_BAND_E Band)
{
    LogFmtStrA(SPLOGLV_INFO, "%s, %s", __FUNCTION__, CLteUtility::m_BandInfo[Band].NameA);

    LTE_RF_PATH_INFO_EX_T Download_PathInfo;
    memset(&Download_PathInfo, 0, sizeof(Download_PathInfo));
    CHKRESULT(SP_LTE_ModemV4_Load_Pathinfo(m_pSpatBase->m_hDUT, &Download_PathInfo, Band, TRUE));

    NvData_LTE_ANT_MAP Download_AntMap;
    memset(&Download_AntMap, 0, sizeof(Download_AntMap));
    CHKRESULT(SP_LTE_ModemV4_Load_AntMap(m_pSpatBase->m_hDUT, &Download_AntMap, Band, FALSE));

    LTE_BAND_PATH_INFO* pInfo = &m_arrPathInfo[nIndex];
    pInfo->TriggerPath.bTx = FALSE;

    for (int i = 0; i < 2; i++) // 2 Path Type
    {
        LogFmtStrA(SPLOGLV_INFO, "%drx: Path Count = %d", i * 2 + 2, Download_PathInfo.Path[i].Count);
        pInfo->Type[i].PathCount = Download_PathInfo.Path[i].Count;

        for (int j = 0; j < Download_PathInfo.Path[i].Count; j++)
        {
            pInfo->Type[i].Path[j].channel_id = Download_PathInfo.Path[i].Element[j].channel_id;

            for (int k = 0; k < i * 2 + 2; k++) // Ant
            {
                uint8 rx_ant = k;
                uint8 tx_ant = k;

                if (m_bPathAntEnable)
                {
                    if (Download_PathInfo.Path[i].Element[j].reserved)
                    {
                        rx_ant = Download_PathInfo.Path[i].Element[j].actual_rx_ant_index[k] & 0xF;
                        tx_ant = Download_PathInfo.Path[i].Element[j].actual_tx_ant_index[k] & 0xF;
                        pInfo->Type[i].Path[j].RX[k].RxInsPort = (Download_PathInfo.Path[i].Element[j].actual_rx_ant_index[k] >> 4) - 1;
                        pInfo->Type[i].Path[j].TX[k].TxInsPort = (Download_PathInfo.Path[i].Element[j].actual_tx_ant_index[k] >> 4) - 1;

                        if (rx_ant > 3 || tx_ant > 3)
                        {
                            LogFmtStrA(SPLOGLV_ERROR, "%s: rx_ant:%d > 3 tx_ant:%d > 3", __FUNCTION__, rx_ant, tx_ant);
                            return SP_E_INVALID_PARAMETER;
                        }
                        LogFmtStrA(SPLOGLV_INFO, "%s: rx_ant:%d tx_ant:%d rx_rfant:%d tx_rfant:%d"
                            , __FUNCTION__, rx_ant, tx_ant, pInfo->Type[i].Path[j].RX[k].RxInsPort, pInfo->Type[i].Path[j].TX[k].TxInsPort);
                    }
                    else
                    {
                        rx_ant = Download_PathInfo.Path[i].Element[j].actual_rx_ant_index[k];
                        tx_ant = Download_PathInfo.Path[i].Element[j].actual_tx_ant_index[k];
                        pInfo->Type[i].Path[j].RX[k].RxInsPort = Download_AntMap.Rx[rx_ant];
                        pInfo->Type[i].Path[j].TX[k].TxInsPort = Download_AntMap.Tx[tx_ant];

                        if (rx_ant > 3 || tx_ant > 3)
                        {
                            LogFmtStrA(SPLOGLV_ERROR, "%s: rx_ant:%d > 3 tx_ant:%d > 3", __FUNCTION__, rx_ant, tx_ant);
                            return SP_E_INVALID_PARAMETER;
                        }
                    }

                }

                pInfo->Type[i].Path[j].RX[k].Ant = (LTE_RF_ANTENNA_E)k;
                pInfo->Type[i].Path[j].RX[k].AntMapIndex = (LTE_RF_ANTENNA_E)rx_ant;
                pInfo->Type[i].Path[j].RX[k].CaliEnable = (Download_PathInfo.Path[i].Element[j].rx_cali_en >> k) & 0x1;
                pInfo->Type[i].Path[j].RX[k].CaliSaveEnable = (Download_PathInfo.Path[i].Element[j].rx_cali_save_en >> k) & 0x1;
                pInfo->Type[i].Path[j].RX[k].CaliSaveIndex = Download_PathInfo.Path[i].Element[j].rx_cali_save_index[k];
                pInfo->Type[i].Path[j].RX[k].NvAnt = (LTE_RF_ANTENNA_E)(pInfo->Type[i].Path[j].RX[k].CaliSaveIndex % 2);//& 0x1);
                pInfo->Type[i].Path[j].RX[k].NvChannel = (RF_LTE_CHAIN_V4_E)(pInfo->Type[i].Path[j].RX[k].CaliSaveIndex / 2);
                pInfo->Type[i].Path[j].RX[k].b2rx = (0 == i) ? TRUE : FALSE;

                if (pInfo->Type[0].Path[0].TX[0].TxInsPort < 0 || pInfo->Type[0].Path[0].TX[0].TxInsPort >= MAX_RF_ANT)
                {
                    LogFmtStrA(SPLOGLV_ERROR, "%s: Tx InsPort Fail = %d", __FUNCTION__, pInfo->Type[0].Path[0].TX[0].TxInsPort);
                    return SP_E_INVALID_PARAMETER;
                }
                pInfo->Type[i].Path[j].RX[k].TxTriggerPort = pInfo->Type[0].Path[0].TX[0].TxInsPort;

                pInfo->Type[i].Path[j].TX[k].Ant = (LTE_RF_ANTENNA_E)k;
                pInfo->Type[i].Path[j].TX[k].AntMapIndex = (LTE_RF_ANTENNA_E)tx_ant;
                pInfo->Type[i].Path[j].TX[k].CaliEnable = (Download_PathInfo.Path[i].Element[j].tx_cali_en >> k) & 0x1;
                pInfo->Type[i].Path[j].TX[k].CaliSaveEnable = (Download_PathInfo.Path[i].Element[j].tx_cali_save_en >> k) & 0x1;
                pInfo->Type[i].Path[j].TX[k].CaliSaveIndex = Download_PathInfo.Path[i].Element[j].tx_cali_save_index[k];
                pInfo->Type[i].Path[j].TX[k].NvAnt = (LTE_RF_ANTENNA_E)0;
                pInfo->Type[i].Path[j].TX[k].NvChannel = (RF_LTE_CHAIN_V4_E)pInfo->Type[i].Path[j].TX[k].CaliSaveIndex;

                if (pInfo->Type[i].Path[j].RX[k].b2rx && 0 == pInfo->Type[i].Path[j].TX[k].CaliSaveIndex &&
                    pInfo->Type[i].Path[j].TX[k].CaliEnable && pInfo->Type[i].Path[j].TX[k].Ant == 0)
                {
                    pInfo->TriggerPath.Trigger_Ant = pInfo->Type[i].Path[j].TX[k].Ant;
                    pInfo->TriggerPath.Trigger_channel_id = pInfo->Type[i].Path[j].channel_id;
                    pInfo->TriggerPath.bTx = TRUE;
                }

                LogFmtStrA(SPLOGLV_INFO, "%s: channel_id:%d, rx_cali_en:%d rx_cali_save_en:%d rx_cali_save_index:%d",
                    __FUNCTION__, pInfo->Type[i].Path[j].channel_id, pInfo->Type[i].Path[j].RX[k].CaliEnable,
                    pInfo->Type[i].Path[j].RX[k].CaliSaveEnable, pInfo->Type[i].Path[j].RX[k].CaliSaveIndex);

                LogFmtStrA(SPLOGLV_INFO, "%s: channel_id:%d, tx_cali_en:%d tx_cali_save_en:%d tx_cali_save_index:%d",
                    __FUNCTION__, pInfo->Type[i].Path[j].channel_id, pInfo->Type[i].Path[j].TX[k].CaliEnable,
                    pInfo->Type[i].Path[j].TX[k].CaliSaveEnable, pInfo->Type[i].Path[j].TX[k].CaliSaveIndex);

                LogFmtStrA(SPLOGLV_INFO, "%s: rx_ant:%d tx_ant:%d", __FUNCTION__, rx_ant, tx_ant);
                if (m_bNewPathStructure)
                {
                    if (pInfo->Type[i].Path[j].TX[k].NvChannel > 2 && pInfo->Type[i].Path[j].TX[k].CaliEnable)
                    {
                        return SP_E_INVALID_PARAMETER;
                    }
                }
                else
                {
                    if (pInfo->Type[i].Path[j].TX[k].NvChannel > 1 && pInfo->Type[i].Path[j].TX[k].CaliEnable)
                    {
                        return SP_E_INVALID_PARAMETER;
                    }
                }
            }
        }
    }

    return SP_OK;
}

SPRESULT CModV4Nv::Load()
{
	LogFmtStrA(SPLOGLV_INFO, "%s: Load NV start", __FUNCTION__);

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

        NvData_LTE_VERSION Download_Version;
        memset(&Download_Version, 0, sizeof(Download_Version));
        CHKRESULT(SP_LTE_ModemV4_Load_Version(m_pSpatBase->m_hDUT, &Download_Version, TRUE));

        m_bBandAdaptation = (BOOL)Download_Version.CAL_LTE_Band_Adaptation;
        m_bPathAntEnable = (BOOL)Download_Version.CAL_LTE_QUERY_CH_REQ_NEW;
        m_bNewPathStructure = (BOOL)Download_Version.CAL_LTE_QUERY_CH_REQ_V3;

        LogFmtStrA(SPLOGLV_INFO, "%s: Load NV m_bPathAntEnable: %d", __FUNCTION__, (int)m_bPathAntEnable);

        CHKRESULT(SP_LTE_ModemV4_Load_CalReserved(m_pSpatBase->m_hDUT, &m_Calibration_Reserved[(LTE_BAND_E)m_nBand[i]], (LTE_BAND_E)m_nBand[i], FALSE));
        CHKRESULT(ParsePathInfo(i,(LTE_BAND_E)m_nBand[i]));

		CHKRESULT(ReadBandRfChainV4(i, CLteUtility::m_BandInfo[m_nBand[i]].BandIdent));

		CHKRESULT(ConfigBandRfChainEnV4(i, nBnd))
	}

	CHKRESULT( GetPortComp() );

	LogFmtStrA(SPLOGLV_INFO, "%s: Load NV End", __FUNCTION__);
	return SP_OK;
}

SPRESULT CModV4Nv::LoadClcPorComp()
{
    LogFmtStrA(SPLOGLV_INFO, "%s: Load NV start", __FUNCTION__);

    CHKRESULT(LoadAndCheckFeatureSupport());
    CHKRESULT(LoadSupportBandInfo());

    for (uint32 i = 0; i < m_nClcBand.size(); i++)
    {
        int nBnd = 0;

        for (; nBnd < m_nMaxBandCount; nBnd++)
        {
            if (CLteUtility::m_BandInfo[m_nClcBand[i]].BandIdent == m_BandSupportInfo[nBnd].band_num)
            {
                break;
            }
        }

        if (nBnd == m_nMaxBandCount)
        {
            UiSendMsg("Not Support Band", LEVEL_UI, 1, 0, 1, CLteUtility::m_BandInfo[m_nClcBand[i]].NameA, -1, NULL, NULL);
            return SP_E_LTE_CALI_NOT_SUPPORT_BAND;
        }

        NvData_LTE_VERSION Download_Version;
        memset(&Download_Version, 0, sizeof(Download_Version));
        CHKRESULT(SP_LTE_ModemV4_Load_Version(m_pSpatBase->m_hDUT, &Download_Version, TRUE));

        m_bBandAdaptation = (BOOL)Download_Version.CAL_LTE_Band_Adaptation;
        m_bPathAntEnable = (BOOL)Download_Version.CAL_LTE_QUERY_CH_REQ_NEW;
		m_bNewPathStructure = (BOOL)Download_Version.CAL_LTE_QUERY_CH_REQ_V3;
        LogFmtStrA(SPLOGLV_INFO, "%s: Load NV m_bPathAntEnable: %d", __FUNCTION__, (int)m_bPathAntEnable);

        CHKRESULT(SP_LTE_ModemV4_Load_CalReserved(m_pSpatBase->m_hDUT, &m_Calibration_Reserved[(LTE_BAND_E)m_nClcBand[i]], (LTE_BAND_E)m_nClcBand[i], FALSE));
        CHKRESULT(ParsePathInfo(i, (LTE_BAND_E)m_nClcBand[i]));


        CHKRESULT(LoadDownloadAntMap(m_nClcBand[i], m_BandSupportInfo[nBnd].band_num));

        CHKRESULT(ReadBandRfChainV4(i, CLteUtility::m_BandInfo[m_nClcBand[i]].BandIdent));

        CHKRESULT(ConfigBandRfChainEnV4(i, nBnd))
    }

    CHKRESULT(GetPortComp());

    LogFmtStrA(SPLOGLV_INFO, "%s: Load NV End", __FUNCTION__);
    return SP_OK;
}

SPRESULT CModV4Nv::ReadNv(  PC_MODEM_RF_V3_DATA_PARAM_T *pNvRsp,
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

SPRESULT CModV4Nv::WriteNv( PC_MODEM_RF_V3_DATA_PARAM_T *pNvData, int eNvType, int BandId, int nDataSize, int nRfChain, int nAnt, int nDataOffset, int nBwId )
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

SPRESULT CModV4Nv::InsertBand( uint8 nBand )
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

SPRESULT CModV4Nv::InitBandNv()
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

SPRESULT CModV4Nv::ReadBandList( void )
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

SPRESULT CModV4Nv::LoadSupportBandInfo( void )
{
	m_stNvRsp.NVM_Uninitialize();

	CHKRESULT(ReadNv( &m_stNvRsp.stRspNv, NVM_LTE_DOWNLOAD_BAND_INFOR ));
	memcpy(&m_BandSupportInfo[0], &m_stNvRsp.stRspNv.nData[0], sizeof(NvData_LTE_BAND_SUPPORT_INFO)*MAX_NV_BAND_NUMBER_MV3);
	return SP_OK;
}


SPRESULT CModV4Nv::GetPortComp( void )
{
    m_arrGS_PortComp.clear();
    READ_NV_PARAM_RLT_ANT_MAP portComp;
    int iStart = 0;
    for (; iStart < MAX_LTE_BAND; iStart++)
    {
        if ((m_arrDownload_AntMap[iStart].Tx[0] != 0)
            || (m_arrDownload_AntMap[iStart].Rx[0] != 0)
            || (m_arrDownload_AntMap[iStart].Rx[1] != 0)
            || (m_arrDownload_AntMap[iStart].Rx[2] != 0)
            || (m_arrDownload_AntMap[iStart].Rx[3] != 0))
        {
            portComp.Tx[0] = m_arrDownload_AntMap[iStart].Tx[0];
            portComp.Rx[0] = m_arrDownload_AntMap[iStart].Rx[0];
            portComp.Rx[1] = m_arrDownload_AntMap[iStart].Rx[1];
            portComp.Rx[2] = m_arrDownload_AntMap[iStart].Rx[2];
            portComp.Rx[3] = m_arrDownload_AntMap[iStart].Rx[3];
            m_arrGS_PortComp.push_back(portComp);
            break;
        }
    }

    for (int i = iStart; i < MAX_LTE_BAND; i++)
    {
        if ((m_arrDownload_AntMap[i].Tx[0] == 0)
            && (m_arrDownload_AntMap[i].Rx[0] == 0)
            && (m_arrDownload_AntMap[i].Rx[1] == 0)
            && (m_arrDownload_AntMap[i].Rx[2] == 0)
            && (m_arrDownload_AntMap[i].Rx[3] == 0))
        {
            continue;
        }

        uint8 iTxInsPort = m_arrDownload_AntMap[i].Tx[0];
        uint8 iRx0InsPort = m_arrDownload_AntMap[i].Rx[0];
        uint8 iRx1InsPort = m_arrDownload_AntMap[i].Rx[1];
        uint8 iRx2InsPort = m_arrDownload_AntMap[i].Rx[2];
        uint8 iRx3InsPort = m_arrDownload_AntMap[i].Rx[3];

        uint32 n = 0;
        for (; n < m_arrGS_PortComp.size(); n++)
        {
            if ((m_arrGS_PortComp[n].Tx[0] == iTxInsPort)
                && (m_arrGS_PortComp[n].Rx[0] == iRx0InsPort)
                && (m_arrGS_PortComp[n].Rx[1] == iRx1InsPort)
                && (m_arrGS_PortComp[n].Rx[2] == iRx2InsPort)
                && (m_arrGS_PortComp[n].Rx[3] == iRx3InsPort))
            {
                break;
            }
        }
        if (n == m_arrGS_PortComp.size())
        {
            portComp.Tx[0] = iTxInsPort;
            portComp.Rx[0] = iRx0InsPort;
            portComp.Rx[1] = iRx1InsPort;
            portComp.Rx[2] = iRx2InsPort;
            portComp.Rx[3] = iRx3InsPort;
            m_arrGS_PortComp.push_back(portComp);
        }

    }

	return SP_OK;
}

SPRESULT CModV4Nv::LoadDownloadAntMap( int nBandIndex, int nBandPos )
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


SPRESULT CModV4Nv::LoadAndCheckFeatureSupport()
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

SPRESULT CModV4Nv::ReadBandRfChainV4(int nIndex, LTE_BAND_IDENT_E Band)
{
	PC_MODEM_RF_V4_LTE_CH_REQ_CMD_T req;
	ZeroMemory(&req, sizeof(req));
	PC_MODEM_RF_V4_LTE_CH_RSP_T MODEM_RF_V4_LTE_CH_RSP;
	ZeroMemory(&MODEM_RF_V4_LTE_CH_RSP, sizeof(MODEM_RF_V4_LTE_CH_RSP));
	req.nBandNum = 1;
	req.bandList[0] = Band;
	CHKRESULT(SP_ModemV4_Query_CH(m_hDUT, &req, &MODEM_RF_V4_LTE_CH_RSP));

	CopyMemory(&m_BandPathInfo[nIndex].RFChannel, &MODEM_RF_V4_LTE_CH_RSP.RFChannel[0], sizeof(RF_LTE_CALI_RF_CH_T));

	//m_BandPathInfo[nIndex].Band = nBand;

	return SP_OK;
}

SPRESULT CModV4Nv::ConfigBandRfChainEnV4(const unsigned int nBandIndex, int nNvPos)
{
    m_BandPathInfoEn[nBandIndex].NvPos = (uint8)nNvPos;

    uint8 cnt_2rx = (uint8)m_BandPathInfo[nBandIndex].RFChannel.cnt_2rx;
    uint8 cnt_4rx = (uint8)m_BandPathInfo[nBandIndex].RFChannel.cnt_4rx;

    Band_RF_CHAIN_PATH_T temp;

    for (uint8 i = 0; i < cnt_2rx; i++)
    {
        for (uint8 j = 0; j < 2; j++)
        {
            bool rx_cali_en = (m_BandPathInfo[nBandIndex].RFChannel.ch_2rx[i].rx_cali_en >> j) & 0x1;
            bool rx_cali_save_en = (m_BandPathInfo[nBandIndex].RFChannel.ch_2rx[i].rx_cali_save_en >> j) & 0x1;
            uint8 rx_cali_save_index = m_BandPathInfo[nBandIndex].RFChannel.ch_2rx[i].rx_cali_save_index[j];

            bool  tx_cali_en = (m_BandPathInfo[nBandIndex].RFChannel.ch_2rx[i].tx_cali_en >> j) & 0x1;
            bool tx_cali_save_en = (m_BandPathInfo[nBandIndex].RFChannel.ch_2rx[i].tx_cali_save_en >> j) & 0x1;
            uint8 tx_cali_save_index = m_BandPathInfo[nBandIndex].RFChannel.ch_2rx[i].tx_cali_save_index[j];

            if ((rx_cali_en)&&(rx_cali_save_en))
            {
                m_BandPathInfoEn[nBandIndex].RxAnt.antRxPathNum[m_arrPathInfo[nBandIndex].Type[0].Path[i].RX[j].AntMapIndex] += 1;
                switch (m_arrPathInfo[nBandIndex].Type[0].Path[i].RX[j].AntMapIndex)
                {
                case 0:
                    m_BandPathInfoEn[nBandIndex].RxChain.bChannelEn[RF_CHAINNAL0] = TRUE;
                    m_BandPathInfoEn[nBandIndex].RxChain.bChannelPriEn[RF_CHAINNAL0] = TRUE;
                    m_BandPathInfoEn[nBandIndex].RxChain.bChannelPriEnSave[RF_CHAINNAL0] = m_BandPathInfoEn[nBandIndex].RxChain.bChannelPriEnSave[RF_CHAINNAL0] || rx_cali_save_en;
                    m_BandPathInfoEn[nBandIndex].RxChain.ChannelPriPathNum[RF_CHAINNAL0] += 1;
                    temp.AntIndex = j;
                    temp.bSaveNvEn = rx_cali_save_en;
                    temp.rf_channel_id = m_BandPathInfo[nBandIndex].RFChannel.ch_2rx[i].rf_channel_id;
                    temp.b2rx = TRUE;
                    temp.channel = 0;
                    temp.priordiv = 0;
                    m_BandPathInfoEn[nBandIndex].RxChain.priPathinfo[RF_CHAINNAL0].push_back(temp);

                    break;
                case 1:
                    m_BandPathInfoEn[nBandIndex].RxChain.bChannelEn[RF_CHAINNAL1] = TRUE;
                    m_BandPathInfoEn[nBandIndex].RxChain.bChannelDivEn[RF_CHAINNAL1] = TRUE;
                    m_BandPathInfoEn[nBandIndex].RxChain.bChannelDivEnSave[RF_CHAINNAL1] = m_BandPathInfoEn[nBandIndex].RxChain.bChannelDivEnSave[RF_CHAINNAL0] || rx_cali_save_en;
                    m_BandPathInfoEn[nBandIndex].RxChain.ChannelDivPathNum[RF_CHAINNAL1] += 1;
                    temp.AntIndex = j;
                    temp.bSaveNvEn = rx_cali_save_en;
                    temp.rf_channel_id = m_BandPathInfo[nBandIndex].RFChannel.ch_2rx[i].rf_channel_id;
                    temp.b2rx = TRUE;
                    temp.channel = 0;
                    temp.priordiv = 1;
                    m_BandPathInfoEn[nBandIndex].RxChain.divPathinfo[RF_CHAINNAL1].push_back(temp);
                    break;
                case 2:
                    m_BandPathInfoEn[nBandIndex].RxChain.bChannelEn[RF_CHAINNAL2] = TRUE;
                    m_BandPathInfoEn[nBandIndex].RxChain.bChannelPriEn[RF_CHAINNAL2] = TRUE;
                    m_BandPathInfoEn[nBandIndex].RxChain.bChannelPriEnSave[RF_CHAINNAL2] = m_BandPathInfoEn[nBandIndex].RxChain.bChannelPriEnSave[RF_CHAINNAL1] || rx_cali_save_en;
                    m_BandPathInfoEn[nBandIndex].RxChain.ChannelPriPathNum[RF_CHAINNAL2] += 1;
                    temp.AntIndex = j;
                    temp.bSaveNvEn = rx_cali_save_en;
                    temp.rf_channel_id = m_BandPathInfo[nBandIndex].RFChannel.ch_2rx[i].rf_channel_id;
                    temp.b2rx = TRUE;
                    temp.channel = 1;
                    temp.priordiv = 0;
                    m_BandPathInfoEn[nBandIndex].RxChain.priPathinfo[RF_CHAINNAL2].push_back(temp);
                    break;
                case 3:
                    m_BandPathInfoEn[nBandIndex].RxChain.bChannelEn[RF_CHAINNAL3] = TRUE;
                    m_BandPathInfoEn[nBandIndex].RxChain.bChannelDivEn[RF_CHAINNAL3] = TRUE;
                    m_BandPathInfoEn[nBandIndex].RxChain.bChannelDivEnSave[RF_CHAINNAL3] = m_BandPathInfoEn[nBandIndex].RxChain.bChannelDivEnSave[RF_CHAINNAL1] || rx_cali_save_en;
                    m_BandPathInfoEn[nBandIndex].RxChain.ChannelDivPathNum[RF_CHAINNAL3] += 1;
                    temp.AntIndex = j;
                    temp.bSaveNvEn = rx_cali_save_en;
                    temp.rf_channel_id = m_BandPathInfo[nBandIndex].RFChannel.ch_2rx[i].rf_channel_id;
                    temp.b2rx = TRUE;
                    temp.channel = 1;
                    temp.priordiv = 1;
                    m_BandPathInfoEn[nBandIndex].RxChain.divPathinfo[RF_CHAINNAL3].push_back(temp);
                    break;
                default:
                    LogFmtStrA(SPLOGLV_ERROR, "Unsupported Flag Type");
                }
                m_BandPathInfoEn[nBandIndex].RxAnt.rxPathinfo[j].push_back(temp);
            }

            if ((tx_cali_en)&&(tx_cali_save_en))
            {
                m_BandPathInfoEn[nBandIndex].TxAnt.antTxPathNum[j] += 1;
                switch (m_arrPathInfo[nBandIndex].Type[0].Path[i].TX[j].AntMapIndex)
                {
                case 0:
                    m_BandPathInfoEn[nBandIndex].TxChain.bChannelEn[RF_CHAINNAL0] = TRUE;
                    m_BandPathInfoEn[nBandIndex].TxChain.bChannelPriEn[RF_CHAINNAL0] = TRUE;
                    m_BandPathInfoEn[nBandIndex].TxChain.bChannelPriEnSave[RF_CHAINNAL0] = m_BandPathInfoEn[nBandIndex].TxChain.bChannelPriEnSave[RF_CHAINNAL0] || tx_cali_save_en;
                    m_BandPathInfoEn[nBandIndex].TxChain.ChannelPriPathNum[RF_CHAINNAL0] += 1;
                    temp.AntIndex = j;
                    temp.bSaveNvEn = tx_cali_save_en;
                    temp.rf_channel_id = m_BandPathInfo[nBandIndex].RFChannel.ch_2rx[i].rf_channel_id;
                    temp.b2rx = TRUE;
                    temp.channel = 0;
                    temp.priordiv = 0;
                    m_BandPathInfoEn[nBandIndex].TxChain.priPathinfo[RF_CHAINNAL0].push_back(temp);
                    m_BandPathInfoEn[nBandIndex].triggerPath.push_back(temp);
                    break;
                case 1:
                    m_BandPathInfoEn[nBandIndex].TxChain.bChannelEn[RF_CHAINNAL1] = TRUE;
                    m_BandPathInfoEn[nBandIndex].TxChain.bChannelPriEn[RF_CHAINNAL1] = TRUE;
                    m_BandPathInfoEn[nBandIndex].TxChain.bChannelPriEnSave[RF_CHAINNAL1] = m_BandPathInfoEn[nBandIndex].TxChain.bChannelPriEnSave[RF_CHAINNAL1] || tx_cali_save_en;
                    m_BandPathInfoEn[nBandIndex].TxChain.ChannelPriPathNum[RF_CHAINNAL1] += 1;
                    temp.AntIndex = j;
                    temp.bSaveNvEn = tx_cali_save_en;
                    temp.rf_channel_id = m_BandPathInfo[nBandIndex].RFChannel.ch_2rx[i].rf_channel_id;
                    temp.b2rx = TRUE;
                    temp.channel = 1;
                    temp.priordiv = 0;
                    m_BandPathInfoEn[nBandIndex].TxChain.priPathinfo[RF_CHAINNAL1].push_back(temp);
                    break;
                default:
                    LogFmtStrA(SPLOGLV_ERROR, "Unsupported Flag Type");
                }
                m_BandPathInfoEn[nBandIndex].TxAnt.txPathinfo[j].push_back(temp);
            }

        }
    }

    for (uint8 i = 0; i < cnt_4rx; i++)
    {
        for (uint8 j = 0; j < 4; j++)
        {
            bool rx_cali_en = (m_BandPathInfo[nBandIndex].RFChannel.ch_4rx[i].rx_cali_en >> j) & 0x1;
            bool rx_cali_save_en = (m_BandPathInfo[nBandIndex].RFChannel.ch_4rx[i].rx_cali_save_en >> j) & 0x1;
            uint8 rx_cali_save_index = m_BandPathInfo[nBandIndex].RFChannel.ch_4rx[i].rx_cali_save_index[j];
            
            uint8 tx_cali_save_index = 0;
            if (j < 2)
            {
                tx_cali_save_index = m_BandPathInfo[nBandIndex].RFChannel.ch_4rx[i].tx_cali_save_index[j];
            }
            else
            {
                tx_cali_save_index = 0;
            }
            bool tx_cali_en = (m_BandPathInfo[nBandIndex].RFChannel.ch_4rx[i].tx_cali_en >> j) & 0x1;
            bool tx_cali_save_en = (m_BandPathInfo[nBandIndex].RFChannel.ch_4rx[i].tx_cali_save_en >> j) & 0x1;
            

            if ((rx_cali_en) && (rx_cali_save_en))
            {
                m_BandPathInfoEn[nBandIndex].RxAnt.antRxPathNum[m_arrPathInfo[nBandIndex].Type[1].Path[i].RX[j].AntMapIndex] += 1;
                switch (m_arrPathInfo[nBandIndex].Type[1].Path[i].RX[j].AntMapIndex)
                {
                case 0:
                    m_BandPathInfoEn[nBandIndex].RxChain.bChannelEn[RF_CHAINNAL0] = TRUE;
                    m_BandPathInfoEn[nBandIndex].RxChain.bChannelPriEn[RF_CHAINNAL0] = TRUE;
                    m_BandPathInfoEn[nBandIndex].RxChain.bChannelPriEnSave[RF_CHAINNAL0] = m_BandPathInfoEn[nBandIndex].RxChain.bChannelPriEnSave[RF_CHAINNAL0] || rx_cali_save_en;
                    m_BandPathInfoEn[nBandIndex].RxChain.ChannelPriPathNum[RF_CHAINNAL0] += 1;
                    temp.AntIndex = j;
                    temp.bSaveNvEn = rx_cali_save_en;
                    temp.rf_channel_id = m_BandPathInfo[nBandIndex].RFChannel.ch_4rx[i].rf_channel_id;
                    temp.b2rx = FALSE;
                    temp.channel = 0;
                    temp.priordiv = 0;
                    m_BandPathInfoEn[nBandIndex].RxChain.priPathinfo[RF_CHAINNAL0].push_back(temp);
                    break;
                case 1:
                    m_BandPathInfoEn[nBandIndex].RxChain.bChannelEn[RF_CHAINNAL1] = TRUE;
                    m_BandPathInfoEn[nBandIndex].RxChain.bChannelDivEn[RF_CHAINNAL1] = TRUE;
                    m_BandPathInfoEn[nBandIndex].RxChain.bChannelDivEnSave[RF_CHAINNAL1] = m_BandPathInfoEn[nBandIndex].RxChain.bChannelDivEnSave[RF_CHAINNAL1] || rx_cali_save_en;
                    m_BandPathInfoEn[nBandIndex].RxChain.ChannelDivPathNum[RF_CHAINNAL1] += 1;
                    temp.AntIndex = j;
                    temp.bSaveNvEn = rx_cali_save_en;
                    temp.rf_channel_id = m_BandPathInfo[nBandIndex].RFChannel.ch_4rx[i].rf_channel_id;
                    temp.b2rx = FALSE;
                    temp.channel = 0;
                    temp.priordiv = 1;
                    m_BandPathInfoEn[nBandIndex].RxChain.divPathinfo[RF_CHAINNAL1].push_back(temp);
                    break;
                case 2:
                    m_BandPathInfoEn[nBandIndex].RxChain.bChannelEn[RF_CHAINNAL2] = TRUE;
                    m_BandPathInfoEn[nBandIndex].RxChain.bChannelPriEn[RF_CHAINNAL2] = TRUE;
                    m_BandPathInfoEn[nBandIndex].RxChain.bChannelPriEnSave[RF_CHAINNAL2] = m_BandPathInfoEn[nBandIndex].RxChain.bChannelPriEnSave[RF_CHAINNAL2] || rx_cali_save_en;
                    m_BandPathInfoEn[nBandIndex].RxChain.ChannelPriPathNum[RF_CHAINNAL2] += 1;
                    temp.AntIndex = j;
                    temp.bSaveNvEn = rx_cali_save_en;
                    temp.rf_channel_id = m_BandPathInfo[nBandIndex].RFChannel.ch_4rx[i].rf_channel_id;
                    temp.b2rx = FALSE;
                    temp.channel = 1;
                    temp.priordiv = 0;
                    m_BandPathInfoEn[nBandIndex].RxChain.priPathinfo[RF_CHAINNAL2].push_back(temp);
                    break;
                case 3:
                    m_BandPathInfoEn[nBandIndex].RxChain.bChannelEn[RF_CHAINNAL3] = TRUE;
                    m_BandPathInfoEn[nBandIndex].RxChain.bChannelDivEn[RF_CHAINNAL3] = TRUE;
                    m_BandPathInfoEn[nBandIndex].RxChain.bChannelDivEnSave[RF_CHAINNAL3] = m_BandPathInfoEn[nBandIndex].RxChain.bChannelDivEnSave[RF_CHAINNAL3] || rx_cali_save_en;
                    m_BandPathInfoEn[nBandIndex].RxChain.ChannelDivPathNum[RF_CHAINNAL3] += 1;
                    temp.AntIndex = j;
                    temp.bSaveNvEn = rx_cali_save_en;
                    temp.rf_channel_id = m_BandPathInfo[nBandIndex].RFChannel.ch_4rx[i].rf_channel_id;
                    temp.b2rx = FALSE;
                    temp.channel = 1;
                    temp.priordiv = 1;
                    m_BandPathInfoEn[nBandIndex].RxChain.divPathinfo[RF_CHAINNAL3].push_back(temp);
                    break;
                default:
                    LogFmtStrA(SPLOGLV_ERROR, "Unsupported Flag Type");
                }
                m_BandPathInfoEn[nBandIndex].RxAnt.rxPathinfo[j].push_back(temp);
            }

            if ((tx_cali_en) && (tx_cali_save_en))
            {
                m_BandPathInfoEn[nBandIndex].TxAnt.antTxPathNum[m_arrPathInfo[nBandIndex].Type[1].Path[i].RX[j].AntMapIndex] += 1;
                switch (m_arrPathInfo[nBandIndex].Type[1].Path[i].TX[j].AntMapIndex)
                {
                case 0:
                    m_BandPathInfoEn[nBandIndex].TxChain.bChannelEn[RF_CHAINNAL0] = TRUE;
                    m_BandPathInfoEn[nBandIndex].TxChain.bChannelPriEn[RF_CHAINNAL0] = TRUE;
                    m_BandPathInfoEn[nBandIndex].TxChain.bChannelPriEnSave[RF_CHAINNAL0] = m_BandPathInfoEn[nBandIndex].TxChain.bChannelPriEnSave[RF_CHAINNAL0] || tx_cali_save_en;
                    m_BandPathInfoEn[nBandIndex].TxChain.ChannelPriPathNum[RF_CHAINNAL0] += 1;
                    temp.AntIndex = j;
                    temp.bSaveNvEn = tx_cali_save_en;
                    temp.rf_channel_id = m_BandPathInfo[nBandIndex].RFChannel.ch_4rx[i].rf_channel_id;
                    temp.b2rx = FALSE;
                    m_BandPathInfoEn[nBandIndex].TxChain.priPathinfo[RF_CHAINNAL0].push_back(temp);
                    m_BandPathInfoEn[nBandIndex].triggerPath.push_back(temp);
                    break;
                case 1:
                    m_BandPathInfoEn[nBandIndex].TxChain.bChannelEn[RF_CHAINNAL1] = TRUE;
                    m_BandPathInfoEn[nBandIndex].TxChain.bChannelPriEn[RF_CHAINNAL1] = TRUE;
                    m_BandPathInfoEn[nBandIndex].TxChain.bChannelPriEnSave[RF_CHAINNAL1] = m_BandPathInfoEn[nBandIndex].TxChain.bChannelPriEnSave[RF_CHAINNAL1] || tx_cali_save_en;
                    m_BandPathInfoEn[nBandIndex].TxChain.ChannelPriPathNum[RF_CHAINNAL1] += 1;
                    temp.AntIndex = j;
                    temp.bSaveNvEn = tx_cali_save_en;
                    temp.rf_channel_id = m_BandPathInfo[nBandIndex].RFChannel.ch_4rx[i].rf_channel_id;
                    temp.b2rx = FALSE;
                    m_BandPathInfoEn[nBandIndex].TxChain.priPathinfo[RF_CHAINNAL1].push_back(temp);
                    break;
                case 2:
                    m_BandPathInfoEn[nBandIndex].TxChain.bChannelEn[RF_CHAINNAL2] = TRUE;
                    m_BandPathInfoEn[nBandIndex].TxChain.bChannelPriEn[RF_CHAINNAL2] = TRUE;
                    m_BandPathInfoEn[nBandIndex].TxChain.bChannelPriEnSave[RF_CHAINNAL2] = m_BandPathInfoEn[nBandIndex].TxChain.bChannelPriEnSave[RF_CHAINNAL2] || tx_cali_save_en;
                    m_BandPathInfoEn[nBandIndex].TxChain.ChannelPriPathNum[RF_CHAINNAL2] += 1;
                    temp.AntIndex = j;
                    temp.bSaveNvEn = tx_cali_save_en;
                    temp.rf_channel_id = m_BandPathInfo[nBandIndex].RFChannel.ch_4rx[i].rf_channel_id;
                    temp.b2rx = FALSE;
                    m_BandPathInfoEn[nBandIndex].TxChain.priPathinfo[RF_CHAINNAL2].push_back(temp);
                    break;
                case 3:
                    m_BandPathInfoEn[nBandIndex].TxChain.bChannelEn[RF_CHAINNAL3] = TRUE;
                    m_BandPathInfoEn[nBandIndex].TxChain.bChannelPriEn[RF_CHAINNAL3] = TRUE;
                    m_BandPathInfoEn[nBandIndex].TxChain.bChannelPriEnSave[RF_CHAINNAL3] = m_BandPathInfoEn[nBandIndex].TxChain.bChannelPriEnSave[RF_CHAINNAL3] || tx_cali_save_en;
                    m_BandPathInfoEn[nBandIndex].TxChain.ChannelPriPathNum[RF_CHAINNAL3] += 1;
                    temp.AntIndex = j;
                    temp.bSaveNvEn = tx_cali_save_en;
                    temp.rf_channel_id = m_BandPathInfo[nBandIndex].RFChannel.ch_4rx[i].rf_channel_id;
                    temp.b2rx = FALSE;
                    m_BandPathInfoEn[nBandIndex].TxChain.priPathinfo[RF_CHAINNAL3].push_back(temp);
                    break;
                default:
                    LogFmtStrA(SPLOGLV_ERROR, "Unsupported Flag Type");
                }
                m_BandPathInfoEn[nBandIndex].TxAnt.txPathinfo[j].push_back(temp);
            }

        }
    }
    return SP_OK;
}
