#include "StdAfx.h"
#include "Nv_UIS8910.h"
#include "LteUtility.h"

PC_LTE_NV_UIS8910_DATA_T g_nvTransBuf;

CNv_UIS8910::CNv_UIS8910( LPCWSTR lpName, CFuncCenter* pFuncCenter ) : CFuncBase(lpName, pFuncCenter)
, m_uVersion(0)
{
	m_bSameDownloadChecksum = FALSE;
	m_nBandCount = 0;
	m_pCalibration_Version = NULL;
	m_hDUT = NULL;
	ZeroMemory((void *)(&m_LteNv_Download_Checksum), sizeof(PC_LTE_NV_DATA_T)); 
	ZeroMemory((void *)(&m_LteNv_Cali_Version), sizeof(PC_LTE_NV_DATA_T)); 
	m_pDownload_Checksum = NULL;
	m_pFileConfig = NULL;
    ZeroMemory((void*)(&m_arrUeband_info[0]), UIS8910_BAND_NUM*sizeof(T_UIS8910_BAND_INFO));
	m_nMaxBandCount = 0;
}

CNv_UIS8910::~CNv_UIS8910(void)
{

}

SPRESULT CNv_UIS8910::PreInit()
{
    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

    m_pFileConfig = (CFileConfig_UIS8910*)m_pFuncCenter->GetFunc(FUNC_FILE_CONFIG);

    m_hDUT = m_pSpatBase->m_hDUT;

    m_pDownload_Checksum   = (NvData_DownloadChecksum*)  m_LteNv_Download_Checksum .nData;
    m_pCalibration_Version = (NvData_CalibrationVersion*)m_LteNv_Cali_Version      .nData;

    m_LteNv_Cali_Version.eNvType            = LTE_NV_TYPE_CALI_PARAM_VERSION;

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}


SPRESULT CNv_UIS8910:: GetUeRfbandInfo()
{
    ZeroMemory(&g_nvTransBuf, sizeof(PC_LTE_NV_UIS8910_DATA_T));

    if (0x8850 == m_pFileConfig->m_Uetype)
    {
        g_nvTransBuf.header.eNvType = LTE_NV_UIS8910_TYPE_GET_RFBAND_INFO;
        g_nvTransBuf.header.Position = 0;
        g_nvTransBuf.header.DataSize = sizeof(T_UIS8850_BAND_NV);

        CHKRESULT(SP_lteLoadNV_UIS8910(m_hDUT, &g_nvTransBuf));

        memcpy(&m_arrUeband_info[0], &g_nvTransBuf.nData[0], UIS8910_BAND_NUM*sizeof(T_UIS8910_BAND_INFO));
    }
    else
    {
        g_nvTransBuf.header.eNvType = LTE_NV_UIS8910_TYPE_GET_RFBAND_INFO;
        g_nvTransBuf.header.Position = 0;
        g_nvTransBuf.header.DataSize = sizeof(T_UIS8910_BAND_NV);

        CHKRESULT(SP_lteLoadNV_UIS8910(m_hDUT, &g_nvTransBuf));

        memcpy(&m_arrUeband_info[0], &g_nvTransBuf.nData[0], UIS8910_BAND_NUM * sizeof(T_UIS8910_BAND_INFO));
    }

	for(int i=0; i<UIS8910_BAND_NUM; i++)
	{
	    if(m_arrUeband_info[i].nActiveFlag == 1)
		{
		    m_nMaxBandCount++;
		}
	}

	return SP_OK;
}

SPRESULT CNv_UIS8910::Load()
{
    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

	m_nMaxBandCount = 0;

	CHKRESULT(GetUeRfbandInfo());

	m_arrpDownload_BandInfo_Complete.resize(m_nMaxBandCount);
	m_arrDownload_BandInfo.resize(m_nMaxBandCount);
	m_arrpDownload_BandInfo.resize(m_nMaxBandCount);
    m_arrBandInfo.resize(m_nMaxBandCount);
    //m_arrDownload_BandInfo_Complete_V2.resize(m_nMaxBandCount);
    //m_arrDownload_BandInfo_Complete_V3.resize(m_nMaxBandCount);
    //m_arrpDownload_BandInfo_Complete.resize(m_nMaxBandCount);

	CHKRESULT(InitBandNv());
	if (m_pFileConfig->m_bCompatibility)
	{
		PrintCompatibleBand();
	}

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}

void CNv_UIS8910::ApplyChecksum()
{
    for (int i = 0; i < 4; i++)
    {
        m_Last_Download_Checksum.Checksum[i] = m_pDownload_Checksum->Checksum[i];
    }

    m_uVersion = m_pCalibration_Version->Version;
}


SPRESULT CNv_UIS8910::InsertBand(LTE_BAND_E Band)
{
    IsValidIndex(Band);
    LogFmtStrA(SPLOGLV_INFO, "%s Band = %s", __FUNCTION__, CLteUtility::m_BandInfo[Band].NameA);

    Band_Info* pBandInfo = &m_arrBandInfo[m_nBandCount];

    pBandInfo->nBand = Band;
    pBandInfo->nNumeral = CLteUtility::m_BandInfo[Band].nNumeral;
    pBandInfo->nIndicator = CLteUtility::m_BandInfo[Band].nIndicator;

    //Band info
    if (m_pFileConfig->m_bCompatibility)
    {
        for (int i = 0; i < m_nMaxBandCount; i++)
        {
            if (m_arrpDownload_BandInfo_Complete[i]->Band == pBandInfo->nNumeral
                && m_arrpDownload_BandInfo_Complete[i]->Indicator == pBandInfo->nIndicator)
            {
                m_arrpDownload_BandInfo[m_nBandCount] = m_arrpDownload_BandInfo_Complete[i];
                break;
            }
        }
    }
    else
    {
        memset(&m_arrDownload_BandInfo_Complete_V2[m_nBandCount], 0, sizeof(PC_LTE_NV_V2_DATA_T));
        m_arrDownload_BandInfo_Complete_V2[m_nBandCount].eNvType = LTE_NV_V3_TYPE_DOWNLOAD_BAND_INFO;
        m_arrDownload_BandInfo_Complete_V2[m_nBandCount].nBandNum = 1;
        m_arrDownload_BandInfo_Complete_V2[m_nBandCount].headinfo[0].band = (uint16)CLteUtility::m_BandInfo[Band].nNumeral;
        m_arrDownload_BandInfo_Complete_V2[m_nBandCount].headinfo[0].reserve[0] = (uint16)CLteUtility::m_BandInfo[Band].nIndicator;
        m_arrDownload_BandInfo_Complete_V2[m_nBandCount].headinfo[0].isPartion = 0;

        CHKRESULT_WITH_FMT_STR_W(SP_lteLoadNV_V2(m_hDUT, &m_arrDownload_BandInfo_Complete_V2[m_nBandCount]), L"Read NV LTE_NV_V3_TYPE_DOWNLOAD_BAND_INFO fail!");
        
        m_arrpDownload_BandInfo[m_nBandCount] = (NvData_BandInfo*)&m_arrDownload_BandInfo_Complete_V2[m_nBandCount].nData;
    }

    m_nBandCount++;

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}

SPRESULT CNv_UIS8910::InitBandNv()
{
    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    m_nBandCount = 0;

    if (m_pFileConfig->m_bCompatibility)
    {
		CHKRESULT(InitBandNv8910());
        uint32 iArrBandNvSize = m_arrBandNv.size();
        for (uint32 i = 0; i < iArrBandNvSize; i++)
        {
            CHKRESULT(InsertBand((LTE_BAND_E)m_arrBandNv[i]));
        }
    }
    else
    {
        for (uint32 i = 0; i < MAX_LTE_BAND; i++)
        {
            if (m_pFileConfig->m_arrBandEnable[i])
            {
                CHKRESULT(InsertBand((LTE_BAND_E)i));
            }
        }  
    }

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}
#if 0
SPRESULT CNv_UIS8910::InitBandNv()
{
    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    m_nBandCount = 0;

    if (m_pFileConfig->m_bCompatibility)
    {
        switch(m_pCalibration_Version->Version)
        {
        case LTE_NV_VERSION_V5:
            CHKRESULT(InitBandNvV5());
            break;
        default:
            CHKRESULT(InitBandNvV6());
            break;
        }

        for (uint32 i = 0; i < m_arrBandNv.size(); i++)
        {
            CHKRESULT(InsertBand((LTE_BAND_E)m_arrBandNv[i]));
        }
    }
    else
    {
        for (uint32 i = 0; i < MAX_LTE_BAND; i++)
        {
            if (m_pFileConfig->m_arrBandEnable[i])
            {
                CHKRESULT(InsertBand((LTE_BAND_E)i));
            }
        }  
    }

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}
#endif

void CNv_UIS8910::PrintCompatibleBand()
{
    uint32 iArrCompatibleBandNvSize = m_arrCompatibleBandNv.size();
    for (uint32 i = 0; i < iArrCompatibleBandNvSize; i++)
    {
        UiSendMsg("Skip Compatible Band", LEVEL_UI, 1, 1, 1, CLteUtility::m_BandInfo[m_arrCompatibleBandNv[i]].NameA, -1, NULL, NULL);
    }
}


SPRESULT CNv_UIS8910::InitBandNv8910()
{
    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

    m_arrBandNv.clear();
    m_arrCompatibleBandNv.clear();

	//CHKRESULT_WITH_FMT_STR_W(GetUeRfbandInfo(), L"Read UE BAND INFO FAIL!");
	for(int i=0,j=0; i<UIS8910_BAND_NUM; i++)
	{
	    if(m_arrUeband_info[i].nActiveFlag == 1)
		{
		    m_arrDownload_BandInfo[j].Band = ((uint16)(m_arrUeband_info[i].nNumeral&0xffff));
			m_arrDownload_BandInfo[j].Indicator = ((uint16)(m_arrUeband_info[i].nIndicator&0x0fff));
			m_arrDownload_BandInfo[j].Compatible = 0;
			m_arrDownload_BandInfo[j].TxDcEnable = 0;
			m_arrDownload_BandInfo[j].DivDisable = 1;
			m_arrDownload_BandInfo[j].RxCa = 0;
			m_arrDownload_BandInfo[j].TxCa = 0;
			m_arrpDownload_BandInfo_Complete[j] = &m_arrDownload_BandInfo[j];
			j++;
		}
	}

	if(m_pFileConfig->m_bAutoBand == TRUE)
	{
		for (int j = 0; j < m_nMaxBandCount; j++)
		{
			BOOL bSupport = FALSE;

			for (int i = 0; i < MAX_LTE_BAND; i++)
			{			
				if (CLteUtility::m_BandInfo[i].nNumeral == m_arrpDownload_BandInfo_Complete[j]->Band
					&& CLteUtility::m_BandInfo[i].nIndicator == m_arrpDownload_BandInfo_Complete[j]->Indicator)
				{
					bSupport= TRUE;
					m_arrBandNv.push_back(i);
					break;
				}
#if 0
				else if (CLteUtility::m_BandInfo[i].nNumeral == m_arrpDownload_BandInfo_Complete[j]->Compatible)
				{
					bSupport= TRUE;
					m_arrCompatibleBandNv.push_back(i);
					break;
				} 
#endif
			}

			char tempStr[40];
			sprintf_s(tempStr, "Band%d", m_arrpDownload_BandInfo_Complete[j]->Band);

			if (!bSupport)
			{
				//UiSendMsg("Not Support Band", LEVEL_UI, 1, 0, 1, CLteUtility::m_BandInfo[i].NameA, -1, NULL, NULL);
				UiSendMsg("Not Support Band", LEVEL_UI, 1, 0, 1,tempStr, -1, NULL, NULL);
				return SP_E_LTE_CALI_NOT_SUPPORT_BAND; 
			}

		}
	}
	else
	{
        for (int i = 0; i < MAX_LTE_BAND; i++)
		{
			if (m_pFileConfig->m_arrBandEnable[i] == 0)
			{
                continue;
			}
			
			BOOL bSupport = FALSE;

			for (int j = 0; j < m_nMaxBandCount; j++)
			{			
				if (CLteUtility::m_BandInfo[i].nNumeral == m_arrpDownload_BandInfo_Complete[j]->Band
					&& CLteUtility::m_BandInfo[i].nIndicator == m_arrpDownload_BandInfo_Complete[j]->Indicator)
				{
					bSupport= TRUE;
					m_arrBandNv.push_back(i);
					break;
				}
#if 0
				else if (CLteUtility::m_BandInfo[i].nNumeral == m_arrpDownload_BandInfo_Complete[j]->Compatible)
				{
					bSupport= TRUE;
					m_arrCompatibleBandNv.push_back(i);
					break;
				} 
#endif
			}

			if (!bSupport)
			{
				UiSendMsg("Not Support Band", LEVEL_UI, 1, 0, 1, CLteUtility::m_BandInfo[i].NameA, -1, NULL, NULL);
				return SP_E_LTE_CALI_NOT_SUPPORT_BAND; 
			}
		}
	}

    if (m_arrBandNv.size() == 0)
    {
        UiSendMsg("No Band Selected", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, NULL);
        return SP_E_LTE_CALI_NO_BAND_SELECTED;
    }

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}


SPRESULT CNv_UIS8910::InitBandNvV5()
{
    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

    m_arrBandNv.clear();
    m_arrCompatibleBandNv.clear();

    memset(&m_arrDownload_BandInfo_Complete_V2[0], 0, sizeof(PC_LTE_NV_V2_DATA_T));
    m_arrDownload_BandInfo_Complete_V2[0].eNvType = LTE_NV_V3_TYPE_DOWNLOAD_BAND_INFO;
    m_arrDownload_BandInfo_Complete_V2[0].nBandNum = 20;

    CHKRESULT_WITH_FMT_STR_W(SP_lteLoadNV_V2(m_hDUT, &m_arrDownload_BandInfo_Complete_V2[0])
        , L"Read NV LTE_NV_V3_TYPE_DOWNLOAD_BAND_INFO fail!");

    int nIndex = 0;
    for (int i = 0; i < 20; i++)
    {
        m_arrpDownload_BandInfo_Complete[i] = (NvData_BandInfo*)&m_arrDownload_BandInfo_Complete_V2[0].nData[nIndex];
        nIndex += m_arrDownload_BandInfo_Complete_V2[0].headinfo[i].BandDataCount;
    }

    for (int i = 0; i < MAX_LTE_BAND; i++)
    {
        if (m_pFileConfig->m_arrBandEnable[i])
        {
            BOOL bSupport = FALSE;
            for (int j = 0; j < 20; j++)
            {
                if (CLteUtility::m_BandInfo[i].nNumeral == m_arrpDownload_BandInfo_Complete[j]->Band
                    && CLteUtility::m_BandInfo[i].nIndicator == m_arrpDownload_BandInfo_Complete[j]->Indicator)
                {
                    bSupport= TRUE;
                    m_arrBandNv.push_back(i);
                    break;
                }
                else if (CLteUtility::m_BandInfo[i].nNumeral == m_arrpDownload_BandInfo_Complete[j]->Compatible)
                {
                    bSupport= TRUE;
                    m_arrCompatibleBandNv.push_back(i);
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

    if (m_arrBandNv.size() == 0)
    {
        UiSendMsg("No Band Selected", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, NULL);
        return SP_E_LTE_CALI_NO_BAND_SELECTED;
    }

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}

SPRESULT CNv_UIS8910::InitBandNvV6()
{
    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

    m_arrBandNv.clear();
    m_arrCompatibleBandNv.clear();

    for (int i = 0; i < m_nMaxBandCount; i++)
    {
        m_arrDownload_BandInfo_Complete_V3[i].eNvType = LTE_NV_V6_TYPE_DOWNLOAD_BAND_INFO;
        m_arrDownload_BandInfo_Complete_V3[i].Band = 0;
        m_arrDownload_BandInfo_Complete_V3[i].Indicator = 0;
        m_arrDownload_BandInfo_Complete_V3[i].Position = (uint16)(i);
        m_arrpDownload_BandInfo_Complete[i] = (NvData_BandInfo*)&m_arrDownload_BandInfo_Complete_V3[i].nData;

        CHKRESULT_WITH_FMT_STR_W(SP_lteLoadNV_V3(m_hDUT, &m_arrDownload_BandInfo_Complete_V3[i])
            , L"Read NV LTE_NV_V6_TYPE_DOWNLOAD_BAND_INFO Position = %d fail!"
            , m_arrDownload_BandInfo_Complete_V3[i].Position);
    }

    for (int i = 0; i < MAX_LTE_BAND; i++)
    {
        if (m_pFileConfig->m_arrBandEnable[i])
        {
            BOOL bSupport = FALSE;
            for (int j = 0; j < m_nMaxBandCount; j++)
            {
                if (CLteUtility::m_BandInfo[i].nNumeral == m_arrpDownload_BandInfo_Complete[j]->Band
                    && CLteUtility::m_BandInfo[i].nIndicator == m_arrpDownload_BandInfo_Complete[j]->Indicator)
                {
                    bSupport= TRUE;
                    m_arrBandNv.push_back(i);
                    break;
                }
                else if (CLteUtility::m_BandInfo[i].nNumeral == m_arrpDownload_BandInfo_Complete[j]->Compatible)
                {
                    bSupport= TRUE;
                    m_arrCompatibleBandNv.push_back(i);
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

    if (m_arrBandNv.size() == 0)
    {
        UiSendMsg("No Band Selected", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, NULL);
        return SP_E_LTE_CALI_NO_BAND_SELECTED;
    }

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}
