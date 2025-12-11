#include "StdAfx.h"
#include "AudioSaveRecorder.h"
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CAudioSaveRecorder)

///

const LPCWSTR g_strMicType[SP_AUDIO_MIC_MAX_TYPE] = {L"", L"Primary", L"AUX", L"Dual", L"Earphone"};
CAudioSaveRecorder::CAudioSaveRecorder(void)
    : m_bSNSuffix(TRUE)
    , m_bTimeSuffix(TRUE)
    , m_bRecordInfoSuffix(FALSE)
{

}

CAudioSaveRecorder::~CAudioSaveRecorder(void)
{

}

BOOL CAudioSaveRecorder::LoadXMLConfig(void)
{
	std::wstring strVal = GetConfigValue(L"Option:FilePath:Local", L"");
	trimW(strVal);
	m_strFilePath = GetAbsoluteFilePathW(strVal.c_str());
	if (0 == m_strFilePath.length())
	{
		return FALSE;
	}

	m_bSNSuffix = GetConfigValue(L"Option:FilePath:Suffix:SN", TRUE);
	m_bTimeSuffix = GetConfigValue(L"Option:FilePath:Suffix:Time", TRUE);
	m_bRecordInfoSuffix = GetConfigValue(L"Option:FilePath:Suffix:RecordSet", FALSE);

	strVal = GetConfigValue(L"Option:RecordSet:MicType", L"Primary");
	for (int i=0; i<SP_AUDIO_MIC_MAX_TYPE; i++)
	{
		if (0 == strVal.compare(g_strMicType[i]))
		{
			m_RecordSet.eMicType = (SP_AUDIO_MIC_TYPE)i;
			break;
		}
	}
	
    m_RecordSet.u16SampleRate = (uint16)GetConfigValue(L"Option:RecordSet:SampleRate", 44100);
	m_RecordSet.u32Time = (uint16)GetConfigValue(L"Option:RecordSet:Time", 10000);
    if (m_RecordSet.u32Time < 40 || m_RecordSet.u32Time > 60000)
    {
        // According to Audio SW guys(Min.Wei)
        SendCommonCallback(L"RecordSet:Time is out of range: [40 - 60000]");
        return FALSE;
    }
	m_RecordSet.u8Bit = (uint8)GetConfigValue(L"Option:RecordSet:Bit", 16);
	m_RecordSet.u8TrackCount = (uint8)GetConfigValue(L"Option:RecordSet:TrackCount", 1);

    return TRUE;
}

SPRESULT CAudioSaveRecorder::__PollAction(void)
{
    // TODO: Get directory, name and extension from path
    std::wstring strPath = L"";
    std::wstring strName = L"";
    std::wstring strExt  = L"";
    if (PathIsDirectoryW(m_strFilePath.c_str()))
    {
        // Path is directory
        strPath = m_strFilePath;
        strName = L"record";
        strExt  = L".wav";
    }
    else
    {
        // Get directory
        strPath = m_strFilePath;
        std::wstring::size_type nPos = strPath.rfind(L'\\');
        if (nPos != std::wstring::npos)
        {
            strPath = strPath.substr(0, nPos + 1);
        }
        
        // Get file name
        strName = PathFindFileNameW(m_strFilePath.c_str());
        WCHAR szName[_MAX_PATH] = {0};
        wcsncpy_s(szName, strName.c_str(), strName.length());
        PathRemoveExtensionW(szName);
        strName = szName;

        // Get file extension
        strExt  = PathFindExtensionW(m_strFilePath.c_str());
    }

    if (!CreateMultiDirectoryW(strPath.c_str(), NULL))
    {
        LogFmtStrW(SPLOGLV_ERROR, L"Create Directory %s failed", strPath.c_str());
        return SP_E_FAIL;
    }

    if (m_bSNSuffix)
    {
        CHAR szSN[64] = {0};
        SPRESULT res = GetShareMemory(ShareMemory_SN1, (void* )szSN, sizeof(szSN));
        if (SP_OK != res)
        {
            CHKRESULT_WITH_NOTIFY_FUNNAME(SP_LoadSN(m_hDUT, SN1, szSN, sizeof(szSN)));
        }

        if (strlen(szSN) > 0)
        {
            strName += L"_";
            strName += _A2CW(szSN);
        }
    }

    if (m_bTimeSuffix)
    {
        WCHAR szTmp[128] = {0};
        SYSTEMTIME    t;
        GetLocalTime(&t);
        swprintf_s(szTmp, L"_%02d_%02d_%02d_%02d_%02d_%02d", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
        strName += szTmp;
    }

	if (m_bRecordInfoSuffix)
	{
        WCHAR szTmp[128] = {0};
		swprintf_s(szTmp, L"%s_%dHz_track%d_%dbits_%dms", g_strMicType[m_RecordSet.eMicType], 
			m_RecordSet.u16SampleRate, m_RecordSet.u8TrackCount, m_RecordSet.u8Bit, m_RecordSet.u32Time);
		strName += szTmp;
	}

    strPath = strPath + strName + strExt;
	CHKRESULT_WITH_NOTIFY_FUNNAME(SP_audioStartRecord(m_hDUT, &m_RecordSet));

    DWORD dwTimeOut = m_RecordSet.u32Time + TIMEOUT_10S;
	CHKRESULT_WITH_NOTIFY_FUNNAME(SP_audioSaveRecordFile(m_hDUT, strPath.c_str(), dwTimeOut));

	return SP_OK;
}

