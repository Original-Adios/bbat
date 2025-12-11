#include "stdafx.h"
#include "CheckCamera.h"

IMPLEMENT_RUNTIME_CLASS(CCheckCamera)

//////////////////////////////////////////////////////////////////////////
CCheckCamera::CCheckCamera()
{
    for (INT i=0; i<MAX_CAMERA_COUNT; i++)
    {
        m_bChkCamera[i] = FALSE;
    }
}

CCheckCamera::~CCheckCamera()
{

}

BOOL CCheckCamera::LoadXMLConfig(void)
{
    m_bChkCamera[0]    = (BOOL)GetConfigValue(L"Option:MainCamera:Check", 0);
    m_strCameraType[0] = _W2CA(GetConfigValue(L"Option:MainCamera:CameraType", L""));
    m_strCameraSize[0] = _W2CA(GetConfigValue(L"Option:MainCamera:CameraSize", L""));

    m_bChkCamera[1]    = (BOOL)GetConfigValue(L"Option:PreCamera:Check", 0);
    m_strCameraType[1] = _W2CA(GetConfigValue(L"Option:PreCamera:CameraType", L""));
    m_strCameraSize[1] = _W2CA(GetConfigValue(L"Option:PreCamera:CameraSize", L""));
    
    return TRUE;
}

SPRESULT CCheckCamera::__PollAction(void)
{
    for (INT i=0; i<MAX_CAMERA_COUNT; i++)
    {
        CHKRESULT(CheckCamera(i));
    }

    return SP_OK;
}

SPRESULT CCheckCamera::CheckCamera(INT nCamNo)
{
    if (!m_bChkCamera[nCamNo])
    {
        return SP_OK;
    }

    LPCSTR   CAM_TYPE_CMD[] = {"AT+CAM0ID",   "AT+CAM1ID"  };
    LPCSTR   CAM_SIZE_CMD[] = {"AT+CAM0SIZE", "AT+CAM1SIZE"};
    SPRESULT CAM_TYPE_ERR[] = {SP_E_CHECK_MAINCAMERA_TYPE_ERROR, SP_E_CHECK_PRECAMERA_TYPE_ERROR};
    SPRESULT CAM_SIZE_ERR[] = {SP_E_CHECK_MAINCAMERA_SIZE_ERROR, SP_E_CHECK_PRECAMERA_SIZE_ERROR};

    CHAR   szName[32] = {0};

    // Camera ID
    sprintf_s(szName, "CAM%dID", nCamNo);
    SPRESULT res = SendATCommand(CAM_TYPE_CMD[nCamNo]);
    if (SP_OK != res)
    {
        NOTIFY(szName, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, CAM_TYPE_CMD[nCamNo]);
        return res;
    }
    else
    {
        if (NULL == strstr(m_strRevBuf.c_str(), m_strCameraType[nCamNo].c_str()))
        {
            NOTIFY(szName, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, m_strRevBuf.c_str());
            return CAM_TYPE_ERR[nCamNo];
        }
        else
        {
            NOTIFY(szName, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, m_strRevBuf.c_str());
        }
    }

    sprintf_s(szName, "CAM%dSIZE", nCamNo);
    res = SendATCommand(CAM_SIZE_CMD[nCamNo]);
    if (SP_OK != res)
    {
        NOTIFY(szName, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, CAM_SIZE_CMD[nCamNo]);
        return res;
    }
    else
    {
        if (NULL == strstr(m_strRevBuf.c_str(), m_strCameraSize[nCamNo].c_str()))
        {
            NOTIFY(szName, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, m_strRevBuf.c_str());
            return CAM_SIZE_ERR[nCamNo];
        }
        else
        {
            NOTIFY(szName, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, m_strRevBuf.c_str());
        }
    }

    return SP_OK;
}
