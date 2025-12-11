#include "StdAfx.h"
#include "Nv.h"
#include "LteUtility.h"

CNv::CNv( LPCWSTR lpName, CFuncCenter* pFuncCenter ) : CFuncBase(lpName, pFuncCenter)
, m_uVersion(0)
{
	m_bSameDownloadChecksum = FALSE;
	m_nBandCount = 20;
	m_pCalibration_Version = NULL;
	m_hDUT = NULL;

	ZeroMemory(&m_LteNv_Download_Checksum, sizeof(m_LteNv_Download_Checksum));
	ZeroMemory(&m_LteNv_Cali_Version, sizeof(m_LteNv_Cali_Version));

	m_pDownload_Checksum = NULL;
	m_pFileConfig = NULL;
	m_nMaxBandCount = 23;

}

CNv::~CNv(void)
{

}

SPRESULT CNv::PreInit()
{
    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

    m_pFileConfig = (CFileConfig*)m_pFuncCenter->GetFunc(FUNC_FILE_CONFIG);

    m_hDUT = m_pSpatBase->m_hDUT;

    m_pDownload_Checksum   = (NvData_DownloadChecksum*)  m_LteNv_Download_Checksum .nData;
    m_pCalibration_Version = (NvData_CalibrationVersion*)m_LteNv_Cali_Version      .nData;

    m_LteNv_Cali_Version.eNvType            = LTE_NV_TYPE_CALI_PARAM_VERSION;

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}

SPRESULT CNv::Load()
{
    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

    CHKRESULT_WITH_RAW_STR_W(SP_lteLoadNV(m_hDUT, &m_LteNv_Cali_Version), L"Read NV LTE_NV_TYPE_CALI_PARAM_VERSION fail!");

    //checksum
    if (m_pCalibration_Version->Version == LTE_NV_VERSION_V5)
    {
        m_LteNv_Download_Checksum.eNvType = LTE_NV_V3_TYPE_DOWNLOAD_CHECKSUM;
        m_nMaxBandCount = 20;
    }
    else if (m_pCalibration_Version->Version == LTE_NV_VERSION_V6)
    {
        m_LteNv_Download_Checksum.eNvType = LTE_NV_V6_TYPE_DOWNLOAD_CHECKSUM;
        m_nMaxBandCount = 20;
    }
    else if (m_pCalibration_Version->Version == LTE_NV_VERSION_V7 ||m_pCalibration_Version->Version == LTE_NV_VERSION_V8)
    {
        m_LteNv_Download_Checksum.eNvType = LTE_NV_V6_TYPE_DOWNLOAD_CHECKSUM;
        m_nMaxBandCount = 23;
    }
    else
    {
        return SP_E_LTE_CALI_INVALID_CALI_VERSION;
    }

    m_arrpDownload_BandInfo.resize(m_nMaxBandCount);
    m_arrBandInfo.resize(m_nMaxBandCount);
    m_arrDownload_BandInfo_Complete_V2.resize(m_nMaxBandCount);
    m_arrDownload_BandInfo_Complete_V3.resize(m_nMaxBandCount);
    m_arrpDownload_BandInfo_Complete.resize(m_nMaxBandCount);
    
    CHKRESULT_WITH_RAW_STR_W(SP_lteLoadNV(m_hDUT, &m_LteNv_Download_Checksum), L"Read NV LTE_NV_V3_TYPE_DOWNLOAD_CHECKSUM fail!");

    if (   m_pDownload_Checksum->Checksum[0] == 0
        && m_pDownload_Checksum->Checksum[1] == 0
        && m_pDownload_Checksum->Checksum[2] == 0
        && m_pDownload_Checksum->Checksum[3] == 0)
    {
        return SP_E_LTE_CALI_INVALID_CHECKSUM;
    }

    m_bSameDownloadChecksum = TRUE;

    for (int i = 0; i < 4; i++)
    {
        if (m_pDownload_Checksum->Checksum[i] != m_Last_Download_Checksum.Checksum[i])
        {
            m_bSameDownloadChecksum = FALSE;
            break;
        }
    }

    if (m_pCalibration_Version->Version != m_uVersion)
    {
        m_bSameDownloadChecksum = FALSE;
    }

    if (m_bSameDownloadChecksum )
    {
        if (m_pFileConfig->m_bCompatibility)
        {
            PrintCompatibleBand();
        }
    }
    else
    {
        CHKRESULT(InitBandNv());
        if (m_pFileConfig->m_bCompatibility)
        {
            PrintCompatibleBand();
        }
    }

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}

void CNv::ApplyChecksum()
{
    for (int i = 0; i < 4; i++)
    {
        m_Last_Download_Checksum.Checksum[i] = m_pDownload_Checksum->Checksum[i];
    }

    m_uVersion = m_pCalibration_Version->Version;
}


SPRESULT CNv::InsertBand(LTE_BAND_E Band)
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

SPRESULT CNv::InitBandNv()
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

void CNv::PrintCompatibleBand()
{
    for (uint32 i = 0; i < m_arrCompatibleBandNv.size(); i++)
    {
        UiSendMsg("Skip Compatible Band", LEVEL_UI, 1, 1, 1, CLteUtility::m_BandInfo[m_arrCompatibleBandNv[i]].NameA, -1, NULL, NULL);
    }
}

SPRESULT CNv::InitBandNvV5()
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

SPRESULT CNv::InitBandNvV6()
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
