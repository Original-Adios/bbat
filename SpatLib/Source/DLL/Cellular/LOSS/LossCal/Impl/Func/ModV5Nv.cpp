#include "StdAfx.h"
#include "ModV5Nv.h"
#include "LteUtility.h"

CModV5Nv::CModV5Nv( LPCWSTR lpName, CFuncCenter* pFuncCenter ) : CFuncBase(lpName, pFuncCenter)
{
	m_nMaxBandCount = MAX_NV_BAND_NUMBER_MV3;
	m_hDUT = NULL;
	m_stNvmParam.NVM_Uninitialize();

	ZeroMemory(m_nBand, sizeof(uint8)*MAX_NV_BAND_NUMBER_MV3);
	ZeroMemory(&m_BandSupportInfoV5, sizeof(NvData_LTE_BAND_SUPPORT_INFO_BAND_V5)*MAX_LTE_BAND);
	ZeroMemory(&m_BandListEnFlag, sizeof(NvData_LTE_BAND_LIST_FLAG));
    ZeroMemory(m_BandPathInfo, sizeof(NvData_LTE_BAND_RF_PATH) * MAX_NV_BAND_NUMBER_MV3);

	m_pFileConfig = NULL;
	m_nBandCnt = 0;
	m_nCaliParamVersion = 0;
	m_stNvRsp.NVM_Uninitialize();
	m_stProgramNvData.NVM_Uninitialize();

	m_arrGS_PortComp.clear();
	m_arrNvPos.clear();

	m_bBandAdaptation = FALSE;
    SP_LTE_Base_Init(m_pSpatBase->m_hDUT, MODEM_V5);

}

CModV5Nv::~CModV5Nv(void)
{
    SP_LTE_Base_Release(m_pSpatBase->m_hDUT);
}

SPRESULT CModV5Nv::PreInit()
{
	CHKRESULT(__super::PreInit());

	m_hDUT = m_pSpatBase->m_hDUT;
	m_pFileConfig = (CFileConfig*)m_pFuncCenter->GetFunc(FUNC_FILE_CONFIG);
	return SP_OK;
}

SPRESULT CModV5Nv::Load()
{
	LogFmtStrA(SPLOGLV_INFO, "%s: Load NV start", __FUNCTION__);

	CHKRESULT(LoadAndCheckFeatureSupport());
	CHKRESULT(InitBandNv());

	for ( uint32 i = 0; i < m_nBandCnt; i++ )
	{
		int nBnd = 0;

		for ( ; nBnd < m_nMaxBandCount; nBnd++ )
		{
			if ( CLteUtility::m_BandInfo[m_nBand[i]].BandIdent == m_BandSupportInfoV5[m_nBand[i]].band_num )
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

		CHKRESULT(ReadBandRfChainV5(i, CLteUtility::m_BandInfo[m_nBand[i]].Band));

		CHKRESULT(ConfigBandRfChainEnV4(i, nBnd))
	}

	CHKRESULT( GetPortComp() );

	LogFmtStrA(SPLOGLV_INFO, "%s: Load NV End", __FUNCTION__);
	return SP_OK;
}

SPRESULT CModV5Nv::LoadClcPorComp()
{
    LogFmtStrA(SPLOGLV_INFO, "%s: Load NV start", __FUNCTION__);

    CHKRESULT(ReadBandList());
    CHKRESULT(LoadAndCheckFeatureSupport());

    for (int i = 0; i < MAX_LTE_BAND; i++)
    {
        vector<int>::iterator result = find(m_nClcBand.begin(), m_nClcBand.end(), CLteUtility::m_BandInfo[i].Band); //²éÕÒ3
        if (result == m_nClcBand.end())
        {
            continue;
        }
        if (m_BandListEnFlag.BandFlagEn[i])
        {
            LTE_CALI_RF_PATH_GROUP_T_V5  m_BandPath;
            CHKRESULT(SP_ModemV5_LTE_Load_PathInfo(m_hDUT, (LTE_BAND_E)i, &m_BandPath));
            memset(&m_arrDownload_AntMap[i], -1, sizeof(READ_NV_PARAM_RLT_ANT_MAP));
            Band_RF_CHAIN_PATH_T temp;

            for (int pathIndex = 0; pathIndex < MAX_PATHGROUP_CNT; pathIndex++)
            {
                int groupId = m_BandPath.PathGroup[pathIndex].group_id;
                int rxPathCount = m_BandPath.PathGroup[pathIndex].rx_path_cnt;
                int txPathCount = m_BandPath.PathGroup[pathIndex].tx_path_cnt;
                //tx
                for (size_t txIndex = 0; txIndex < txPathCount; txIndex++)
                {
                    bool  tx_cali_en = (m_BandPath.PathGroup[pathIndex].tx_pathinfo[txIndex].Tx_Cali_En.ApcRefEn >> 7) & 0x1;
                    int ant_index = m_BandPath.PathGroup[pathIndex].tx_pathinfo[txIndex].antL;
                    uint32 pathId = m_BandPath.PathGroup[pathIndex].tx_pathinfo[txIndex].path_id;
                    uint8 phyPort = m_BandPath.PathGroup[pathIndex].tx_pathinfo[txIndex].antP - 1;

                    if (tx_cali_en)
                    {
                        int txAnt = 0;
                        if ((int8)m_arrDownload_AntMap[i].Tx[0] == -1)
                        {
                            m_arrDownload_AntMap[i].Tx[0] = phyPort;
                        }
                        else if ((int8)m_arrDownload_AntMap[i].Tx[0] != -1)
                        {
                            m_arrDownload_AntMap[i].Tx[1] = phyPort;
                            txAnt = 1;
                        }
                    }
                }

                //rx
                for (size_t rxIndex = 0; rxIndex < rxPathCount; rxIndex++)
                {
                    bool rx_cali_en = (m_BandPath.PathGroup[pathIndex].rx_pathinfo[rxIndex].Rx_Cali_En.AgcRefEn >> 7) & 0x1;
                    int ant_index = m_BandPath.PathGroup[pathIndex].rx_pathinfo[rxIndex].antL;
                    uint32 pathId = m_BandPath.PathGroup[pathIndex].rx_pathinfo[rxIndex].path_id;
                    uint8 phyPort = m_BandPath.PathGroup[pathIndex].rx_pathinfo[rxIndex].antP - 1;

                    if (rx_cali_en)
                    {
                        if ((int8)m_arrDownload_AntMap[i].Rx[ant_index] != -1)
                        {
                            continue;
                        }
                        m_arrDownload_AntMap[i].Rx[ant_index] = phyPort;
                    }
                }
            }
        }
    }


    CHKRESULT(GetPortComp());

    LogFmtStrA(SPLOGLV_INFO, "%s: Load NV End", __FUNCTION__);
    return SP_OK;
}

SPRESULT CModV5Nv::ReadNv(  PC_MODEM_RF_V3_DATA_PARAM_T *pNvRsp,
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

SPRESULT CModV5Nv::WriteNv( PC_MODEM_RF_V3_DATA_PARAM_T *pNvData, int eNvType, int BandId, int nDataSize, int nRfChain, int nAnt, int nDataOffset, int nBwId )
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

SPRESULT CModV5Nv::InsertBand( uint8 nBand )
{
	Band_Info* pBandInfo = &m_arrBandInfo[m_nBandCnt];

	pBandInfo->nBand = (LTE_BAND_E)nBand;
	pBandInfo->nNumeral = CLteUtility::m_BandInfo[nBand].BandIdent;
	pBandInfo->nIndicator = CLteUtility::m_BandInfo[nBand].nIndicator;

	for (int i = 0; i < MAX_LTE_BAND; i++)
	{
		if (m_BandSupportInfoV5[i].band_num == pBandInfo->nNumeral)
		{
			m_arrpDownload_BandInfo.at(m_nBandCnt) = m_BandSupportInfoV5[i];
            m_nBand[m_nBandCnt] = nBand;
            m_nBandCnt++;
            return SP_OK;
		}
	}

    UiSendMsg("Not Support Band", LEVEL_UI, 1, 0, 1, CLteUtility::m_BandInfo[m_nBand[nBand]].NameA, -1, NULL, NULL);
    return SP_E_LTE_CALI_NOT_SUPPORT_BAND;
}

SPRESULT CModV5Nv::InitBandNv()
{
	m_arrBandSelected.clear();
	m_nBandCnt = 0;
	m_arrpDownload_BandInfo.resize(m_nMaxBandCount);
	m_arrBandInfo.resize(m_nMaxBandCount);

	CHKRESULT(ReadBandList());

	for (int i = 0; i < MAX_LTE_BAND; i++)
	{
		if (m_pFileConfig->m_arrBandEnable[i])
		{
			BOOL bSupport = FALSE;


			if (m_BandListEnFlag.BandFlagEn[i])
			{
				bSupport= TRUE;
				m_arrBandSelected.push_back(i);
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

SPRESULT CModV5Nv::ReadBandList( void )
{
    ZeroMemory(&m_BandListEnFlag, sizeof(NvData_LTE_BAND_LIST_FLAG));
    CHKRESULT(SP_LTE_ModemV5_Load_BandList(m_hDUT, &m_BandListEnFlag, FALSE));

    for (int i = 0; i < MAX_LTE_BAND; i++)
    {
		if (m_BandListEnFlag.BandFlagEn[i])
		{
            CHKRESULT(SP_LTE_ModemV5_Load_BandInfo(m_hDUT, &m_BandSupportInfoV5[i], (LTE_BAND_E)i, FALSE));
            CHKRESULT(SP_LTE_ModemV5_Load_Download_CaliSetting(m_hDUT, &m_Download_CalSettingV5[i], (LTE_BAND_E)i, FALSE));

		}
    }
	return SP_OK;
}

SPRESULT CModV5Nv::GetPortComp( void )
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

SPRESULT CModV5Nv::LoadDownloadAntMap( int nBandIndex, int nBandPos )
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


SPRESULT CModV5Nv::LoadAndCheckFeatureSupport()
{
	/*LTE_VERSION_T NvExport_Version;
	CHKRESULT(SP_ModemV3_LTE_COMMOM_GetVersion(m_hDUT, &NvExport_Version));

	if(1 != NvExport_Version.CAL_LTE_ANT_MAP_REQ)
	{
		LogFmtStrA( SPLOGLV_ERROR, "%s: Not support atenna map feature !", __FUNCTION__ );
		return SP_E_LTE_CALI_INVALID_CALI_VERSION;
	}

	m_bBandAdaptation = (BOOL)NvExport_Version.CAL_LTE_Band_Adaptation;*/

    NvData_LTE_VERSION m_Download_Version;
    CHKRESULT(SP_LTE_ModemV4_Load_Version(m_hDUT, &m_Download_Version, FALSE));

    if (1 != m_Download_Version.CAL_LTE_ANT_MAP_REQ)
    {
        LogFmtStrA(SPLOGLV_ERROR, "%s: Not support atenna map feature !", __FUNCTION__);
        return SP_E_LTE_CALI_INVALID_CALI_VERSION;
    }

    m_bBandAdaptation = (BOOL)m_Download_Version.CAL_LTE_Band_Adaptation;

	return SP_OK;
}

SPRESULT CModV5Nv::ReadBandRfChainV5(int nIndex, LTE_BAND_E Band)
{
    memset(&m_BandPathInfo[nIndex].Groups, 0, sizeof(m_BandPathInfo[nIndex].Groups));
    CHKRESULT(SP_ModemV5_LTE_Load_PathInfo(m_hDUT, Band, &m_BandPathInfo[nIndex].Groups));
	return SP_OK;
}

SPRESULT CModV5Nv::ConfigBandRfChainEnV4(const unsigned int nBandIndex, int nNvPos)
{
    m_BandPathInfoEn[nBandIndex].NvPos = (uint8)nNvPos;
    uint8 cnt_2rx = 0;
    uint8 cnt_4rx = 0;

	memset(&m_arrDownload_AntMap[m_nBand[nBandIndex]], -1, sizeof(READ_NV_PARAM_RLT_ANT_MAP));
    Band_RF_CHAIN_PATH_T temp;

    for (int i = 0; i < MAX_PATHGROUP_CNT; i++)
    {
        int groupId = m_BandPathInfo[nBandIndex].Groups.PathGroup[i].group_id;
        int rxPathCount = m_BandPathInfo[nBandIndex].Groups.PathGroup[i].rx_path_cnt;
        int txPathCount = m_BandPathInfo[nBandIndex].Groups.PathGroup[i].tx_path_cnt;

        //tx
		for (size_t txIndex = 0; txIndex < txPathCount; txIndex++)
		{

			bool  tx_cali_en = (m_BandPathInfo[nBandIndex].Groups.PathGroup[i].tx_pathinfo[txIndex].Tx_Cali_En.ApcRefEn >> 7) & 0x1;
			bool tx_cali_save_en = tx_cali_en;
			uint8 tx_cali_save_index = m_BandPathInfo[nBandIndex].Groups.PathGroup[i].tx_pathinfo[txIndex].Tx_Cali_En.ApcRefEn & 0x7F;
			int ant_index = m_BandPathInfo[nBandIndex].Groups.PathGroup[i].tx_pathinfo[txIndex].antL;
            uint32 pathId = m_BandPathInfo[nBandIndex].Groups.PathGroup[i].tx_pathinfo[txIndex].path_id;
            uint8 phyPort = m_BandPathInfo[nBandIndex].Groups.PathGroup[i].tx_pathinfo[txIndex].antP-1;

			if (tx_cali_en)
			{
                int txAnt = 0;
                if ((int8)m_arrDownload_AntMap[m_nBand[nBandIndex]].Tx[0] == -1)
                {
                    m_arrDownload_AntMap[m_nBand[nBandIndex]].Tx[0] = phyPort;
                }
                else if ((int8)m_arrDownload_AntMap[m_nBand[nBandIndex]].Tx[0] != -1)
                {
                    m_arrDownload_AntMap[m_nBand[nBandIndex]].Tx[1] = phyPort;
					txAnt = 1;
                }

				m_BandPathInfoEn[nBandIndex].TxAnt.antTxPathNum += 1;
				m_BandPathInfoEn[nBandIndex].TxChain.bChannelEn[txAnt] = TRUE;
				m_BandPathInfoEn[nBandIndex].TxChain.bChannelPriEn[txAnt] = TRUE;
				m_BandPathInfoEn[nBandIndex].TxChain.bChannelPriEnSave[txAnt] = m_BandPathInfoEn[nBandIndex].TxChain.bChannelPriEnSave[RF_CHAINNAL0] || tx_cali_save_en;
				m_BandPathInfoEn[nBandIndex].TxChain.ChannelPriPathNum[txAnt] += 1;

				temp.AntIndex = ant_index;
				temp.bSaveNvEn = tx_cali_save_en;
				temp.rf_channel_id = pathId;
				temp.DlStream = m_BandPathInfo[nBandIndex].Groups.PathGroup[i].tx_pathinfo[txIndex].dl_stream;
				temp.b2rx = (2 == m_BandPathInfo[nBandIndex].Groups.PathGroup[i].tx_pathinfo[txIndex].ul_stream) ? TRUE:FALSE;
				temp.Group = groupId;
				temp.priordiv = 0;
				m_BandPathInfoEn[nBandIndex].TxChain.priPathinfo[txAnt].push_back(temp);
				m_BandPathInfoEn[nBandIndex].triggerPath.push_back(temp);
				m_BandPathInfoEn[nBandIndex].TxAnt.txPathinfo.push_back(temp);               
			}
		}

		//rx
        for (size_t rxIndex = 0; rxIndex < rxPathCount; rxIndex++)
        {
            bool rx_cali_en = (m_BandPathInfo[nBandIndex].Groups.PathGroup[i].rx_pathinfo[rxIndex].Rx_Cali_En.AgcRefEn >> 7) & 0x1;
            bool rx_cali_save_en = rx_cali_en;
            uint8 rx_cali_save_index = m_BandPathInfo[nBandIndex].Groups.PathGroup[i].rx_pathinfo[rxIndex].Rx_Cali_En.AgcRefEn & 0x7F;
            int ant_index = m_BandPathInfo[nBandIndex].Groups.PathGroup[i].rx_pathinfo[rxIndex].antL;
            uint32 pathId = m_BandPathInfo[nBandIndex].Groups.PathGroup[i].rx_pathinfo[rxIndex].path_id;
            uint8 phyPort = m_BandPathInfo[nBandIndex].Groups.PathGroup[i].rx_pathinfo[rxIndex].antP - 1;
   
            if (rx_cali_en)
            {
                if ((int8)m_arrDownload_AntMap[m_nBand[nBandIndex]].Rx[ant_index] != -1)
                {
                    continue;
                }

                m_BandPathInfoEn[nBandIndex].RxAnt.antRxPathNum[ant_index] += 1;
                m_BandPathInfoEn[nBandIndex].RxChain.bChannelEn[ant_index] = TRUE;
                m_BandPathInfoEn[nBandIndex].RxChain.bChannelDivEn[ant_index] = TRUE;
                m_BandPathInfoEn[nBandIndex].RxChain.bChannelDivEnSave[ant_index] = m_BandPathInfoEn[nBandIndex].RxChain.bChannelDivEnSave[ant_index] || rx_cali_save_en;
                m_BandPathInfoEn[nBandIndex].RxChain.ChannelDivPathNum[ant_index] += 1;
                temp.AntIndex = ant_index;
                temp.bSaveNvEn = rx_cali_save_en;
                temp.rf_channel_id = pathId;
				temp.DlStream = m_BandPathInfo[nBandIndex].Groups.PathGroup[i].rx_pathinfo[rxIndex].dl_stream;
                temp.b2rx = (2 == m_BandPathInfo[nBandIndex].Groups.PathGroup[i].rx_pathinfo[rxIndex].dl_stream) ? TRUE : FALSE;
                temp.Group = groupId;
                temp.priordiv = 1;
                m_BandPathInfoEn[nBandIndex].RxChain.divPathinfo[ant_index].push_back(temp);

                m_arrDownload_AntMap[m_nBand[nBandIndex]].Rx[ant_index] = phyPort;
            }
        }
    }

    return SP_OK;
}
