#include "stdafx.h"
#include "CheckCommonCamera.h"

IMPLEMENT_RUNTIME_CLASS(CCheckCommonCamera)

//////////////////////////////////////////////////////////////////////////
CCheckCommonCamera::CCheckCommonCamera()
{
	m_nCameraCount = 0;
	m_vecCameraInfo.clear();
}

CCheckCommonCamera::~CCheckCommonCamera()
{
}

BOOL CCheckCommonCamera::LoadXMLConfig(void)
{
    m_nCameraCount    = (uint8)GetConfigValue(L"Param:Camera:GroupCount", 0);

	for (uint8 i = 0; i < m_nCameraCount; i++)
	{
		CAMERA_INFOR CameraInfo;
		WCHAR szCameraTypePath[MAX_PATH] = {0};
		swprintf_s(szCameraTypePath, L"Param:Camera:Group%d:CameraType", i + 1);
		std::string strType = _W2CA(GetConfigValue(szCameraTypePath, L""));
		if (strType.length() > MAX_CAMERA_INFOR_LENGTH || 0 == strType.length())
		{
			return FALSE;
		}
		memcpy_s(CameraInfo.szCameraType, MAX_CAMERA_INFOR_LENGTH, strType.c_str(), strType.length());

		WCHAR szCameraSizePath[MAX_PATH] = {0};
		swprintf_s(szCameraSizePath, L"Param:Camera:Group%d:CameraSize", i + 1);
		std::string strSize = _W2CA(GetConfigValue(szCameraSizePath, L""));
		if (strSize.length() > MAX_CAMERA_INFOR_LENGTH || 0 == strSize.length())
		{
			return FALSE;
		}
		memcpy_s(CameraInfo.szCameraSize, MAX_CAMERA_INFOR_LENGTH, strSize.c_str(), strSize.length());
		m_vecCameraInfo.push_back(CameraInfo);
	}

    
    return TRUE;
}
/*DEMO
> AT+GETCAMINFO?
<slot:4>,ov32a1q_v1,32M,s5ks3p92_v1,16M,ov16885_normal,16M,ov8856_v1,8M
OK
*/
SPRESULT CCheckCommonCamera::__PollAction(void)
{
	const char ITEM_NAME[] = "CheckCommonCamera";
	LPCSTR CAMEINFO_CMD = "AT+GETCAMINFO?";
	SPRESULT res = SendATCommand(CAMEINFO_CMD);
	if (SP_OK != res)
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, CAMEINFO_CMD);
		return SP_E_PHONE_AT_EXECUTE_FAIL;
	}
	else
	{
		LPCSTR szSepCount = "slot:";
		std::string::size_type nPos = m_strRevBuf.find(szSepCount);
		if (std::string::npos == nPos)
		{
			NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "%s find fail", szSepCount);
			return SP_E_NO_USAGE_COUNT;
		}
		m_strRevBuf.erase(0, nPos + strlen(szSepCount));
		uint8 nCamCount = (uint8)atoi(m_strRevBuf.c_str());
		if (nCamCount != m_nCameraCount)
		{
			NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "camera count(%d) fail;setting(%d)", nCamCount, m_nCameraCount);
			return SP_E_NO_USAGE_COUNT;
		}
		
		LPCSTR szSep = ",";
		nPos = m_strRevBuf.find(szSep);
		if (std::string::npos == nPos)
		{
			NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, CAMEINFO_CMD);
			return SP_E_NO_USAGE_COUNT;
		}
		m_strRevBuf.erase(0, nPos + 1);

		m_strRevBuf += ",";//方便数据分析
		int nSepCount = 0;
		while(std::string::npos != (nPos = m_strRevBuf.find(szSep)))
		{
			std::string strType = m_strRevBuf.substr(0, nPos);
			if (0 != strType.compare(m_vecCameraInfo[nSepCount].szCameraType))
			{
				NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Camera%d type fail", nSepCount + 1);
				return 0 == nSepCount ? SP_E_CHECK_MAINCAMERA_TYPE_ERROR:SP_E_CHECK_PRECAMERA_TYPE_ERROR;
			}
			m_strRevBuf.erase(0, nPos + 1);

			nPos = m_strRevBuf.find(szSep);
			std::string strSize = m_strRevBuf.substr(0, nPos);
			if (0 != strSize.compare(m_vecCameraInfo[nSepCount].szCameraSize))
			{
				NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Camera%d size fail", nSepCount + 1);
				return 0 == nSepCount ? SP_E_CHECK_MAINCAMERA_SIZE_ERROR:SP_E_CHECK_PRECAMERA_SIZE_ERROR;
			}
			m_strRevBuf.erase(0, nPos + 1);

			nSepCount ++;
		}

		if (nCamCount != nSepCount)
		{
			NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "camera count(%d) fail;sep count(%d)", nCamCount, nSepCount);
			return SP_E_NO_USAGE_COUNT;
		}

		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "camera count(%d)", nCamCount);
		return SP_OK;
	}
}